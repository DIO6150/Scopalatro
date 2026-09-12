#pragma once

#include <glm/glm.hpp>

#include <Renderer/RendererCommon.hpp>
#include <Renderer/Renderables/Sprite.hpp>

class HealthBar : public TLOT::Sprite
{
public:
	HealthBar (TLOT::Sprite && base) : Sprite (std::move (base)) {}

	float maxHP = 0;
	float HP = 0;
	float poison = 0;
};