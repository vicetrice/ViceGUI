#pragma once

namespace Vicetrice
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const void* data, unsigned int count);

		~IndexBuffer();


		void Update(const void* data, unsigned int count, unsigned int offset = 0);
		void Bind() const;
		void Unbind() const;
		inline unsigned int GetCount() const { return m_Count; }


	private:
		unsigned int m_RendererID;
		unsigned int m_Count;

	}; //class IndexBuffer
} //namespace Vicetrice