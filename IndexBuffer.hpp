#pragma once

namespace Vicetrice
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* data, unsigned int count, unsigned int* CurrentID);

		~IndexBuffer();

		void Bind() const;
		void Unbind() const;
		inline unsigned int GetCount() const { return m_Count; }


	private:
		unsigned int m_RendererID;
		unsigned int m_Count;
		unsigned int* m_CurrentID;

	}; //class IndexBuffer
} //namespace Vicetrice