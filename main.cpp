#define STB_TRUETYPE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <chrono>

#include "Window.hpp"

using namespace Vicetrice;

// Variables globales
GLFWwindow* window;
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

void iniContext();

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

	iniContext();
	{
		Window Vwindow(InicontextWidth, InicontextHeight);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		do
		{

			glfwWaitEvents();
			auto start = std::chrono::steady_clock::now(); // Start timer
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


			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				Vwindow.RemoveIcon();
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{

				Vwindow.addIcon("Sapeee");
			}

			if (Vwindow.Rendering() || Vwindow.Dragging())
			{
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

				Vwindow.Draw();
				glfwSwapBuffers(window);

			}

			auto end = std::chrono::steady_clock::now(); // End timer
			std::chrono::duration<double, std::milli> elapsed = end - start;
			//std::cout << "El tiempo transcurrido es: " << elapsed.count() << " ms" << std::endl;
			//std::cout << "FPS: " << 1 / (elapsed.count() * 0.001) << std::endl;



		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
	}
	glfwTerminate();
	return 0;
}

void iniContext()
{
	// Inicializar GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Crear una ventana y su contexto OpenGL
	window = glfwCreateWindow(InicontextWidth, InicontextHeight, "GUI", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Sincronizar con la tasa de refresco de la pantalla

	// Inicializar GLEW
	glewExperimental = true; // Necesario para el core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return;
	}

	std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

	// Registrar la funci�n de callback para el redimensionamiento y el mouse
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
}