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
		m_mouseInside{ false }, m_resize{ ResizeTypes::NORESIZE }
	{
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
		return normalizedMouseX >= m_model[3].x - (0.5f * m_model[0][0]) - epsilon &&
			normalizedMouseX <= m_model[3].x + (0.5f * m_model[0][0]) + epsilon &&
			normalizedMouseY >= m_model[3].y - (0.5f * m_model[1][1]) - epsilon &&
			normalizedMouseY <= m_model[3].y + (0.5f * m_model[1][1]) + epsilon;
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
					if (m_resize == ResizeTypes::NORESIZE)
						m_resize = CheckResize(context, normalizedMouseX, normalizedMouseY);
				}
			}
			else if (action == GLFW_RELEASE) {
				m_dragging = false;
				m_resize = ResizeTypes::NORESIZE;
			}
		}
	}

	void Window::Move(double xpos, double ypos)
	{
		if (m_dragging && m_resize == ResizeTypes::NORESIZE)
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

	void Window::Draw(const Shader& shader, const VertexArray& va, const IndexBuffer& ib) {
		shader.Bind();
		va.Bind();
		ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
		m_render = false;
	}

	ResizeTypes Window::CheckResize(GLFWwindow* context, float normalizedMouseX, float  normalizedMouseY)
	{
		const float epsilon = 0.01f;


		if (IsinLimit(epsilon, normalizedMouseX, m_model[3].x - (0.5f * m_model[0][0])))
		{
			glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
			return ResizeTypes::LXRESIZE;

		}
		if (IsinLimit(epsilon, normalizedMouseX, m_model[3].x + (0.5f * m_model[0][0])))
		{
			glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
			return ResizeTypes::RXRESIZE;
		}
		if (IsinLimit(epsilon, normalizedMouseY, m_model[3].y - (0.5f * m_model[1][1])))
		{
			glfwSetCursor(context, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
			return ResizeTypes::DYRESIZE;
		}
		if (IsinLimit(epsilon, normalizedMouseY, m_model[3].y + (0.5f * m_model[1][1])))
		{
			glfwSetCursor(context, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
			return ResizeTypes::UYRESIZE;
		}


		glfwSetCursor(context, NULL);
		return ResizeTypes::NORESIZE;

	}

	void Window::Resize(GLFWwindow* context, double xpos, double ypos)
	{
		float normalizedMouseX, normalizedMouseY;
		NormalizeMouseCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);


	
		// Verifica si se debe redimensionar
		CheckResize(context, normalizedMouseX, normalizedMouseY);

		if (m_dragging && m_resize != ResizeTypes::NORESIZE)
		{
			// Cálculo del desplazamiento del ratón en pantalla
			float deltaX = static_cast<float>(xpos - m_lastMouseX);
			float deltaY = static_cast<float>(ypos - m_lastMouseY);

			// Convertir el desplazamiento a coordenadas normalizadas
			float deltaXNorm = (2.0f * deltaX / m_ContextWidth);
			float deltaYNorm = -(2.0f * deltaY / m_ContextHeight);

			// Actualiza la escala del objeto basado en el desplazamiento del ratón
			glm::mat4 dummy(m_model);

			switch (m_resize)
			{
			case Vicetrice::ResizeTypes::RXRESIZE:
				dummy = glm::translate(dummy, glm::vec3(deltaXNorm / 2.0f, 0.0f, 0.0f));  // Mantener el lado izquierdo
				dummy = glm::scale(dummy, glm::vec3(deltaXNorm + 1.0f, 1.0f, 1.0f));
				break;
			case Vicetrice::ResizeTypes::LXRESIZE:
				dummy = glm::translate(dummy, glm::vec3(deltaXNorm / 2.0f, 0.0f, 0.0f)); // Mantener el lado derecho
				dummy = glm::scale(dummy, glm::vec3(-deltaXNorm + 1.0f, 1.0f, 1.0f));
				break;
			case Vicetrice::ResizeTypes::UYRESIZE:
				dummy = glm::translate(dummy, glm::vec3(0.0f, deltaYNorm / 2.0f, 0.0f));  // Mantener el lado inferior
				dummy = glm::scale(dummy, glm::vec3(1.0f, deltaYNorm + 1.0f, 1.0f));
				break;
			case Vicetrice::ResizeTypes::DYRESIZE:
				dummy = glm::translate(dummy, glm::vec3(0.0f, deltaYNorm / 2.0f, 0.0f)); // Mantener el lado superior
				dummy = glm::scale(dummy, glm::vec3(1.0f, -deltaYNorm + 1.0f, 1.0f));
				break;
			default:
				break;
			}

			if (dummy[0][0] > 0.25f && dummy[1][1] > 0.1f)
			{
				m_model = dummy;
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
