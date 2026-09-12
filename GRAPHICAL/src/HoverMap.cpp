#include <HoverMap.hpp>

using namespace TLOT;

void HoverMap::Hover(IObject * id, double dt)
{
	m_hoverCount[id].frameCount += dt;
	m_hoverCount[id].hoveredThisFrame = true;
}

HoverType HoverMap::GetHover(IObject * id)
{
	auto count = m_hoverCount[id].frameCount;
	
	if (count >= 1.0)
		return HoverType::Long;
	else if (count >= 0.5)
		return HoverType::Medium;
	else if (count >= 0.16)
		return HoverType::Short;
	else if (count >=  0.03)
		return HoverType::Instant;

	return HoverType::None;
}

void HoverMap::Update()
{
	for (auto& [id, info] : m_hoverCount)
    {
        if (!info.hoveredThisFrame)
            info.frameCount = 0.0f;
        
        info.hoveredThisFrame = false;
    }
}