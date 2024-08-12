#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "Window.hpp"

using namespace Vicetrice;

// Variables globales
int InicontextWidth = 800;
int InicontextHeight = 600;
Window Vwindow(InicontextWidth, InicontextHeight);


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	// Ajusta la proyecci�n ortogr�fica seg�n el nuevo tama�o de la ventana
	Vwindow.AdjustProj(width, height);

	// Ajusta el viewport de OpenGL
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Vwindow.DragON(window, button, action);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Vwindow.Resize(window, xpos, ypos);
	Vwindow.Move(xpos, ypos);

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






	// Bucle principal
	do
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		// Actualizar la matriz MVP

		// Configurar el shader y los buffers

		if (Vwindow.Rendering() || Vwindow.Dragging())
		{
			Vwindow.Draw();
			glfwSwapBuffers(window);
		}
		glfwWaitEvents();


	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();
	return 0;
}