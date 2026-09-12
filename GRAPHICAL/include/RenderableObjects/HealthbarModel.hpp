#pragma once

#include <AssetManager.hpp>
#include <RenderableObject.hpp>

#include <Core/Logger.hpp>

class HealthbarModel : public TLOT::CommonObject
{
public:
	HealthbarModel(TLOT::Renderer * renderer, float health, float poison, bool right = true)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_quad"), TLOT::AssetManager::Cache("material_healthbar")}
	, m_text {renderer, TLOT::AssetManager::Cache("font_hh_light"), TLOT::AssetManager::Cache("font_hh_it"), TLOT::AssetManager::Cache("font_hh_bold")}
	, m_right {right}
	{
		m_material.Set("health", health);
		m_material.Set("poison", poison);
		m_material.Set("fullHealthBar",  renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_full")));
		m_material.Set("emptyHealthBar", renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_empty")));
		m_material.Set("fullPoisonBar",  renderer->Quad(TLOT::AssetManager::Cache("texture_poisonbar_full")));
		m_material.Set("emptyPoisonBar", renderer->Quad(TLOT::AssetManager::Cache("texture_poisonbar_empty")));

		auto quad = renderer->Quad(TLOT::AssetManager::Cache("texture_healthbar_full"));

		height = quad.h * width / quad.w;

		SetScale({width, height, 0.});
		SetRotation(glm::radians(glm::vec3 {0.0, 0.0, 90.0}));

		m_materialUpdateFlag = true;
	}

	void SetHealth(float hp, float maxhp)
	{
		m_hp = hp;
		m_maxhp = maxhp;
		float health;

		if (maxhp != 0)
			health = hp / maxhp;
		else
			health = 0;
		
		m_material.Set("health", health);
		m_text.SetText(std::format("{{C:WHITE, S:BOLD}}{}{{C:BLACK}}/{{C:WHITE, S:BOLD}}{}", static_cast<int>(hp), static_cast<int>(maxhp)), 44);
		UpdateTextPosition();

		UpdateMaterial();
		UpdateTransform();
	}

	void SetPoison(float poison)
	{
		float m_poisonRatio = poison;
		m_material.Set("poison", poison);
		UpdateMaterial();
	}

	void Render() override
	{
		CommonObject::Render();
		
		m_text.Render();
	}

	void Destroy() override
	{
		CommonObject::Destroy();
		m_text.Destroy();
	}

	void SetPosition(glm::vec3 position) override
	{
		CommonObject::SetPosition(position);
		
		UpdateTextPosition();
	}

	std::pair<int, int> GetHealth()
	{
		return {m_hp, m_maxhp};
	}

	float GetStoredPoison()
	{
		return m_poisonRatio;
	}

	float GetHeight()
	{
		return height;
	}

	float GetWidth()
	{
		return width;
	}

private:
	void UpdateTextPosition()
	{
		auto position = GetPosition();
		if(m_right)
			m_text.SetPosition(position + glm::vec3{height, (width - m_text.GetHeight()) / 2, 0.1});
		else
			m_text.SetPosition(position + glm::vec3{- m_text.GetWidth() - 5, (width - m_text.GetHeight()) / 2, 0.1});
	}

	TLOT::TextObject m_text;

	size_t width  = 400;
	size_t height;

	float m_poisonRatio = 0.0;

	float m_hp = 0;
	float m_maxhp = 0;

	bool m_right;
};