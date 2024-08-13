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
		virtual void AddContext(const std::vector<float>& ContextVertices, bool IsWindow);

	protected:

		std::unordered_map<std::string, std::unique_ptr<Icon>> m_icons;

	private:

	};


} // namespace Vicetrice
