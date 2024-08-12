#include "Icon.hpp"
#include <cassert>

namespace Vicetrice
{

	template<typename T>
	Icon<T>::Icon(T* context, float width, float height) : m_context{ context }, m_width{ width }, m_height{ height }
	{

	}

	template<typename T>
	void Icon<T>::Draw()
	{
		assert(m_context != nullptr);

	}

	template<typename T>
	void Icon<T>::OnClick(void(*func)())
	{
		assert(m_context != nullptr);
	}

	template<typename T>
	void Icon<T>::AddIcon(float width, float height)
	{


	}

	template<typename T>
	void Icon<T>::AddContext()
	{
	}

};