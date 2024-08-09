#pragma once

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"

namespace Vicetrice
{
	enum class ResizeTypes
	{
		NORESIZE,
		RXRESIZE,
		LXRESIZE,
		UYRESIZE,
		DYRESIZE
	};

	class Window
	{
	public:

		Window(int ContextWidth, int ContextHeight);
		~Window();

		void AdjustProj(int ContextWidth, int ContextHeight);

		void DragON(GLFWwindow* context, int button, int action);

		void Move(double xpos, double ypos);

		void Draw(const Shader& shader, const VertexArray& va, const IndexBuffer& ib);

		void Resize(GLFWwindow* context, double xpos, double ypos);

		inline bool Dragging()
		{
			return m_dragging;
		}
		inline bool Rendering()
		{
			return m_render;
		}
		inline glm::mat4 ModelMatrix()
		{
			return m_model;
		}


	private:

		glm::mat4 m_model;

		bool m_dragging;
		bool m_render;

		int m_ContextWidth;
		int	m_ContextHeight;

		double	m_lastMouseX;
		double	m_lastMouseY;

		bool m_mouseInside;
		ResizeTypes m_resize;


		void NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const;

		bool IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const;

		bool IsinLimit(float epsilon, float point, float limit) const;

		ResizeTypes CheckResize(GLFWwindow* context, float normalizedMouseX, float normalizedMouseY);


	}; //class Window


} //namespace Vicetrice
