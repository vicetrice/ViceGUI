#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <GL/glew.h>
#include "Error.hpp"


namespace Vicetrice
{
	VertexArray::VertexArray(unsigned int* currentID) : m_CurrentID{ currentID }
	{
		GLCall(glGenVertexArrays(1, &m_RendererID));
	}

	VertexArray::~VertexArray()
	{
		GLCall(glDeleteVertexArrays(1, &m_RendererID));

	}

	void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const
	{
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		// define vertex position layout
		// This links the attrib pointer wih the buffer at index 0 in the vertex array object
		for (unsigned int i = 0; i < elements.size(); ++i)
		{
			const auto& element = elements[i];
			GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
			GLCall(glEnableVertexAttribArray(i));

			offset += element.count * VertexBufferElement::GetSizeofType(element.type);
		}
	}

	void VertexArray::Bind() const
	{
		if (*m_CurrentID != m_RendererID)
		{
			GLCall(glBindVertexArray(m_RendererID));
			*m_CurrentID = m_RendererID;
		}
	}

	void VertexArray::Unbind() const
	{
		GLCall(glBindVertexArray(0));
		*m_CurrentID = 0;
	}
} //namespace Vicetrice