#pragma once

#include <map>

#include <MVPHelper.hpp>
#include <RenderableObject.hpp>

enum class HoverType
{
	None    = -1,
	Instant =  0,
	Short   =  1,
	Medium  =  2,
	Long    =  3,
};

class HoverMap
{
public:
	void Hover(TLOT::IObject * id, double deltaTime);
	HoverType GetHover(TLOT::IObject * id);

	void Update();

private:
	struct HoverCount
	{
		double frameCount = 0.0; // default
		bool hoveredThisFrame = false;
	};

	std::map<TLOT::IObject *, HoverCount> m_hoverCount;
};
