#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <iostream>

namespace Vicetrice {

	Window::Window(int ContextWidth, int ContextHeight)
		: m_model{ 1.0f },
		m_dragging{ false }, m_render{ true }, m_ContextWidth{ ContextWidth },
		m_ContextHeight{ ContextHeight }, m_lastMouseX{ 0.0 }, m_lastMouseY{ 0.0 },
		m_resize{ ResizeTypes::NORESIZE }, m_moving{ false }
	{
		for (size_t i = 0; i < sizeof(m_VertexSpecific) / sizeof(float); i++)
		{
			m_VertexSpecific[i] = 0;
		}
	}

	Window::~Window() {
	}

	void Window::AdjustProj(int ContextWidth, int ContextHeight) {

		m_ContextWidth = ContextWidth;
		m_ContextHeight = ContextHeight;
		m_render = true;
	}

	void Window::NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const {
		normalizedX = (2.0f * (static_cast<float>(mouseX) / m_ContextWidth)) - 1.0f;
		normalizedY = 1.0f - (2.0f * (static_cast<float>(mouseY) / m_ContextHeight));
	}

	bool Window::IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const
	{
		const float epsilon = 0.01f;
		return normalizedMouseX >= m_model[3].x + m_VertexSpecific[0] - (0.5f * m_model[0][0]) - epsilon &&
			normalizedMouseX <= m_model[3].x + m_VertexSpecific[2] + (0.5f * m_model[0][0]) + epsilon &&
			normalizedMouseY >= m_model[3].y + m_VertexSpecific[1] - (0.5f * m_model[1][1]) - epsilon &&
			normalizedMouseY <= m_model[3].y + m_VertexSpecific[3] + (0.5f * m_model[1][1]) + epsilon;
	}

	void Window::DragON(GLFWwindow* context, int button, int action)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
			{
				double mouseX, mouseY;
				glfwGetCursorPos(context, &mouseX, &mouseY);

				float normalizedMouseX, normalizedMouseY;
				NormalizeMouseCoords(mouseX, mouseY, normalizedMouseX, normalizedMouseY);

				if (IsMouseInsideObject(normalizedMouseX, normalizedMouseY))
				{
					m_dragging = true;
					m_lastMouseX = mouseX;
					m_lastMouseY = mouseY;
					CheckResize(context, normalizedMouseX, normalizedMouseY);
					if (m_resize == ResizeTypes::NORESIZE)
						m_moving = true;

				}
			}
			else if (action == GLFW_RELEASE) {
				m_dragging = false;
				m_moving = false;
				m_resize = ResizeTypes::NORESIZE;
			}
		}
	}

	void Window::Move(double xpos, double ypos)
	{
		if (m_dragging && m_resize == ResizeTypes::NORESIZE && m_moving)
		{
			float deltaX = static_cast<float>(xpos - m_lastMouseX);
			float deltaY = static_cast<float>(ypos - m_lastMouseY);

			float deltaXNorm, deltaYNorm;

			deltaXNorm = (2.0f * deltaX / m_ContextWidth);
			deltaYNorm = -(2.0f * deltaY / m_ContextHeight);


			m_model = glm::translate(m_model, glm::vec3(deltaXNorm / m_model[0][0], deltaYNorm / m_model[1][1], 0.0f));

			m_lastMouseX = xpos;
			m_lastMouseY = ypos;
		}
	}

	void Window::Draw(const Shader& shader, const VertexArray& va, const IndexBuffer& ib)
	{
		shader.Bind();
		va.Bind();
		ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
		m_render = false;
	}

	void Window::CheckResize(GLFWwindow* context, float normalizedMouseX, float  normalizedMouseY)
	{
		const float epsilon = 0.01f;

		if (IsMouseInsideObject(normalizedMouseX, normalizedMouseY))
		{
			if (IsinLimit(epsilon, normalizedMouseX, m_model[3].x + m_VertexSpecific[0] - (0.5f * m_model[0][0])))
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
				m_resize = ResizeTypes::LXRESIZE;
				return;

			}
			if (IsinLimit(epsilon, normalizedMouseX, m_model[3].x + m_VertexSpecific[2] + (0.5f * m_model[0][0])))
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
				m_resize = ResizeTypes::RXRESIZE;
				return;
			}
			if (IsinLimit(epsilon, normalizedMouseY, m_model[3].y + m_VertexSpecific[1] - (0.5f * m_model[1][1])))
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
				m_resize = ResizeTypes::DYRESIZE;
				return;
			}
			if (IsinLimit(epsilon, normalizedMouseY, m_model[3].y + m_VertexSpecific[3] + (0.5f * m_model[1][1])))
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
				m_resize = ResizeTypes::UYRESIZE;
				return;
			}


		}

		if (!m_dragging)
		{
			glfwSetCursor(context, NULL);
			m_resize = ResizeTypes::NORESIZE;
		}


	}

	void Window::Resize(GLFWwindow* context, double xpos, double ypos)
	{
		float normalizedMouseX, normalizedMouseY;
		NormalizeMouseCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);

		// Verifica si se debe redimensionar
		if (!m_moving)
			CheckResize(context, normalizedMouseX, normalizedMouseY);

		if (m_dragging && m_resize != ResizeTypes::NORESIZE)
		{
			// Cálculo del desplazamiento del ratón en pantalla
			float deltaX = static_cast<float>(xpos - m_lastMouseX);
			float deltaY = static_cast<float>(ypos - m_lastMouseY);

			// Convertir el desplazamiento a coordenadas normalizadas
			float deltaXNorm = (2.0f * deltaX / m_ContextWidth);
			float deltaYNorm = -(2.0f * deltaY / m_ContextHeight);


			// Aplicar la escala en función del tipo de redimensionamiento
			switch (m_resize)
			{
			case Vicetrice::ResizeTypes::RXRESIZE:
				m_VertexSpecific[2] += deltaXNorm;
				break;
			case Vicetrice::ResizeTypes::LXRESIZE:
				m_VertexSpecific[0] += deltaXNorm;
				break;
			case Vicetrice::ResizeTypes::UYRESIZE:
				m_VertexSpecific[3] += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::DYRESIZE:
				m_VertexSpecific[1] += deltaYNorm;
				break;
			default:
				break;
			}

			// Actualiza la última posición del ratón
			m_lastMouseX = xpos;
			m_lastMouseY = ypos;
		}
	}



	bool Window::IsinLimit(float epsilon, float point, float limit) const
	{
		return ((point >= limit - epsilon) && (point <= limit + epsilon));
	}

} // namespace Vicetrice
