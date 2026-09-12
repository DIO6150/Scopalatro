#pragma once

#include <AssetManager.hpp>
#include <RenderableObject.hpp>
#include <Card.hpp>
#include <HoverMap.hpp>
#include <RenderableObjects/ToolTipModel.hpp>


class CardModel : public TLOT::CommonObject
{
public:
	CardModel(TLOT::Renderer * renderer, Suit suit, CardValue value)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_card"), TLOT::AssetManager::Cache("material_card")}
	{
		std::string symbol_key = "texture_card_" + Card::GetKey(suit, value);
		m_material.Set("symbol", renderer->Quad(TLOT::AssetManager::Cache(symbol_key)));
		m_material.Set("back",   renderer->Quad(TLOT::AssetManager::Cache("texture_card_back")));

		m_tooltip = std::make_unique<ToolTipModel>(m_renderer);

		UpdateMaterial();
	}

	void UpdateSymbol(Suit suit, CardValue value)
	{
		std::string symbol_key = "texture_card_" + Card::GetKey(suit, value);
		m_material.Set("symbol", m_renderer->Quad(TLOT::AssetManager::Cache(symbol_key)));
		UpdateMaterial();
	}

	void SetPosition(glm::vec3 position) override
	{
		CommonObject::SetPosition(position);

		m_tooltip->SetPosition(position + glm::vec3{-m_tooltip->GetWidth() / 2.0f, m_transform.scale.y, 2.0f});
	}

	void SetTooltip(std::string name, std::string description)
	{
		m_tooltip->SetText(name, description);
	}

	void OnHoverStart()
	{
		m_hovering = true;
	}

	void OnHoverStop()
	{
		m_hovering = false;
	}

	void Destroy() override
	{
		CommonObject::Destroy();
		m_tooltip->Destroy();
	}

	void Render() override
	{
		CommonObject::Render();

		if (m_hovering)
		{
			m_tooltip->Render();
		}
	}

	~CardModel()
	{
		TLOT::Logger::log(TLOT::LogLevel::Error, "no");
	}
private:
	std::unique_ptr<ToolTipModel> m_tooltip;
	bool m_hovering = false;
};