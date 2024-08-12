#include "IndexBuffer.hpp"
#include "Error.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Vicetrice
{

	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
		: m_Count{ count } {

		assert(sizeof(unsigned int) == sizeof(GLuint));

		GLCall(glGenBuffers(1, &m_RendererID));


		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));


		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));

	}


	IndexBuffer::~IndexBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RendererID));
	}

	void IndexBuffer::Bind() const
	{

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

	}

	void IndexBuffer::Unbind() const
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}