#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include <GL/glew.h>
#include "Error.hpp"

namespace Vicetrice
{
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const;

		void Bind() const;

		void Unbind() const;

	private:
		unsigned int m_RendererID;

	}; //class VertexArray
} //namespace Vicetrice