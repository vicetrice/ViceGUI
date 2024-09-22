// Icon.hpp
#pragma once




#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>
#include <utility>

#include "vendor/glm/gtc/matrix_transform.hpp"

namespace Vicetrice
{
	enum class IconType
	{
		STATICTEXTICON,
		BASICICON
	};

	template<typename T>
	class Icon
	{
	public:
		Icon(std::vector<T>& Vertices);
		virtual ~Icon()
		{
			m_vertices.clear();
			m_vertices.shrink_to_fit();
		}

		inline const  std::vector<T>& GetVertices() const
		{
			return m_vertices;
		}
		virtual void OnClick(void (*func)());
		virtual void AddToContext(std::vector<float>& ContextVertices, std::vector<unsigned int>& Indices, unsigned int IconNumber);
		inline virtual IconType Icontype()
		{
			return IconType::BASICICON;  // Retorna el tipo de ícono (modificable)

		}

	protected:

		//std::unordered_map<std::string, std::unique_ptr<Icon>> m_icons;
		std::vector<T> m_vertices;
	}; //class Icon


	template<typename T>
	class StaticTextIcon : public Icon<T>
	{

	public:
		StaticTextIcon(std::vector<T>& Vertices);

		~StaticTextIcon();

		inline IconType Icontype() override
		{
			return IconType::STATICTEXTICON;
		}



	}; //class StaticTextIcon
} // namespace Vicetrice

#include "Icon.inl"
