#pragma once

#include <algorithm>
#include <cctype>
#include <string>

#include <AssetManager.hpp>
#include <RenderableObject.hpp>
#include <Card.hpp>
#include <HoverMap.hpp>
#include <RenderableObjects/ToolTipModel.hpp>


class IconModel : public TLOT::CommonObject
{
public:
	IconModel(TLOT::Renderer * renderer, std::string status)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_quad"), TLOT::AssetManager::Cache("material_card")}
	{
		//std::string lowered = std::transform(status.begin(), status.end(), status.begin(), ::tolower);

		m_material.Set("texture",   renderer->Quad(TLOT::AssetManager::Cache("texture_status_" + status )));

		m_status = status;
		m_tooltip = std::make_unique<ToolTipModel>(m_renderer);

		UpdateMaterial();
	}

	void SetPosition(glm::vec3 position) override
	{
		CommonObject::SetPosition(position);

		m_tooltip->SetPosition(position + glm::vec3{-m_tooltip->GetWidth() / 2.0f, m_transform.scale.y, 2.0f});
	}

	void SetTooltip(std::string description)
	{
		m_tooltip->SetText(m_status, description);
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

	~IconModel()
	{
		TLOT::Logger::log(TLOT::LogLevel::Error, "no");
	}
private:
	std::unique_ptr<ToolTipModel> m_tooltip;
	bool m_hovering = false;
	std::string m_status;
};