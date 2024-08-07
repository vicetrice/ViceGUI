#pragma once

#include <vector>
#include <GL/glew.h>
#include <cassert>



namespace Vicetrice
{
	struct VertexBufferElement
	{
		unsigned int type;
		unsigned int count;
		unsigned char normalized;


		static unsigned int GetSizeofType(unsigned int type)
		{
			switch (type)
			{
			case GL_FLOAT:return 4;
			case GL_UNSIGNED_INT:return 4;
			case GL_UNSIGNED_BYTE:return 1;

			}
			assert(false);
			return 0;
		};
	}; //struct VertexBufferElement

	class VertexBufferLayout
	{
	public:

		VertexBufferLayout() : m_Stride{ 0 } {}


		/**
		*	@brief Recieve the number of elements that fill the layout
		*	@param cont number of elements to be added in the VAO
		*/
		template <typename T>
		inline void Push(unsigned int cont)
		{
			static_assert(false);
		}


		/**
		*	@brief Recieve the number of elements that fill the layout
		*	@param cont number of elements to be added in the VAO
		*/
		template <>
		void Push<float>(unsigned int cont);


		/**
		*	@brief Recieve the number of elements that fill the layout
		*	@param cont number of elements to be added in the VAO
		*/
		template<>
		void Push<unsigned int>(unsigned int cont);


		/**
		*	@brief Recieve the number of elements that fill the layout
		*	@param cont number of elements to be added in the VAO
		*/
		template<>
		void Push<unsigned char>(unsigned int cont);


		inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
		inline unsigned int GetStride() const { return m_Stride; };

	private:
		std::vector <VertexBufferElement> m_Elements;
		unsigned int m_Stride;
	}; //class VertexBufferLayout
} //namespace Vicetrice


