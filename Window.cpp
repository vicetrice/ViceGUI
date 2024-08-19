#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <iostream>
#include <string>


#include "Icon.hpp"
#include <unordered_map>
#include <memory>




namespace Vicetrice
{
	static const float epsilon = 0.01f;
	static const unsigned int VerticesPerIcon = 12;
	static const unsigned int IndicesPerIcon = 6;


	//---------------------------------------- PUBLIC

	/**
	* @brief Constructor for the Window class.
	*
	* @param ContextWidth Width of the context (window).
	* @param ContextHeight Height of the context (window).
	* @param shPath Path to the shader files.
	*/
	Window::Window(int ContextWidth, int ContextHeight)
		: m_model{ 1.0f },
		m_dragging{ false },
		m_render{ true },
		m_ContextWidth{ ContextWidth },
		m_ContextHeight{ ContextHeight },
		m_lastMouseX{ 0.0 },
		m_lastMouseY{ 0.0 },
		m_resize{ ResizeTypes::NORESIZE },
		m_moving{ false },
		m_IndexToFirstIconToRender{ 0 },
		m_MaxIconsToRender{ 40 },
		m_va{},
		m_vb{ IniVertex(), static_cast <unsigned int> (sizeof(float) * m_vertex.size()) },
		m_shader{ "res/shaders/Window.shader" },
		m_ib{ IniIndex(),static_cast<unsigned int> (sizeof(unsigned int) * m_indices.size()) },
		m_vaI{},
		m_vbI{ nullptr,VerticesPerIcon * m_MaxIconsToRender * sizeof(float) },
		m_shaderI{ "res/shaders/Icon.shader" },
		m_ibI{ nullptr,IndicesPerIcon * m_MaxIconsToRender * sizeof(unsigned int) },
		m_SliderEnable{ false },
		m_SliderModel{ 1.0f }, //TODO: ADD IT TO CLASS SLIDERICON,
		m_sliding{ false }
	{

		VertexBufferLayout layout;

		layout.Push<float>(2);
		layout.Push<float>(4);
		layout.Push<float>(1);

		m_va.addBuffer(m_vb, layout);

		m_vaI.addBuffer(m_vbI, layout);

		updateLimits();

	}


	/**
	* @brief Destructor for the Window class.
	*/
	Window::~Window() {
	}

	/**
		 * @brief Adjusts the projection matrix based on the new context dimensions.
		 *
		 * @param ContextWidth New width of the context.
		 * @param ContextHeight New height of the context.
		 */
	void Window::AdjustProj(int ContextWidth, int ContextHeight) {

		m_ContextWidth = ContextWidth;
		m_ContextHeight = ContextHeight;
		m_render = true;
	}

