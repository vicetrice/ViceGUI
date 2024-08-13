#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Error.hpp"
namespace Vicetrice
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);

		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
		// Método para actualizar el contenido del buffer
		void Update(const void* data, unsigned int size, unsigned int offset = 0) const;


	private:
		unsigned int m_RendererID;

	}; //class VertexBuffer
} //namespace Vicetrice