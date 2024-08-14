// Icon.hpp
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <cassert>

namespace Vicetrice
{

	class Icon
	{
	public:
		Icon();

		void Draw();
		virtual void OnClick(void(*func)());
		void AddIcon();
		virtual void AddToContext(std::vector<float>& ContextVertices, std::vector<unsigned int>& Indices, bool IsWindow, unsigned int IconNumber) ;

	protected:

		std::unordered_map<std::string, std::unique_ptr<Icon>> m_icons;

	private:

	};


} // namespace Vicetrice
