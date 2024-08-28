#pragma once

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <string>

#include "Icon.hpp"

namespace Vicetrice
{
	/**
	 * @brief Enumeration for different resize types.
	 */
	enum class ResizeTypes
	{
		NORESIZE,   /// No resizing
		RXRESIZE,   /// Resize on the right side
		LXRESIZE,   /// Resize on the left side
		UYRESIZE,   /// Resize on the upper side
		DYRESIZE,   /// Resize on the bottom side
		RXUYRESIZE, /// Resize on the upper right corner
		RXDYRESIZE, /// Resize on the bottom right corner
		LXUYRESIZE, /// Resize on the upper left corner
		LXDYRESIZE  /// Resize on the bottom left corner
	};

	/**
	 * @brief Class representing a window with icons that can be dragged, resized, and rendered.
	 */
	class Window
	{

	public:

		/**
		 * @brief Constructor for the Window class.
		 *
		 * @param ContextWidth Width of the context (window).
		 * @param ContextHeight Height of the context (window).
		 * @param shPath Path to the shader files.
		 */
		Window(int ContextWidth, int ContextHeight);

		/**
		 * @brief Destructor for the Window class.
		 */
		~Window();

		/**
		 * @brief Adjusts the projection matrix based on the new context dimensions.
		 *
		 * @param ContextWidth New width of the context.
		 * @param ContextHeight New height of the context.
		 */
		void AdjustProj(int ContextWidth, int ContextHeight);

		/**
		 * @brief Enables or disables dragging of the window based on mouse events.
		 *
		 * @param context Pointer to the GLFW window context.
		 * @param button Mouse button involved in the action.
		 * @param action Action taken (e.g., press, release).
		 */
		void DragON(GLFWwindow* context, int button, int action);

		/**
		 * @brief Moves the window based on the mouse position.
		 *
		 * @param xpos X position of the mouse.
		 * @param ypos Y position of the mouse.
		 */
		void Move(double xpos, double ypos);

		/**
		 * @brief Draws the window and its icons on the screen.
		 */
		void Draw();

		/**
		 * @brief Resizes the window based on mouse position.
		 *
		 * @param context Pointer to the GLFW window context.
		 * @param xpos X position of the mouse.
		 * @param ypos Y position of the mouse.
		 */
		void Resize(GLFWwindow* context, double xpos, double ypos);

		/**
		 * @brief Adds an icon to the window.
		 */
		void addIcon();

		/**
		 * @brief Removes an icon from the window.
		 */
		void RemoveIcon();

		/**
		 * @brief Checks if the window is currently being dragged.
		 *
		 * @return True if the window is being dragged, otherwise false.
		 */
		inline bool Dragging() const
		{
			return m_dragging;
		}

		/**
		 * @brief Checks if the window is currently being rendered.
		 *
		 * @return True if the window is being rendered, otherwise false.
		 */
		inline bool Rendering() const
		{
			return m_render;
		}

		/**
		 * @brief Returns the current model matrix of the window.
		 *
		 * @return The model matrix.
		 */
		inline glm::mat4 ModelMatrix() const
		{
			return m_model;
		}

		/**
		 * @brief Provides access to the vertex data of the window.
		 *
		 * @return A reference to the vector containing vertex data.
		 */
		inline std::vector<float>& Vertices()
		{
			return m_vertex;
		}

		/**
		 * @brief Provides access to the index data of the window.
		 *
		 * @return A reference to the vector containing index data.
		 */
		inline std::vector<unsigned int>& Indices()
		{
			return m_indices;
		}



	private:


		//---------------------------------------- ATTRIBUTES

		glm::mat4 m_model;             /// Model matrix of the window.
		glm::mat4 m_proj;              /// Projection matrix of the window.

		bool m_dragging;               /// Flag indicating if the window is being dragged.
		bool m_render;                 /// Flag indicating if the window is being rendered.

		int m_ContextWidth;            /// Width of the window context.
		int m_ContextHeight;           /// Height of the window context.

		double m_lastMouseX;           /// Last X position of the mouse.
		double m_lastMouseY;           /// Last Y position of the mouse.

		ResizeTypes m_resize;          /// Current resize type.
		bool m_moving;                 /// Flag indicating if the window is moving.

		std::vector<float> m_vertex;   /// Vertex data of the window.
		std::vector<unsigned int> m_indices; /// Index data of the window.

		unsigned int m_IndexToFirstIconToRender; /// Index of the first icon to be rendered.
		unsigned int m_MaxIconsToRender; /// Maximum number of icons to render.

		float m_WindowLimits[4];       /// Array containing the window limits.

		// Window
		VertexArray m_va;              /// Vertex array object for the window.
		VertexBuffer m_vb;             /// Vertex buffer object for the window.
		Shader m_shader;               /// Shader object for the window.
		IndexBuffer m_ib;              /// Index buffer object for the window.

		// Icons
		VertexArray m_vaI;             /// Vertex array object for the icons.
		VertexBuffer m_vbI;            /// Vertex buffer object for the icons.
		Shader m_shaderI;              /// Shader object for the icons.
		IndexBuffer m_ibI;             /// Index buffer object for the icons.

		std::vector<Icon> m_icons;     /// Vector containing all icons in the window.


		bool m_SliderEnable;
		float m_SlideLimits[4];

		// TODO: ADD TO SLIDERICON CLASS
		glm::mat4 m_SliderModel;
		bool m_sliding;


		//---------------------------------------- PRIVATE METHODS

		/**
		 * @brief Normalizes mouse coordinates from screen space to OpenGL space.
		 *
		 * @param mouseX X position of the mouse in screen space.
		 * @param mouseY Y position of the mouse in screen space.
		 * @param normalizedX Reference to store the normalized X position.
		 * @param normalizedY Reference to store the normalized Y position.
		 */
		void NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const;

		/**
		 * @brief Checks if the mouse is inside the window's object.
		 *
		 * @param normalizedMouseX Normalized X position of the mouse.
		 * @param normalizedMouseY Normalized Y position of the mouse.
		 * @return True if the mouse is inside the object, otherwise false.
		 */
		bool IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const;

		/**
		 * @brief Helper function to check if a point is within limits, considering a tolerance (epsilon).
		 *
		 * @param epsilon The tolerance.
		 * @param point The point to check.
		 * @param limit The limit to check against.
		 * @return True if the point is within the limits, otherwise false.
		 */
		inline bool IsInBetween(float epsilon, float point, float limit) const
		{
			return (point >= limit - epsilon && point <= limit + epsilon);
		}

		/**
		 * @brief Draws an icon on the window.
		 */
		void DrawIcon();

		/**
		 * @brief Checks and adjusts the window resizing based on mouse position.
		 *
		 * @param context Pointer to the GLFW window context.
		 * @param normalizedMouseX Normalized X position of the mouse.
		 * @param normalizedMouseY Normalized Y position of the mouse.
		 */
		void CheckResize(GLFWwindow* context, float normalizedMouseX, float normalizedMouseY);

		/**
		 * @brief Initializes the vertex data for the window.
		 *
		 * @return Pointer to the vertex data array.
		 */
		float* IniVertex();

		/**
		 * @brief Initializes the index data for the window.
		 *
		 * @return Pointer to the index data array.
		 */
		unsigned int* IniIndex();

		/**
		 * @brief Renders an icon on the window.
		 */
		void RenderIcon();

		/**
		 * @brief Updates the limits of the window based on its current position and size.
		 */
		void updateLimits();

		bool CheckSlide(float normalizedMouseX, float  normalizedMouseY);

	}; // class Window

} // namespace Vicetrice
