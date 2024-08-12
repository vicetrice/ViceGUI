#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <iostream>
#include <string>

namespace Vicetrice
{
	static const float epsilon = 0.01f;

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

		m_vertex = {
			//Position	 //VertexID
			-0.5f, -0.5f, 0.0f, // 0-LD
			 0.5f, -0.5f, 1.0f, // 1-RD
			 0.5f,  0.5f, 2.0f,	// 2-RU
			-0.5f,  0.5f ,3.0f	// 3-LU
		};

	}

	Window::~Window() {
	}


	void Window::AdjustProj(int ContextWidth, int ContextHeight) {

		m_ContextWidth = ContextWidth;
		m_ContextHeight = ContextHeight;
		m_render = true;
	}

	void Window::NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const
	{
		normalizedX = (2.0f * (static_cast<float>(mouseX) / m_ContextWidth)) - 1.0f;
		normalizedY = 1.0f - (2.0f * (static_cast<float>(mouseY) / m_ContextHeight));
	}

	bool Window::IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const
	{
		return (normalizedMouseX >= m_model[3].x + m_VertexSpecific[0] + m_vertex[0] - epsilon &&
			normalizedMouseX <= m_model[3].x + m_VertexSpecific[2] + m_vertex[3] + epsilon &&
			normalizedMouseY >= m_model[3].y + m_VertexSpecific[1] + m_vertex[1] - epsilon &&
			normalizedMouseY <= m_model[3].y + m_VertexSpecific[3] + m_vertex[7] + epsilon);
	}

	void Window::DragON(GLFWwindow* context, int button, int action)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			double mouseX, mouseY;
			glfwGetCursorPos(context, &mouseX, &mouseY);

			float normalizedMouseX, normalizedMouseY;
			NormalizeMouseCoords(mouseX, mouseY, normalizedMouseX, normalizedMouseY);
			if (action == GLFW_PRESS)
			{
				if (IsMouseInsideObject(normalizedMouseX, normalizedMouseY))
				{
					m_dragging = true;
					m_lastMouseX = normalizedMouseX;
					m_lastMouseY = normalizedMouseY;
					CheckResize(context, normalizedMouseX, normalizedMouseY);
					if (m_resize == ResizeTypes::NORESIZE)
						m_moving = true;

				}
			}
			else if (action == GLFW_RELEASE) {
				m_dragging = false;
				m_moving = false;
				CheckResize(context, normalizedMouseX, normalizedMouseY);
			}
		}
	}

	void Window::Move(double xpos, double ypos)
	{
		if (m_dragging && m_resize == ResizeTypes::NORESIZE && m_moving)
		{
			float normalizedMouseX, normalizedMouseY;
			NormalizeMouseCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);
			float deltaXNorm = static_cast<float>(normalizedMouseX - m_lastMouseX);
			float deltaYNorm = static_cast<float>(normalizedMouseY - m_lastMouseY);

			m_model = glm::translate(m_model, glm::vec3(deltaXNorm, deltaYNorm, 0.0f));

			m_lastMouseX = normalizedMouseX;
			m_lastMouseY = normalizedMouseY;
		}
	}

	void Window::Draw()
	{



		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};



		VertexArray va;
		VertexBuffer vb(m_vertex.data(), sizeof(float) * m_vertex.size());
		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(1);
		va.addBuffer(vb, layout);

		Shader shader("res/shaders/Window.shader");

		IndexBuffer ib(indices, sizeof(indices));

		shader.Bind();
		shader.SetUniformMat4f("u_MVP", m_model);
		shader.SetUniform4f("u_VertexSpecific", m_VertexSpecific[0], m_VertexSpecific[1], m_VertexSpecific[2], m_VertexSpecific[3]);
		va.Bind();
		ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
		m_render = false;
	}

	void Window::CheckResize(GLFWwindow* context, float normalizedMouseX, float  normalizedMouseY)
	{

		if (IsMouseInsideObject(normalizedMouseX, normalizedMouseY) && !m_dragging)
		{
			bool IsInRx = IsInBetween(epsilon, normalizedMouseX, m_model[3].x + m_VertexSpecific[2] + m_vertex[3]);
			bool IsInLx = IsInBetween(epsilon, normalizedMouseX, m_model[3].x + m_VertexSpecific[0] + m_vertex[0]);
			bool IsInUy = IsInBetween(epsilon, normalizedMouseY, m_model[3].y + m_VertexSpecific[3] + m_vertex[7]);
			bool IsInDy = IsInBetween(epsilon, normalizedMouseY, m_model[3].y + m_VertexSpecific[1] + m_vertex[1]);



			if (IsInLx && IsInUy)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR));
				m_resize = ResizeTypes::LXUYRESIZE;
				return;
			}
			else if (IsInLx && IsInDy)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR));
				m_resize = ResizeTypes::LXDYRESIZE;
				return;
			}
			else if (IsInRx && IsInUy)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR));
				m_resize = ResizeTypes::RXUYRESIZE;
				return;
			}
			else if (IsInRx && IsInDy)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR));
				m_resize = ResizeTypes::RXDYRESIZE;
				return;
			}
			else if (IsInLx)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
				m_resize = ResizeTypes::LXRESIZE;
				return;

			}
			else if (IsInRx)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
				m_resize = ResizeTypes::RXRESIZE;
				return;
			}
			else if (IsInDy)
			{
				glfwSetCursor(context, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
				m_resize = ResizeTypes::DYRESIZE;
				return;
			}
			else if (IsInUy)
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
		float normalizedMouseX = 0.0f;
		float normalizedMouseY = 0.0f;

		if (!m_moving)
		{
			NormalizeMouseCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);
			CheckResize(context, normalizedMouseX, normalizedMouseY);
		}

		if (m_dragging && m_resize != ResizeTypes::NORESIZE && !m_moving)
		{

			float deltaXNorm = static_cast<float>(normalizedMouseX - m_lastMouseX);
			float deltaYNorm = static_cast<float>(normalizedMouseY - m_lastMouseY);



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
			case Vicetrice::ResizeTypes::RXDYRESIZE:
				m_VertexSpecific[2] += deltaXNorm;
				m_VertexSpecific[1] += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::RXUYRESIZE:
				m_VertexSpecific[2] += deltaXNorm;
				m_VertexSpecific[3] += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::LXDYRESIZE:
				m_VertexSpecific[0] += deltaXNorm;
				m_VertexSpecific[1] += deltaYNorm;

				break;
			case Vicetrice::ResizeTypes::LXUYRESIZE:
				m_VertexSpecific[0] += deltaXNorm;
				m_VertexSpecific[3] += deltaYNorm;
				break;

			default:
				break;
			}

			// Actualiza la última posición del ratón
			m_lastMouseX = normalizedMouseX;
			m_lastMouseY = normalizedMouseY;
		}
	}

	void Window::addIcon(std::string name)
	{

	}

} // namespace Vicetrice