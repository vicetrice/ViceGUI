#pragma once

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"

namespace Vicetrice
{

	class Window
	{
	public:

		Window(int WindowWidth, int WindowHeight);
		~Window();

		void Resize(int ContextWidth, int ContextHeight, int width, int height);

		void Drag(GLFWwindow* window, int button, int action);

		void Move(double xpos, double ypos);

		void Draw(const Shader& shader, const VertexArray& va, const IndexBuffer& ib);
		inline bool Dragging()
		{
			return m_dragging;
		}
		inline bool Rendering()
		{
			return m_render;
		}
		inline glm::mat4 MVP()
		{
			return m_mvp;
		}


	private:
		glm::mat4 m_proj;
		glm::mat4 m_view;
		glm::mat4 m_model;
		glm::mat4 m_mvp;


		bool m_dragging;
		bool m_render;

		int m_windowWidth;
		int	m_windowHeight;

		double	m_lastMouseX;
		double	m_lastMouseY;

		glm::vec3 translation{ 0.0f, 0.0f, 0.0f };



	}; //class Window


} //namespace Vicetrice