	/**
	  * @brief Enables or disables dragging of the window based on mouse events.
	  *
	  * @param context Pointer to the GLFW window context.
	  * @param button Mouse button involved in the action.
	  * @param action Action taken (e.g., press, release).
	  */
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
					if (CheckSlide(normalizedMouseX, normalizedMouseY))
						m_sliding = true;
					else if (m_resize == ResizeTypes::NORESIZE)
						m_moving = true;

				}
			}
			else if (action == GLFW_RELEASE) {
				m_dragging = false;
				m_moving = false;
				m_sliding = false;
				CheckResize(context, normalizedMouseX, normalizedMouseY);
			}
		}
	}


	/**
		* @brief Moves the window based on the mouse position.
		*
		* @param xpos X position of the mouse.
		* @param ypos Y position of the mouse.
		*/
	void Window::Move(double xpos, double ypos)
	{
		if (m_dragging && m_resize == ResizeTypes::NORESIZE)
		{
			float normalizedMouseX, normalizedMouseY;
			NormalizeMouseCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);
			float deltaXNorm = static_cast<float>(normalizedMouseX - m_lastMouseX);
			float deltaYNorm = static_cast<float>(normalizedMouseY - m_lastMouseY);
			if (m_moving)
			{
				m_model = glm::translate(m_model, glm::vec3(deltaXNorm, deltaYNorm, 0.0f));
			}
			else if (m_SliderEnable && m_sliding)
			{
				m_SliderModel = glm::translate(m_SliderModel, glm::vec3(0.0f, deltaYNorm, 0.0f));
				RenderIcon();
			}
			updateLimits();

			m_lastMouseX = normalizedMouseX;
			m_lastMouseY = normalizedMouseY;
		}
	}


	/**
		 * @brief Draws the window and its icons on the screen.
		 */
	void Window::Draw()
	{
		/*bool first = true;

		for (size_t i = 0; i < m_vertex.size(); i++)
		{
			if (first)
			{
				std::cout << "VERTEX ARRAY\n" << "{";
				first = false;
			}
			else
			{
				std::cout << ", ";
			}
			if (i % 7 == 0 && i != 0)
			{
				std::cout << std::endl;
			}
			std::cout << m_vertex[i];
		}
		std::cout << "}" << std::endl;
		first = true;
		for (size_t i = 0; i < m_indices.size(); i++)
		{
			if (first)
			{
				std::cout << "INDEX ARRAY\n" << "{";
				first = false;
			}
			else
			{
				std::cout << ", ";
			}
			if (i % 3 == 0 && i != 0)
			{
				std::cout << std::endl;
			}
			std::cout << m_indices[i];
		}
		std::cout << "}" << std::endl;*/



		m_shader.Bind();
		m_shader.SetUniformMat4f("u_M", m_model);


		m_va.Bind();
		m_ib.Bind();
		GLCall(glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr));
		DrawIcon();
		m_render = false;
	}

	/**
	  * @brief Resizes the window based on mouse position.
	  *
	  * @param context Pointer to the GLFW window context.
	  * @param xpos X position of the mouse.
	  * @param ypos Y position of the mouse.
	  *
	  * TODO: ADD RESIZING LIMITS
	  */
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
				m_vertex[7] = m_vertex[14] += deltaXNorm;
				break;
			case Vicetrice::ResizeTypes::LXRESIZE:
				m_vertex[21] = m_vertex[0] += deltaXNorm;
				break;
			case Vicetrice::ResizeTypes::UYRESIZE:
				m_vertex[15] = m_vertex[22] += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::DYRESIZE:
				m_vertex[1] = m_vertex[8] += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::RXDYRESIZE:
				m_vertex[7] = m_vertex[14] += deltaXNorm;
				m_vertex[1] = m_vertex[8] += deltaYNorm;

				break;
			case Vicetrice::ResizeTypes::RXUYRESIZE:
				m_vertex[7] = m_vertex[14] += deltaXNorm;
				m_vertex[15] = m_vertex[22] += deltaYNorm;

				break;
			case Vicetrice::ResizeTypes::LXDYRESIZE:
				m_vertex[21] = m_vertex[0] += deltaXNorm;
				m_vertex[1] = m_vertex[8] += deltaYNorm;

				break;
			case Vicetrice::ResizeTypes::LXUYRESIZE:
				m_vertex[21] = m_vertex[0] += deltaXNorm;
				m_vertex[15] = m_vertex[22] += deltaYNorm;
				break;

			default:
				break;
			}
			updateLimits();

			m_vb.Update(m_vertex.data(), static_cast<unsigned int>(m_vertex.size() * sizeof(float)));
			RenderIcon();
			m_lastMouseX = normalizedMouseX;
			m_lastMouseY = normalizedMouseY;
		}
	}

	/**
		 * @brief Adds an icon to the window.
		 */
	void Window::addIcon()
	{
		m_icons.emplace_back(Icon());
		RenderIcon();
		m_render = true;
	}

	//---------------------------------------- PRIVATE

	/**
		* @brief Initializes the vertex data for the window.
		*
		* @return Pointer to the vertex data array.
		*/
	float* Window::IniVertex()
	{

		m_vertex = {
			//Position		//Color					//VertexID
			-0.5f, -0.5f,	0.8f,0.2f,0.9f,1.0f,	0.0f,	// 0-LD
			 0.5f, -0.5f,	0.8f,0.2f,0.9f,1.0f,	1.0f,	// 1-RD
			 0.5f,  0.5f,	0.8f,0.2f,0.9f,1.0f,	2.0f,	// 2-RU
			-0.5f,  0.5f,	0.8f,0.2f,0.9f,1.0f,	3.0f	// 3-LU
		};

		return m_vertex.data();
	}


	/**
	   * @brief Initializes the index data for the window.
	   *
	   * @return Pointer to the index data array.
	   */
	unsigned int* Window::IniIndex()
	{

		m_indices =
		{
			2, 0, 1,
			0, 3, 2
		};

		return m_indices.data();

	}

	/**
		* @brief Renders an icon on the window.
		*/
	void Window::RenderIcon()
	{
		if (m_icons.size() == 0)
		{
			return;
		}

		m_MaxIconsToRender = static_cast<unsigned int>(((m_WindowLimits[2] - m_WindowLimits[3]) / 0.1) + 1);

		//CAN BE ELIMINATED IF RESIZING LIMITS GOT DEFINED
		if (m_MaxIconsToRender > 30)
			m_MaxIconsToRender = 30;

		size_t limit;
		float SizeForSlider = 0.0f;
		m_SliderEnable = false;

		if (m_icons.size() > m_MaxIconsToRender - 2)
		{
			SizeForSlider = 0.05f;
			m_SliderEnable = true;
		}


		std::vector<float> IconsToRender;
		std::vector<unsigned int>IconsIndicesToRender;

		unsigned int IconCount = 1;


		IconsToRender = {

			//Position								//Color					//VertexID
			m_vertex[21],	m_vertex[22] - 0.1f,	0.0f,1.0f,1.0f,1.0f,	0.0f,
			m_vertex[14] - SizeForSlider,	m_vertex[15] - 0.1f,	0.0f,1.0f,1.0f,1.0f,	1.0f
		};

		limit = m_icons.size() > m_MaxIconsToRender ? m_MaxIconsToRender : m_icons.size();

		limit -= m_IndexToFirstIconToRender;

		for (size_t i = m_IndexToFirstIconToRender; i < limit; i++)
		{
			m_icons[i].AddToContext(IconsToRender, IconsIndicesToRender, 1, IconCount);
			++IconCount;
		}

		if (m_SliderEnable)
		{
			//TODO: Crear Vertices del slider a partir de m_SliderLimits
			float aux[] =
			{
				//Position															 //Color				//VertexID
				m_vertex[14] - 0.05f, m_vertex[15] - 0.3f + m_SliderModel[3].y,	 1.0f,1.0f,1.0f,1.0f,	IconsToRender.back() + 1.0f, //LD
				m_vertex[14]		 , m_vertex[15] - 0.3f + m_SliderModel[3].y,	 1.0f,1.0f,1.0f,1.0f,	IconsToRender.back() + 2.0f, //RD 
				m_vertex[14]		 , m_vertex[15] - 0.1f + m_SliderModel[3].y,	 1.0f,1.0f,1.0f,1.0f,	IconsToRender.back() + 3.0f, //RU 
				m_vertex[14] - 0.05f, m_vertex[15] - 0.1f + m_SliderModel[3].y,	 1.0f,1.0f,1.0f,1.0f,	IconsToRender.back() + 4.0f, //LU
			};

			float auxIndex[] =
			{
				aux[27], aux[6], aux[13],
				aux[27], aux[20], aux[13]

			};

			for (size_t i = 0; i < sizeof(aux) / sizeof(float); i++)
			{
				IconsToRender.emplace_back(aux[i]);
			}
			for (size_t i = 0; i < sizeof(auxIndex) / sizeof(float); i++)
			{
				IconsIndicesToRender.emplace_back(static_cast<unsigned int>(auxIndex[i]));
			}
		}


		m_vbI.Update(IconsToRender.data(), static_cast<unsigned int>(IconsToRender.size() * sizeof(float)));
		m_ibI.Update(IconsIndicesToRender.data(), static_cast<unsigned int>(IconsIndicesToRender.size() * sizeof(unsigned int)));


		/*
		bool first = true;

		for (size_t i = 0; i < IconsToRender.size(); i++)
		{
			if (first)
			{
				std::cout << "VERTEX ARRAY\n" << "{";
				first = false;
			}
			else
			{
				std::cout << ", ";
			}
			if (i % 7 == 0 && i != 0)
			{
				std::cout << std::endl;
			}
			std::cout << IconsToRender[i];
		}
		std::cout << "}" << std::endl;
		first = true;
		for (size_t i = 0; i < IconsIndicesToRender.size(); i++)
		{
			if (first)
			{
				std::cout << "INDEX ARRAY\n" << "{";
				first = false;
			}
			else
			{
				std::cout << ", ";
			}
			if (i % 3 == 0 && i != 0)
			{
				std::cout << std::endl;
			}
			std::cout << IconsIndicesToRender[i];
		}
		std::cout << "}" << std::endl; */


	}


	/**
		 * @brief Removes an icon from the window.
		 */
	void Window::RemoveIcon()
	{
		if (!m_icons.empty())
			m_icons.pop_back();
		RenderIcon();
		m_render = true;
	}


	/**
	 * @brief Draws an icon on the window.
	 */
	void Window::DrawIcon()
	{

		m_shaderI.Bind();
		m_shaderI.SetUniformMat4f("u_M", m_model);

		int viewportWidth = m_ContextWidth;
		int viewportHeight = m_ContextHeight;

		float xMaxNDC = m_WindowLimits[0];
		float xMinNDC = m_WindowLimits[1];
		float yMaxNDC = m_WindowLimits[2];
		float yMinNDC = m_WindowLimits[3];

		float xMaxScreen = ((xMaxNDC + 1.0f) / 2.0f) * viewportWidth;
		float xMinScreen = ((xMinNDC + 1.0f) / 2.0f) * viewportWidth;
		float yMaxScreen = ((yMaxNDC + 1.0f) / 2.0f) * viewportHeight;
		float yMinScreen = ((yMinNDC + 1.0f) / 2.0f) * viewportHeight;

		m_shaderI.SetUniform4f("u_WinLimit", xMaxScreen, xMinScreen, yMaxScreen, yMinScreen);

		m_vaI.Bind();
		m_vbI.Bind();
		m_ibI.Bind();

		unsigned int size = static_cast<unsigned int>(m_icons.size());

		unsigned int limit = size < m_MaxIconsToRender ? size : m_MaxIconsToRender;

		if (m_SliderEnable)
		{
			++limit;
		}

		GLCall(glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(limit * IndicesPerIcon), GL_UNSIGNED_INT, nullptr));

	}


	/**
		 * @brief Updates the limits of the window based on its current position and size.
		 */
	void Window::updateLimits()
	{
		m_WindowLimits[0] = m_model[3].x + m_vertex[7];
		m_WindowLimits[1] = m_model[3].x + m_vertex[0];
		m_WindowLimits[2] = m_model[3].y + m_vertex[15];
		m_WindowLimits[3] = m_model[3].y + m_vertex[1];


		//LIMITES DEL SLIDER
		m_SlideLimits[0] = m_vertex[14] + m_model[3].x;
		m_SlideLimits[1] = m_vertex[14] - 0.05f + m_model[3].x;
		m_SlideLimits[2] = m_vertex[15] - 0.1f + m_model[3].y;
		m_SlideLimits[3] = m_vertex[15] - 0.3f + m_model[3].y;
	}

	/**
   * @brief Checks and adjusts the window resizing based on mouse position.
   *
   * @param context Pointer to the GLFW window context.
   * @param normalizedMouseX Normalized X position of the mouse.
   * @param normalizedMouseY Normalized Y position of the mouse.
   */
	void Window::CheckResize(GLFWwindow* context, float normalizedMouseX, float  normalizedMouseY)
	{

		if (IsMouseInsideObject(normalizedMouseX, normalizedMouseY) && !m_dragging)
		{

			bool IsInRx = IsInBetween(epsilon, normalizedMouseX, m_WindowLimits[0]);
			bool IsInLx = IsInBetween(epsilon, normalizedMouseX, m_WindowLimits[1]);
			bool IsInUy = IsInBetween(epsilon, normalizedMouseY, m_WindowLimits[2]);
			bool IsInDy = IsInBetween(epsilon, normalizedMouseY, m_WindowLimits[3]);



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


	/**
		* @brief Normalizes mouse coordinates from screen space to OpenGL space.
		*
		* @param mouseX X position of the mouse in screen space.
		* @param mouseY Y position of the mouse in screen space.
		* @param normalizedX Reference to store the normalized X position.
		* @param normalizedY Reference to store the normalized Y position.
		*/
	void Window::NormalizeMouseCoords(double mouseX, double mouseY, float& normalizedX, float& normalizedY) const
	{
		normalizedX = (2.0f * (static_cast<float>(mouseX) / m_ContextWidth)) - 1.0f;
		normalizedY = 1.0f - (2.0f * (static_cast<float>(mouseY) / m_ContextHeight));
	}


	/**
		* @brief Checks if the mouse is inside the window's object.
		*
		* @param normalizedMouseX Normalized X position of the mouse.
		* @param normalizedMouseY Normalized Y position of the mouse.
		* @return True if the mouse is inside the object, otherwise false.
		*/
	bool Window::IsMouseInsideObject(float normalizedMouseX, float normalizedMouseY) const
	{
		return (normalizedMouseX >= m_WindowLimits[1] - epsilon &&
			normalizedMouseX <= m_WindowLimits[0] + epsilon &&
			normalizedMouseY >= m_WindowLimits[3] - epsilon &&
			normalizedMouseY <= m_WindowLimits[2] + epsilon);
	}

	bool Window::CheckSlide(float normalizedMouseX, float  normalizedMouseY)
	{
		if (m_SliderEnable && IsMouseInsideObject(normalizedMouseX, normalizedMouseY) && m_dragging)
		{

			bool IsBeforeRx = normalizedMouseX < m_SlideLimits[0] + m_SliderModel[3].x;
			bool IsAfterLx = normalizedMouseX > m_SlideLimits[1] + m_SliderModel[3].x;
			bool IsBelowUy = normalizedMouseY < m_SlideLimits[2] + m_SliderModel[3].y;
			bool IsAboveDy = normalizedMouseY > m_SlideLimits[3] + m_SliderModel[3].y;

			return (IsBeforeRx && IsAfterLx && IsAboveDy && IsBelowUy);

		}
		return false;
	}

} // namespace Vicetrice