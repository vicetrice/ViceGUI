#pragma once

#include "Icon.hpp"

namespace Vicetrice
{
	template<typename T>
	Icon<T>::Icon(std::vector<T>& Vertices)
		: m_vertices(std::move(Vertices))
	{
	}

	template<typename T>
	void Icon<T>::OnClick(void(*func)())
	{

	}

	template<typename T>
	void Icon<T>::AddToContext(std::vector<float>& ContextVertices, std::vector<unsigned int>& Indices, unsigned int IconNumber)
	{

	}


	//---------------------------------------- StaticTextIcon 

	template<typename T>
	StaticTextIcon<T>::StaticTextIcon(std::vector<T>& Vertices)
		: Icon<T>(Vertices)
	{

	}

	template<typename T>
	StaticTextIcon<T>::~StaticTextIcon()
	{

	}

}

