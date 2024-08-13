#include "Icon.hpp"
#include <cassert>

namespace Vicetrice
{
	Icon::Icon()
	{}

	void Icon::Draw()
	{
	}

	void Icon::OnClick(void(*func)())
	{
	}

	void Icon::AddIcon()
	{
	}

	void Icon::AddContext(const std::vector<float>& ContextVertices, bool IsWindow)
	{
		if (IsWindow)
		{
			assert(ContextVertices.size() >= 12);





		}
		else
		{


		}
	}


}; //namespace Vicetrice
