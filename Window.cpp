#include "Window.hpp"
#include "Renderer.hpp"

namespace Vicetrice
{

	//VARIABLES
	static const float epsilon = 0.01f;

	//Initial Pos of Window
	static float Wright = 0.5f;
	static float Wleft = -0.5f;
	static float Wup = 0.5f;
	static float Wdown = -0.5f;

	//Icon
	static float IconHeight = 0.08f;
	static float IconWidth = 1.0f;

	static const unsigned int VerticesPerQuad = 4;
	static const unsigned int IndicesPerQuad = 6;
	static const unsigned int IconVBOsize = 10000;
	static const unsigned int VBOsize = 8 + IconVBOsize;
	static  std::vector<VertexBufferElement> layout = {
		{ GL_FLOAT, 2, GL_FALSE, sizeof(glm::vec2) },			//POSITION
		{ GL_FLOAT, 4, GL_FALSE, sizeof(glm::vec4) },			//COLOR
		{ GL_FLOAT, 2, GL_FALSE, sizeof(glm::vec2) },			//TEXCOORD										
		{ GL_UNSIGNED_INT, 1, GL_FALSE, sizeof(unsigned int) }  //TEXID										

	};


	//---------------------------------------- PUBLIC

	/**
		 * @brief Constructor for the Window class.
		 *
		 * @param ContextWidth Width of the context (window).
		 * @param ContextHeight Height of the context (window).
		 * @param shPath Path to the shader files.
		 */
	Window::Window(int ContextWidth, int ContextHeight)
		:
		m_model{ 1.0f },
		m_dragging{ false },
		m_render{ true },
		m_ContextWidth{ ContextWidth },
		m_ContextHeight{ ContextHeight },
		m_lastMouseX{ 0.0f },
		m_lastMouseY{ 0.0f },
		m_resize{ ResizeTypes::NORESIZE },
		m_moving{ false },
		m_WindowRenderer{ VBOsize,VBOsize * IndicesPerQuad , layout , RendererType::ATTS_IN_MIXED_BUFFERS },
		m_WindowShader{ "res/shaders/Window.shader" },
		m_IndexToFirstIconToRender{ 0 },
		m_MaxIconsToRender{ 0 },
		m_SliderEnable{ false },
		m_SliderModel{ 1.0f },
		m_sliding{ false }
	{
		updateWindowLimits();
		CreateWindowFrame();
		CalculateMaxIconsToRender();

		unsigned int MaxQuads = VBOsize / VerticesPerQuad;


		std::vector<unsigned int> indices;
		indices.reserve(MaxQuads * IndicesPerQuad);
		unsigned int startVertexIndex = 0;
		for (unsigned int i = 0; i < MaxQuads; i++)
		{
			indices.emplace_back(startVertexIndex);
			indices.emplace_back(startVertexIndex + 1);
			indices.emplace_back(startVertexIndex + 2);
			indices.emplace_back(startVertexIndex + 1);
			indices.emplace_back(startVertexIndex + 2);
			indices.emplace_back(startVertexIndex + 3);

			startVertexIndex += 4;
		}

		m_WindowRenderer.UpdateEBO(indices.data(), sizeof(unsigned int) * indices.size());
	}

	/**
	* @brief Destructor for the Window class.
	*/
	Window::~Window()
	{

	}

