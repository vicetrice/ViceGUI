#pragma once
#include <unordered_map>
#include <memory>
#include <string>

namespace Vicetrice
{
	template <typename T>
	class Icon
	{
	public:

		Icon(T* context, float width, float height);

		void Draw();

		virtual void OnClick(void(*func)());

		void AddIcon(float width, float height);

		virtual void AddContext();

	protected:

		T* m_context;
		float m_width;
		float m_height;
		


		std::unordered_map<std::string, std::unique_ptr<Icon>> m_icons;
	};


}; //namespace Vicetrice