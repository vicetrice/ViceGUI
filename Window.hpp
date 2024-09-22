#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//STD
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <random>
#include <chrono>

//VENDOR
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"


//CUSTOM
#include "Icon.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"

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
		struct Vertex
		{
			glm::vec2 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			int		  TexID;
		};

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
		void addIcon(const std::string &NameToDisplay);

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

		///**
		// * @brief Provides access to the vertex data of the window.
		// *
		// * @return A reference to the vector containing vertex data.
		// */
		//inline std::vector<glm::vec2>& Vertices()
		//{
		//	return m_vertexPositions;
		//}
		//
		///**
		// * @brief Provides access to the index data of the window.
		// *
		// * @return A reference to the vector containing index data.
		// */
		//inline std::vector<unsigned int>& Indices()
		//{
		//	return m_indices;
		//}



	private:


		//---------------------------------------- ATTRIBUTES
		struct Limits
		{
			float right;
			float left;
			float up;
			float down;
		};
		
		// Window
		glm::mat4 m_model;             /// Model matrix of the window.
		glm::mat4 m_proj;

		bool m_dragging;               /// Flag indicating if the window is being dragged.
		bool m_render;                 /// Flag indicating if the window is being rendered.

		int m_BaseContextWidth;        /// Initial Width of the window context.
		int m_BaseContextHeight;       /// Initial Height of the window context.
		int m_ContextWidth;			   /// Current Width of the window context.
		int m_ContextHeight;		   /// Current Height of the window context.

		double m_lastMouseX;           /// Last X position of the mouse.
		double m_lastMouseY;           /// Last Y position of the mouse.

		ResizeTypes m_resize;          /// Current resize type.
		bool m_moving;                 /// Flag indicating if the window is moving.

		
		Limits m_WindowLimits;       
		Renderer m_WindowRenderer;
		Shader m_WindowShader;

		// Icons
		unsigned int m_IndexToFirstIconToRender; /// Index of the first icon to be rendered.
		unsigned int m_MaxIconsToRender; /// Maximum number of icons to render.
		unsigned int m_TotalVerticesToRender;

		std::vector<Icon<Vertex> *> m_icons;     /// Vector containing all icons in the window.

		bool m_SliderEnable;
		Limits m_SlideLimits;

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
		void NormalizeCoords(double X, double Y, float& normalizedX, float& normalizedY) const;

		/**
		 * @brief Denormalizes coordinates from OpenGL space back to screen space.
		 *
		 * @param normalizedX Normalized X coordinate (from -1.0 to 1.0 in OpenGL space).
		 * @param normalizedY Normalized Y coordinate (from -1.0 to 1.0 in OpenGL space).
		 * @param X Reference to store the screen space X coordinate.
		 * @param Y Reference to store the screen space Y coordinate.
		 */
		void DeNormalizeCoords(float normalizedX, float normalizedY, double& X, double& Y) const;
		
		
		/**
		 * @brief Checks if the mouse is inside the window's object.
		 *
		 * @param normalizedMouseX Normalized X position of the mouse.
		 * @param normalizedMouseY Normalized Y position of the mouse.
		 * @return True if the mouse is inside the object, otherwise false.
		 */
		bool IsMouseInsideWindow(float normalizedMouseX, float normalizedMouseY) const;

		bool IsMouseInsideMovingPart(float normalizedMouseX, float normalizedMouseY) const;
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
		 * @brief Updates the limits of the window based on its current position and size.
		 */
		void updateWindowLimits();

		bool CheckSlide(float normalizedMouseX, float  normalizedMouseY);


		
		void GenerateTextVertices(std::vector<Vertex>& VertexStorage ,const std::string& text, float normalizedX, float normalizedY, float scale);

		void CreateWindowFrame();

		void ConfigWindowShader();

		void updateHardWposition();

		void updateSlider();

		unsigned int CalculateMaxIconsToRender();

		float UpdateSliderLimits();



	}; // class Window

} // namespace Vicetrice

