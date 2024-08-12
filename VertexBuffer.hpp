#pragma once

namespace Vicetrice
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);

		~VertexBuffer();

		void Bind() const;
		void Unbind() const;


	private:
		unsigned int m_RendererID;
		
	}; //class VertexBuffer
} //namespace Vicetrice