	/**
	* @brief Draws the window and its icons on the screen.
	*/
	void Window::Draw()
	{
		unsigned int size = static_cast<unsigned int>(m_icons.size());

		unsigned int limit = (size - m_IndexToFirstIconToRender) > m_MaxIconsToRender ? m_MaxIconsToRender : size - m_IndexToFirstIconToRender;
		if (m_SliderEnable)
			limit += 2;

		//Add the Window
		++limit;

		m_WindowRenderer.DrawEBO(0, limit * IndicesPerQuad, m_WindowShader);

		m_render = false;
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
		updateWindowLimits();
		m_render = true;
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
				updateWindowLimits();

			}
			if (m_SliderEnable && m_sliding)
			{
				m_SliderModel = glm::translate(m_SliderModel, glm::vec3(0.0f, deltaYNorm, 0.0f));


				if (m_SliderModel[3].y > 0.0f)
				{
					m_SliderModel[3].x = 0.0f;
					m_SliderModel[3].y = 0.0f;
					m_SliderModel[3].z = 0.0f;


				}
				else if (m_SlideLimits.down + m_SliderModel[3].y < m_WindowLimits.down)
				{
					m_SliderModel[3].x = 0.0f;
					m_SliderModel[3].y = m_WindowLimits.down - m_SlideLimits.down;
					m_SliderModel[3].z = 0.0f;

				}


				//Calculate the displacement based on the number of icons
				float displacementPerIcon = (m_icons.size() - (m_MaxIconsToRender - 2)) != 0 ? (m_WindowLimits.down - m_SlideLimits.down) / (m_icons.size() - (m_MaxIconsToRender - 2)) : (m_WindowLimits.down - m_SlideLimits.down);
				//Calculate the index of the first icon to render based on the displacement
				unsigned int index = static_cast<unsigned int>(m_SliderModel[3].y / displacementPerIcon);
				m_IndexToFirstIconToRender = index;

				updateSlider();
			}



