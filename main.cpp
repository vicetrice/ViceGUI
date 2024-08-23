#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>


#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "Window.hpp"
#include "Icon.hpp"

using namespace Vicetrice;

// Variables globales
int InicontextWidth = 800;
int InicontextHeight = 600;
int Button;
int Action;
double Xpos;
double Ypos;

enum class Events
{
	ContextSize,
	MouseButton,
	CursorPosition,
};

std::vector<Events> events;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	events.push_back(Events::ContextSize);
	InicontextWidth = width;
	InicontextHeight = height;

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	events.push_back(Events::MouseButton);
	Button = button;
	Action = action;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	events.push_back(Events::CursorPosition);
	Xpos = xpos;
	Ypos = ypos;

}

int main()
{
	// Inicializar GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Crear una ventana y su contexto OpenGL
	GLFWwindow* window = glfwCreateWindow(InicontextWidth, InicontextHeight, "GUI", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Sincronizar con la tasa de refresco de la pantalla

	// Inicializar GLEW
	glewExperimental = true; // Necesario para el core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

	// Registrar la funci�n de callback para el redimensionamiento y el mouse
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	{
		Window Vwindow(InicontextWidth, InicontextHeight);
		/*Icon icon;
		icon.AddToContext(Vwindow.Vertices(), Vwindow.Indices(), true, 1);
		icon.AddToContext(Vwindow.Vertices(), Vwindow.Indices(), true, 2);
		icon.AddToContext(Vwindow.Vertices(), Vwindow.Indices(), true, 3);
		icon.AddToContext(Vwindow.Vertices(), Vwindow.Indices(), true, 4);
		icon.AddToContext(Vwindow.Vertices(), Vwindow.Indices(), true, 5);
		*/


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		do
		{
			glfwWaitEvents();
			if (!events.empty()) {
				Events evnt = events.back();
				events.pop_back();


				switch (evnt)
				{

				case Events::CursorPosition:
					Vwindow.Resize(window, Xpos, Ypos);
					Vwindow.Move(Xpos, Ypos);

					break;
				case Events::MouseButton:
					Vwindow.DragON(window, Button, Action);
						
					break;
				case Events::ContextSize:

					Vwindow.AdjustProj(InicontextWidth, InicontextHeight);

					glViewport(0, 0, InicontextWidth, InicontextHeight);

					break;

				default:
					break;
				}
			}

			if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS)
				Vwindow.RemoveIcon();
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				Vwindow.addIcon();
			
			

			// Configurar el shader y los buffers
			if (Vwindow.Rendering() || Vwindow.Dragging())
			{
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

				Vwindow.Draw();
				glfwSwapBuffers(window);
			}




		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
	}
	glfwTerminate();
	return 0;
}