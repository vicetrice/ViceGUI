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
int InicontextWidth = 1000;
int InicontextHeight = 500;
Window Vwindow(InicontextWidth, InicontextHeight);


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	// Ajusta la proyección ortográfica según el nuevo tamaño de la ventana
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
	GLFWwindow* window = glfwCreateWindow(InicontextWidth, InicontextHeight, "Tutorial 02 - Red triangle", NULL, NULL);
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

	// Registrar la función de callback para el redimensionamiento y el mouse
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Inicialización de OpenGL
	float positions[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f  // 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int CurrentVAID = 0;
	unsigned int CurrentVBID = 0;
	unsigned int CurrentIBID = 0;
	unsigned int CurrentShID = 0;
	{
		VertexArray va(&CurrentVAID);
		VertexBuffer vb(positions, sizeof(positions), &CurrentVBID);
		VertexBufferLayout layout;

		layout.Push<float>(2);
		va.addBuffer(vb, layout);

		Shader shader("res/shaders/Basic.shader", &CurrentShID);
		shader.Bind();

		IndexBuffer ib(indices, sizeof(indices), &CurrentIBID);



		float r = 1.0f;
		// Bucle principal
		do
		{
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			// Actualizar la matriz MVP
			shader.SetUniformMat4f("u_MVP", Vwindow.ModelMatrix());

			// Configurar el shader y los buffers

			if (Vwindow.Rendering() || Vwindow.Dragging())
			{
				Vwindow.Draw(shader, va, ib);
				glfwSwapBuffers(window);
			}
			glfwWaitEvents();


		} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
	}
	glfwTerminate();
	return 0;
}