			m_lastMouseX = normalizedMouseX;
			m_lastMouseY = normalizedMouseY;
		}
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
				if (IsMouseInsideWindow(normalizedMouseX, normalizedMouseY))
				{
					m_dragging = true;
					m_lastMouseX = normalizedMouseX;
					m_lastMouseY = normalizedMouseY;
					CheckResize(context, normalizedMouseX, normalizedMouseY);
					if (CheckSlide(normalizedMouseX, normalizedMouseY))
						m_sliding = true;
					else if (m_resize == ResizeTypes::NORESIZE /* && IsMouseInsideMovingPart(normalizedMouseX, normalizedMouseY)*/)
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

			float deltaXNorm;
			float deltaYNorm;

			switch (m_resize)
			{
			case Vicetrice::ResizeTypes::RXRESIZE:
				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.right);
				Wright += deltaXNorm;
				break;

			case Vicetrice::ResizeTypes::LXRESIZE:
				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.left);
				Wleft += deltaXNorm;
				break;

			case Vicetrice::ResizeTypes::UYRESIZE:
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.up);
				Wup += deltaYNorm;
				break;

			case Vicetrice::ResizeTypes::DYRESIZE:
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.down);
				Wdown += deltaYNorm;
				break;

			case Vicetrice::ResizeTypes::RXDYRESIZE:

				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.right);
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.down);

				Wright += deltaXNorm;
				Wdown += deltaYNorm;
				break;

			case Vicetrice::ResizeTypes::RXUYRESIZE:

				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.right);
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.up);

				Wright += deltaXNorm;
				Wup += deltaYNorm;
				break;
			case Vicetrice::ResizeTypes::LXDYRESIZE:

				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.left);
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.down);

				Wleft += deltaXNorm;
				Wdown += deltaYNorm;
				break;

			case Vicetrice::ResizeTypes::LXUYRESIZE:

				deltaXNorm = static_cast<float>(normalizedMouseX - m_WindowLimits.left);
				deltaYNorm = static_cast<float>(normalizedMouseY - m_WindowLimits.up);

				Wleft += deltaXNorm;
				Wup += deltaYNorm;
				break;

			default:
				break;
			}


			CalculateMaxIconsToRender();

			updateWindowLimits();
			updateSlider();

		}
	}



	/**
	* @brief Adds an icon to the window.
	*/
	void Window::addIcon()
	{

		std::random_device rd;  // Fuente de entropía
		std::mt19937 gen(rd()); // Generador de números aleatorios Mersenne Twister

		// Distribución uniforme entre 0 y 1
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Generar un número aleatorio
		float randomValue = static_cast<float>(dis(gen));

		unsigned int IconNumber = m_icons.size();

		std::vector<Vertex> Vertices = {
			{glm::vec2(0.0f,IconHeight)			,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,0},	//LD
			{glm::vec2(IconWidth,IconHeight)	,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,0},	//RD
			{glm::vec2(0.0f,0.0f)				,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,0},	//LU
			{glm::vec2(IconWidth,0.0f)			,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,0}	//RU
		};

		std::vector<glm::vec2> Shift =
		{
			glm::vec2(0.0f,0.0f),
			glm::vec2(0.0f,0.0f),
			glm::vec2(0.0f,0.0f),
			glm::vec2(0.0f,0.0f)
		};

		m_icons.emplace_back(Icon<Vertex>(Vertices));
		updateSlider();

		m_render = true;
	}

	/**
	* @brief Removes an icon from the window.
	*/
	void Window::RemoveIcon()
	{
		if (!m_icons.empty())
		{
			m_icons.pop_back();
			m_WindowRenderer.UpdateVBOByteSize(0, m_WindowRenderer.VBOByteSize(0) - (sizeof(Vertex) * VerticesPerQuad));
			m_WindowRenderer.UpdateVBOByteSize(1, m_WindowRenderer.VBOByteSize(1) - (sizeof(glm::vec2) * VerticesPerQuad));
			m_WindowRenderer.UpdateEBOByteSize(0, m_WindowRenderer.EBOByteSize(0) - (sizeof(unsigned int) * IndicesPerQuad));
		}


		updateSlider();
		m_render = true;
	}



	//---------------------------------------- PRIVATE

	/**
	* @brief Updates the limits of the window based on its current position and size.
	*/
	void Window::updateWindowLimits()
	{
		if (m_resize != ResizeTypes::NORESIZE)
		{
			updateHardWposition();
		}
		glm::vec4 aux = m_model * glm::vec4(Wright, Wup, 0.0f, 1.0f);
		glm::vec4 aux2 = m_model * glm::vec4(Wleft, Wdown, 0.0f, 1.0f);


		m_WindowLimits.right = aux[0];
		m_WindowLimits.left = aux2[0];
		m_WindowLimits.up = aux[1];
		m_WindowLimits.down = aux2[1];

		UpdateSliderLimits();
		ConfigWindowShader();
	}

	void Window::CreateWindowFrame()
	{
		std::vector<Vertex> Vertices = {
			//Window
			{glm::vec2(Wleft,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f), 	glm::vec2(0.0f,0.0f),	0},	//LD
			{glm::vec2(Wright,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0},	//RD
			{glm::vec2(Wleft,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0},	//LU
			{glm::vec2(Wright,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0 } //RU

		};


		m_WindowRenderer.addAttrib({ GL_FLOAT, 2, GL_FALSE, sizeof(glm::vec2) });

		std::vector<glm::vec2> Shift = {
				glm::vec2(0.0f,0.0f),
				glm::vec2(0.0f,0.0f),
				glm::vec2(0.0f,0.0f),
				glm::vec2(0.0f,0.0f)
		};

		m_WindowRenderer.UpdateVBO(Vertices.data(), sizeof(Vertex) * Vertices.size());
		m_WindowRenderer.UpdateVBO(1, Shift.data(), sizeof(glm::vec2) * Shift.size());

	}

	void Window::updateHardWposition()
	{
		std::vector<Vertex> Vertices = {
			//Window
			{glm::vec2(Wleft,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f), 	glm::vec2(0.0f,0.0f),	0},	//LD
			{glm::vec2(Wright,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0},	//RD
			{glm::vec2(Wleft,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0},	//LU
			{glm::vec2(Wright,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	0},	//RU
		};
		m_WindowRenderer.UpdateVBO(Vertices.data(), sizeof(Vertex) * Vertices.size());
	}

	void Window::ConfigWindowShader()
	{
		m_WindowShader.Bind();
		m_WindowShader.SetUniformMat4f("u_M", m_model);

		int viewportWidth = m_ContextWidth;
		int viewportHeight = m_ContextHeight;

		float xMaxNDC = m_WindowLimits.right;
		float xMinNDC = m_WindowLimits.left;
		float yMaxNDC = m_WindowLimits.up;
		float yMinNDC = m_WindowLimits.down;

		float xMaxScreen = ((xMaxNDC + 1.0f) / 2.0f) * viewportWidth;
		float xMinScreen = ((xMinNDC + 1.0f) / 2.0f) * viewportWidth;
		float yMaxScreen = ((yMaxNDC + 1.0f) / 2.0f) * viewportHeight;
		float yMinScreen = ((yMinNDC + 1.0f) / 2.0f) * viewportHeight;

		m_WindowShader.SetUniform4f("u_WinLimit", xMaxScreen, xMinScreen, yMaxScreen, yMinScreen);

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
	bool Window::IsMouseInsideWindow(float normalizedMouseX, float normalizedMouseY) const
	{
		return (normalizedMouseX >= m_WindowLimits.left - epsilon &&
			normalizedMouseX <= m_WindowLimits.right + epsilon &&
			normalizedMouseY >= m_WindowLimits.down - epsilon &&
			normalizedMouseY <= m_WindowLimits.up + epsilon);
	}

	bool Window::IsMouseInsideMovingPart(float normalizedMouseX, float normalizedMouseY) const
	{
		return (normalizedMouseX >= m_WindowLimits.left - epsilon &&
			normalizedMouseX <= m_WindowLimits.right + epsilon &&
			normalizedMouseY >= m_WindowLimits.up - 0.1f - epsilon &&
			normalizedMouseY <= m_WindowLimits.up + epsilon);
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

		if (IsMouseInsideWindow(normalizedMouseX, normalizedMouseY) && !m_dragging)
		{

			bool IsInRx = IsInBetween(epsilon, normalizedMouseX, m_WindowLimits.right);
			bool IsInLx = IsInBetween(epsilon, normalizedMouseX, m_WindowLimits.left);
			bool IsInUy = IsInBetween(epsilon, normalizedMouseY, m_WindowLimits.up);
			bool IsInDy = IsInBetween(epsilon, normalizedMouseY, m_WindowLimits.down);



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

	bool Window::CheckSlide(float normalizedMouseX, float  normalizedMouseY)
	{
		if (m_SliderEnable && IsMouseInsideWindow(normalizedMouseX, normalizedMouseY) && m_dragging)
		{
			bool IsBeforeRx = normalizedMouseX < m_SlideLimits.right + m_SliderModel[3].x;
			bool IsAfterLx = normalizedMouseX > m_SlideLimits.left + m_SliderModel[3].x;
			bool IsBelowUy = normalizedMouseY < m_SlideLimits.up + m_SliderModel[3].y;
			bool IsAboveDy = normalizedMouseY > m_SlideLimits.down + m_SliderModel[3].y;

			return (IsBeforeRx && IsAfterLx && IsAboveDy && IsBelowUy);

		}
		return false;
	}

	void Window::updateSlider()
	{

		if (!m_icons.empty())
		{
			std::vector<glm::vec2> Shift;
			std::vector<Vertex> Vertices;
			unsigned int NumberOfIcons = m_icons.size();

			if (m_IndexToFirstIconToRender != 0 && (m_MaxIconsToRender + m_IndexToFirstIconToRender - 2) > NumberOfIcons)
			{
				--m_IndexToFirstIconToRender;
			}

			Vertices.reserve(VBOsize);
			Shift.reserve(VBOsize);

			unsigned int limit = NumberOfIcons > (m_MaxIconsToRender + m_IndexToFirstIconToRender) ? m_MaxIconsToRender + m_IndexToFirstIconToRender : NumberOfIcons;

			for (unsigned int i = m_IndexToFirstIconToRender; i < limit; i++)
			{
				const std::vector<Vertex>& iconVertices = m_icons[i].GetVertices();
				size_t VectorSize = iconVertices.size();


				Vertices.insert(Vertices.end(), iconVertices.begin(), iconVertices.end());


				glm::vec2 shiftValue = glm::vec2(Wleft, Wup - ((IconHeight * (i - m_IndexToFirstIconToRender + 1)) + 0.1f));


				Shift.insert(Shift.end(), VectorSize, shiftValue);
			}

			if (NumberOfIcons > m_MaxIconsToRender - 2)
			{
				m_SliderEnable = true;
			}
			else
			{
				//RESET POSITION
				m_SliderModel[3].x = 0.0f;
				m_SliderModel[3].y = 0.0f;
				m_SliderModel[3].z = 0.0f;

				m_IndexToFirstIconToRender = 0;

				m_SliderEnable = false;
			}

			if (m_SliderEnable)
			{

				if (m_resize != ResizeTypes::NORESIZE)
				{
					float displacementPerIcon = (NumberOfIcons - (m_MaxIconsToRender - 2)) != 0 ? (m_WindowLimits.down - m_SlideLimits.down) / (NumberOfIcons - (m_MaxIconsToRender - 2)) : (m_WindowLimits.down - m_SlideLimits.down);
					//Calculate Slider position based on index
					float displacement = displacementPerIcon * m_IndexToFirstIconToRender;
					m_SliderModel[3].y = displacement;
				}

				float VariableSize = UpdateSliderLimits();

				Vertices.insert(Vertices.end(), {


					//Slider Frame																 				
					{glm::vec2(Wright - 0.05f	, Wdown),				glm::vec4(0.8f,0.2f,0.4f,1.0f), 	glm::vec2(0.0f,0.0f),	0},	 //LD
					{glm::vec2(Wright			, Wdown),				glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	0},	 //RD 
					{glm::vec2(Wright - 0.05f	, Wup - 0.1f),	 		glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	0},	 //LU
					{glm::vec2(Wright 			, Wup - 0.1f),	 		glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	0},  //RU 

					//Slider
					{glm::vec2(Wright - 0.04f	, Wup - VariableSize + m_SliderModel[3].y),	glm::vec4(1.0f,1.0f,1.0f,1.0f), 	glm::vec2(0.0f,0.0f),	0},	 //LD
					{glm::vec2(Wright - 0.01f	, Wup - VariableSize + m_SliderModel[3].y),	glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	0},	 //RD 
					{glm::vec2(Wright - 0.04f	, Wup - 0.1f + m_SliderModel[3].y),	 		glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	0},	 //LU
					{glm::vec2(Wright - 0.01f	, Wup - 0.1f + m_SliderModel[3].y),	 		glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	0},  //RU 

					});


				Shift.insert(Shift.end(),
					{
						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f),

						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f),
						glm::vec2(0.0f,0.0f)
					});
			}


			m_WindowRenderer.UpdateVBO(Vertices.data(), sizeof(Vertex) * Vertices.size(), sizeof(Vertex) * VerticesPerQuad);
			m_WindowRenderer.UpdateVBO(1, Shift.data(), sizeof(glm::vec2) * Shift.size(), sizeof(glm::vec2) * VerticesPerQuad);
		}
	}

	unsigned int Window::CalculateMaxIconsToRender()
	{
		m_MaxIconsToRender = static_cast<unsigned int>(((m_WindowLimits.up - m_WindowLimits.down) / IconHeight) + 1.0f);

		if (m_MaxIconsToRender == 0)
			m_MaxIconsToRender = 1;


		//CAN BE ELIMINATED IF RESIZING LIMITS GOT DEFINED
		if (m_MaxIconsToRender > 30)
			m_MaxIconsToRender = 30;

		return m_MaxIconsToRender;
	}

	float Window::UpdateSliderLimits()
	{
		//TODO: Crear Vertices del slider a partir de m_SliderLimits
		//IMPORTANT: DANGEROUS CALCULATIONS AHEAD CHANGE IN CASE OF BUGS
		float WindowH = static_cast<float>(m_MaxIconsToRender) > 4.0f ? static_cast<float>(m_MaxIconsToRender) - 4.0f : 0.0f;
		float size = m_icons.empty() ? 1.0f : static_cast<float>(m_icons.size());
		float Aux = 0.13f + (WindowH / size);
		float VariableSize = (Aux > 1.0f || Aux < -1.0f) ? 0.13f : Aux;
		//END OF IMPORTANT

		m_SlideLimits.right = Wright + m_model[3].x;
		m_SlideLimits.left = Wright - 0.05f + m_model[3].x;
		m_SlideLimits.up = Wup - 0.1f + m_model[3].y;
		m_SlideLimits.down = Wup - VariableSize + m_model[3].y;

		return VariableSize;
	}


} //namespace Vicetrice