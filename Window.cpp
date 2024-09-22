#include "Window.hpp"

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
	static float IconHeight = 0.05f;
	static float IconWidth = 1.0f;

	static const unsigned int VerticesPerQuad = 4;
	static const unsigned int IndicesPerQuad = 6;
	static const unsigned int IconVBOsize = 10000;
	static const unsigned int VBOsize = 8 + IconVBOsize;


	static  std::vector<VertexBufferElement> layout = {
		{ GL_FLOAT, 2, GL_FALSE, sizeof(glm::vec2) },			//POSITION
		{ GL_FLOAT, 4, GL_FALSE, sizeof(glm::vec4) },			//COLOR
		{ GL_FLOAT, 2, GL_FALSE, sizeof(glm::vec2) },			//TEXCOORD										
		{ GL_INT, 1, GL_FALSE, sizeof(int) }					//TEXID										

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
		m_proj{ 1.0f },
		m_dragging{ false },
		m_render{ true },
		m_BaseContextWidth{ ContextWidth },
		m_BaseContextHeight{ ContextHeight },
		m_ContextWidth{ ContextWidth },
		m_ContextHeight{ ContextHeight },
		m_lastMouseX{ 0.0f },
		m_lastMouseY{ 0.0f },
		m_resize{ ResizeTypes::NORESIZE },
		m_moving{ false },
		m_WindowRenderer{ VBOsize,VBOsize * IndicesPerQuad , layout , RendererType::ATTS_IN_MIXED_BUFFERS },
		m_WindowShader{ "res/shaders/Window.shader" },
		m_IndexToFirstIconToRender{ 0 },
		m_TotalVerticesToRender{ 0 },
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
		for (auto& icon : m_icons)
		{
			delete icon;
		}
		m_icons.clear();
		m_icons.shrink_to_fit();
	}

	/**
	* @brief Draws the window and its icons on the screen.
	*/
	void Window::Draw()
	{

		unsigned int indices = (m_TotalVerticesToRender / VerticesPerQuad) * IndicesPerQuad;

		//std::cout << "TV: " << m_TotalVerticesToRender << std::endl << "Indices: " << indices << std::endl;
		//std::cout << m_WindowRenderer.VBOByteSize() << std::endl;
		m_WindowRenderer.DrawEBO(0, indices, m_WindowShader);

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

		float aspectRatio = static_cast<float>(ContextWidth) / static_cast<float>(ContextHeight);



		float left = -aspectRatio;
		float right = aspectRatio;

		m_proj = glm::ortho(left, right, -1.0f, 1.0f, -1.0f, 1.0f);
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
			NormalizeCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);
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
			NormalizeCoords(mouseX, mouseY, normalizedMouseX, normalizedMouseY);
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
			else if (action == GLFW_RELEASE)
			{
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
			NormalizeCoords(xpos, ypos, normalizedMouseX, normalizedMouseY);
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
	void Window::addIcon(const std::string& NameToDisplay)
	{

		std::random_device rd;  // Fuente de entropía
		std::mt19937 gen(rd()); // Generador de números aleatorios Mersenne Twister

		// Distribución uniforme entre 0 y 1
		std::uniform_real_distribution<> dis(0.0, 1.0);

		// Generar un número aleatorio
		float randomValue = static_cast<float>(dis(gen));

		unsigned int IconNumber = m_icons.size();

		std::vector<Vertex> Vertices;

		/*= {
		   {glm::vec2(0.0f,IconHeight)			,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,-1},	//LD
		   {glm::vec2(IconWidth,IconHeight)	,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,-1},	//RD
		   {glm::vec2(0.0f,0.0f)				,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,-1},	//LU
		   {glm::vec2(IconWidth,0.0f)			,	glm::vec4(randomValue * m_icons.size(),randomValue,randomValue,1.0f),		glm::vec2(0.0f,0.0f) ,-1}	//RU
	};*/



		GenerateTextVertices(Vertices, NameToDisplay, 0.0f, 0.0f, 0.5f);

		m_TotalVerticesToRender += Vertices.size();

		m_WindowShader.Bind();
		m_WindowShader.SetUniform1i("u_Textures", 0);

		m_icons.emplace_back(new StaticTextIcon<Vertex>(Vertices));
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
			unsigned int IconVertices = m_icons.back()->GetVertices().size();
			m_TotalVerticesToRender -= IconVertices;

			m_WindowRenderer.UpdateVBOByteSize(0, m_WindowRenderer.VBOByteSize(0) - (sizeof(Vertex) * IconVertices));
			m_WindowRenderer.UpdateVBOByteSize(1, m_WindowRenderer.VBOByteSize(1) - (sizeof(glm::vec2) * IconVertices));

			delete m_icons.back();
			m_icons.pop_back();
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
		glm::vec4 aux = (m_proj * m_model) * glm::vec4(Wright, Wup, 0.0f, 1.0f);
		glm::vec4 aux2 = (m_proj * m_model) * glm::vec4(Wleft, Wdown, 0.0f, 1.0f);


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
			{glm::vec2(Wleft,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f), 	glm::vec2(0.0f,0.0f),	-1},	//LD
			{glm::vec2(Wright,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	//RD
			{glm::vec2(Wleft,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	//LU
			{glm::vec2(Wright,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1}		//RU

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

		m_TotalVerticesToRender += Vertices.size();

	}

	void Window::updateHardWposition()
	{
		std::vector<Vertex> Vertices = {
			//Window
			{glm::vec2(Wleft,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f), 	glm::vec2(0.0f,0.0f),	-1},	//LD
			{glm::vec2(Wright,	Wdown)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	//RD
			{glm::vec2(Wleft,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	//LU
			{glm::vec2(Wright,	Wup)	,		glm::vec4(0.8f,0.2f,0.9f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	//RU
		};
		m_WindowRenderer.UpdateVBO(Vertices.data(), sizeof(Vertex) * Vertices.size());
	}

	void Window::ConfigWindowShader()
	{
		m_WindowShader.Bind();
		m_WindowShader.SetUniformMat4f("u_MP", m_proj * m_model);

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
	void Window::NormalizeCoords(double X, double Y, float& normalizedX, float& normalizedY) const
	{
		normalizedX = (2.0f * (static_cast<float>(X) / m_ContextWidth)) - 1.0f;
		normalizedY = 1.0f - (2.0f * (static_cast<float>(Y) / m_ContextHeight));
	}

	/**
	* @brief Denormalizes coordinates from OpenGL space back to screen space.
	*
	* @param normalizedX Normalized X coordinate (from -1.0 to 1.0 in OpenGL space).
	* @param normalizedY Normalized Y coordinate (from -1.0 to 1.0 in OpenGL space).
	* @param X Reference to store the screen space X coordinate.
	* @param Y Reference to store the screen space Y coordinate.
	*/
	void Window::DeNormalizeCoords(float normalizedX, float normalizedY, double& X, double& Y) const
	{
		// Revertir normalización para X
		X = ((normalizedX + 1.0f) * 0.5f) * m_ContextWidth;

		// Revertir normalización para Y (ajustando por la inversión del eje Y)
		Y = (1.0f - ((normalizedY + 1.0f) * 0.5f)) * m_ContextHeight;
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
		//auto start = std::chrono::steady_clock::now(); // Start timer
		if (!m_icons.empty())
		{
			std::vector<glm::vec2> Shift;
			std::vector<Vertex> Vertices;
			unsigned int NumberOfIcons = m_icons.size();
			m_TotalVerticesToRender = 4;
			if (m_IndexToFirstIconToRender != 0 && (m_MaxIconsToRender + m_IndexToFirstIconToRender - 2) > NumberOfIcons)
			{
				--m_IndexToFirstIconToRender;
			}

			Vertices.reserve(VBOsize);
			Shift.reserve(VBOsize);

			unsigned int limit = NumberOfIcons > (m_MaxIconsToRender + m_IndexToFirstIconToRender) ? m_MaxIconsToRender + m_IndexToFirstIconToRender : NumberOfIcons;

			for (unsigned int i = m_IndexToFirstIconToRender; i < limit; i++)
			{
				const std::vector<Vertex>& iconVertices = m_icons[i]->GetVertices();
				size_t VectorSize = iconVertices.size() ;

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
					{glm::vec2(Wright - 0.05f	, Wdown),				glm::vec4(0.8f,0.2f,0.4f,1.0f), 	glm::vec2(0.0f,0.0f),	-1},	 //LD
					{glm::vec2(Wright			, Wdown),				glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //RD 
					{glm::vec2(Wright - 0.05f	, Wup - 0.1f),	 		glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //LU
					{glm::vec2(Wright 			, Wup - 0.1f),	 		glm::vec4(0.8f,0.2f,0.4f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //RU 

					//Slider
					{glm::vec2(Wright - 0.04f	, Wup - VariableSize + m_SliderModel[3].y),	glm::vec4(1.0f,1.0f,1.0f,1.0f), 	glm::vec2(0.0f,0.0f),	-1},	 //LD
					{glm::vec2(Wright - 0.01f	, Wup - VariableSize + m_SliderModel[3].y),	glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //RD 
					{glm::vec2(Wright - 0.04f	, Wup - 0.1f + m_SliderModel[3].y),	 		glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //LU
					{glm::vec2(Wright - 0.01f	, Wup - 0.1f + m_SliderModel[3].y),	 		glm::vec4(1.0f,1.0f,1.0f,1.0f),		glm::vec2(0.0f,0.0f),	-1},	 //RU 

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

			m_TotalVerticesToRender += Vertices.size();
			m_WindowRenderer.UpdateVBO(Vertices.data(), sizeof(Vertex) * Vertices.size(), sizeof(Vertex) * VerticesPerQuad);
			m_WindowRenderer.UpdateVBO(1, Shift.data(), sizeof(glm::vec2) * Shift.size(), sizeof(glm::vec2) * VerticesPerQuad);
		}

		//auto end = std::chrono::steady_clock::now(); // End timer
		//std::chrono::duration<double, std::milli> elapsed = end - start;
		//std::cout << "El tiempo transcurrido es: " << elapsed.count() << " ms" << std::endl;

	}

	unsigned int Window::CalculateMaxIconsToRender()
	{
		m_MaxIconsToRender = static_cast<unsigned int>(((m_WindowLimits.up - m_WindowLimits.down) / IconHeight));

		std::cout << "MITR: " << m_MaxIconsToRender << std::endl;

		if (m_MaxIconsToRender == 0)
			m_MaxIconsToRender = 1;


		//CAN BE ELIMINATED IF RESIZING LIMITS GOT DEFINED
		if (m_MaxIconsToRender > 30)
			m_MaxIconsToRender = 30;

		return m_MaxIconsToRender;
	}

	float Window::UpdateSliderLimits()
	{
		//IMPORTANT: DANGEROUS CALCULATIONS AHEAD CHANGE IN CASE OF BUGS
		float WindowH = static_cast<float>(m_MaxIconsToRender) > 5 ? static_cast<float>(m_MaxIconsToRender) - 5 : 0;
		float size = m_icons.empty() ? 1.0f : static_cast<float>(m_icons.size());
		float Aux = 0.13f + (WindowH / size);
		float VariableSize = (Aux > 1.0f || Aux < -1.0f) ? 0.13f : Aux;
		//END OF IMPORTANT

		m_SlideLimits.right = m_WindowLimits.right;
		m_SlideLimits.left = m_WindowLimits.right - 0.05f;
		m_SlideLimits.up = m_WindowLimits.up - 0.1f;
		m_SlideLimits.down = m_WindowLimits.up - VariableSize;

		return VariableSize;
	}


	void Window::GenerateTextVertices(std::vector<Vertex>& VertexStorage, const std::string& text, float normalizedX, float normalizedY, float scale)
	{

		assert((normalizedX >= -1.0f && normalizedX <= 1.0f) && (normalizedY >= -1.0f && normalizedY <= 1.0f));


		VertexStorage.reserve(text.size() * VerticesPerQuad);

		float X;
		float Y;


		X = ((normalizedX + 1.0f) * 0.5f) * m_BaseContextWidth;


		Y = (1.0f - ((normalizedY + 1.0f) * 0.5f)) * m_BaseContextHeight;



		for (char c : text) {
			if (c < 32 || c >= 128) continue;

			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(m_WindowRenderer.FontInfo(), 512, 512, c - 32, &X, &Y, &q, 1);

			float x0 = q.x0;
			float y0 = q.y0;
			float x1 = q.x1;
			float y1 = q.y1;

			float normX0, normY0, normX1, normY1;

			normX0 = (2.0f * (static_cast<float>(x0) / m_BaseContextWidth)) - 1.0f;
			normY0 = 1.0f - (2.0f * (static_cast<float>(y0) / m_BaseContextHeight));

			normX1 = (2.0f * (static_cast<float>(x1) / m_BaseContextWidth)) - 1.0f;
			normY1 = 1.0f - (2.0f * (static_cast<float>(y1) / m_BaseContextHeight));

			normX0 *= scale;
			normY0 *= scale;

			normX1 *= scale;
			normY1 *= scale;

			VertexStorage.insert(VertexStorage.end(), {

				{glm::vec2(normX0, normY0), glm::vec4(1.0f,1.0f,1.0f,1.0f) ,glm::vec2(q.s0, q.t0), 0}, //LD
				{glm::vec2(normX1, normY0), glm::vec4(1.0f,1.0f,1.0f,1.0f) ,glm::vec2(q.s1, q.t0), 0}, //RD
				{glm::vec2(normX0, normY1), glm::vec4(1.0f,1.0f,1.0f,1.0f) ,glm::vec2(q.s0, q.t1), 0}, //LU
				{glm::vec2(normX1, normY1), glm::vec4(1.0f,1.0f,1.0f,1.0f) ,glm::vec2(q.s1, q.t1), 0}, //RU

				});
		}


	}



} //namespace Vicetrice