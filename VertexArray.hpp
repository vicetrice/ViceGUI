#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <GL/glew.h>
#include "Error.hpp"

namespace Vicetrice
{
	class VertexArray {
	public:
		VertexArray(unsigned int* currentID);
		~VertexArray();

		void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;

		void Bind() const;

		void Unbind() const;

	private:
		unsigned int m_RendererID;
		unsigned int* m_CurrentID;

	}; //class VertexArray
} //namespace Vicetrice