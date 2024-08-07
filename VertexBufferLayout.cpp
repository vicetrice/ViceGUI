#include "VertexBufferLayout.hpp"



namespace Vicetrice
{


	/**
	*	@brief Recieve the number of elements that fill the layout
	*	@param cont number of elements to be added in the VAO
	*/
	template <>
	void VertexBufferLayout::Push<float>(unsigned int cont)
	{
		m_Elements.push_back({ GL_FLOAT,cont,GL_FALSE });
		m_Stride += cont * VertexBufferElement::GetSizeofType(GL_FLOAT);
	}


	/**
	*	@brief Recieve the number of elements that fill the layout
	*	@param cont number of elements to be added in the VAO
	*/
	template<>
	void VertexBufferLayout::Push<unsigned int>(unsigned int cont)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT,cont,GL_FALSE });
		m_Stride += cont * VertexBufferElement::GetSizeofType(GL_UNSIGNED_INT);

	}


	/**
	*	@brief Recieve the number of elements that fill the layout
	*	@param cont number of elements to be added in the VAO
	*/
	template<>
	void VertexBufferLayout::Push<unsigned char>(unsigned int cont)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE,cont,GL_TRUE });
		m_Stride += cont * VertexBufferElement::GetSizeofType(GL_UNSIGNED_BYTE);

	}

}