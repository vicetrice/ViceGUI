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
		DYRESIZE,
		RXUYRESIZE,
		RXDYRESIZE,
		LXUYRESIZE,
		LXDYRESIZE
	};

	class Window
	{
	public:

		Window(int ContextWidth, int ContextHeight);
		~Window();

		void AdjustProj(int ContextWidth, int ContextHeight);

		void DragON(GLFWwindow* context, int button, int action);

		void Move(double xpos, double ypos);

		void Draw();

		void Resize(GLFWwindow* context, double xpos, double ypos);

		inline bool Dragging() const
		{
			return m_dragging;
		}
		inline bool Rendering() const
		{
			return m_render;
		}
		inline glm::mat4 ModelMatrix() const
		{
			return m_model;
		}

		inline const float* vertexSpecific() const
		{
			return m_VertexSpecific;
		}


	private:

		glm::mat4 m_model;
		float m_VertexSpecific[4];

		bool m_dragging;
		bool m_render;

		int m_ContextWidth;
		int	m_ContextHeight;

		double	m_lastMouseX;
		double	m_lastMouseY;

		ResizeTypes m_resize;
		bool m_moving;


		void NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const;

		bool IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const;

		inline bool IsInBetween(float epsilon, float point, float limit) const
		{
			return (point >= limit - epsilon && point <= limit + epsilon);
		}

		void CheckResize(GLFWwindow* context, float normalizedMouseX, float normalizedMouseY);




	}; //class Window


} //namespace Vicetrice