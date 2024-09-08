// Icon.hpp
#pragma once




#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>
#include <utility>

#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb/stb_truetype.h"

namespace Vicetrice
{
	enum class IconType
	{
		STATICTEXTICON
	};

	template<typename T>
	class Icon
	{
	public:
		Icon(std::vector<T> &Vertices);
		virtual ~Icon()
		{

		}

		inline virtual const std::vector<T> &GetVertices()
		{
			return m_vertices;
		}
		virtual void OnClick(void(*func)());
		virtual void AddToContext(std::vector<float>& ContextVertices, std::vector<unsigned int>& Indices, unsigned int IconNumber);
		virtual IconType Icontype();

	protected:

		//std::unordered_map<std::string, std::unique_ptr<Icon>> m_icons;
		std::vector<T> m_vertices;
	}; //class Icon

} // namespace Vicetrice

#include "Icon.inl"
