#pragma once

namespace Vicetrice
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size, unsigned int* CurrentID);

		~VertexBuffer();

		void Bind() const;
		void Unbind() const;


	private:
		unsigned int m_RendererID;
		unsigned int* m_CurrentID;
	}; //class VertexBuffer
} //namespace Vicetrice