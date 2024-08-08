#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"


namespace Vicetrice
{
	Window::Window(int WindowWidth, int WindowHeight) : m_proj{ 1.0f }, m_view{ 1.0f }, m_model{ 1.0f }, m_mvp{ 1.0f }, m_dragging{ false }, m_render{ true }
		, m_windowWidth{ WindowWidth }, m_windowHeight{ WindowHeight }, m_lastMouseX{ 0.0 }, m_lastMouseY{ 0.0 }, translation{ 0.0f,0.0f,0.0f }
	{
	}

	Window::~Window()
	{

	}

	void Window::Resize(int ContextWidth, int ContextHeight, int width, int height)
	{
		m_proj = glm::ortho(
			-1.0f * (float)width / ContextWidth,
			1.0f * (float)width / ContextWidth,
			-1.0f * (float)height / ContextHeight,
			1.0f * (float)height / ContextHeight,
			-1.0f, 1.0f);
		m_windowWidth = ContextWidth;
		m_windowHeight = ContextHeight;
		m_render = true;
	}

	void Window::Drag(GLFWwindow* window, int button, int action)
	{
		double mouseX;
		double mouseY;

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{

			if (action == GLFW_PRESS)
			{
				glfwGetCursorPos(window, &mouseX, &mouseY);
				float normalizedMouseX;
				float normalizedMouseY;
				// Convertir las coordenadas del ratón a coordenadas normalizadas (-1 a 1)
				normalizedMouseX = (2.0f * (static_cast<float>(mouseX) / m_windowWidth)) - 1.0f;
				normalizedMouseY = 1.0f - (2.0f * (static_cast<float>(mouseY) / m_windowHeight));

				// Verificar si el ratón está dentro del área del objeto
				bool mouseInside = (normalizedMouseX >= translation.x - 0.5f &&
					normalizedMouseX <= translation.x + 0.5f &&
					normalizedMouseY >= translation.y - 0.5f &&
					normalizedMouseY <= translation.y + 0.5f);

				if (mouseInside)
				{
					m_dragging = true;
					m_lastMouseX = mouseX;
					m_lastMouseY = mouseY;

				}
			}
			else if (action == GLFW_RELEASE) {
				m_dragging = false;
			}
		}
	}

	void Window::Move(double xpos, double ypos)
	{

		if (m_dragging)
		{
			// Calcular el desplazamiento del ratón
			float deltaX = static_cast<float>(xpos - m_lastMouseX);
			float deltaY = static_cast<float>(ypos - m_lastMouseY);

			// Convertir el desplazamiento a coordenadas normalizadas
			float deltaXNorm = (2.0f * deltaX / m_windowWidth);
			float deltaYNorm = -(2.0f * deltaY / m_windowHeight);

			translation += glm::vec3(deltaXNorm, deltaYNorm, 0.0f);
			m_view = glm::translate(glm::mat4(1.0f), translation);
			m_mvp = m_proj * m_view * m_model;

			// Actualizar la última posición del ratón
			m_lastMouseX = xpos;
			m_lastMouseY = ypos;
		}

	}

	void Window::Draw(const Shader& shader, const VertexArray& va, const IndexBuffer& ib)
	{

		shader.Bind();
		va.Bind();
		ib.Bind();

		// Dibujar el objeto
		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
		// Intercambiar buffers y esperar eventos

		m_render = false;

	}

}//namespace Vicetrice
