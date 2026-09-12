#pragma once

#include <regex>

#include <AssetManager.hpp>
#include <RenderableObject.hpp>

#include <Internal/Style/StyleLexer.hpp>

#include <iostream>

class ToolTipModel : public TLOT::CommonObject
{
public:
	ToolTipModel(TLOT::Renderer * renderer)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_quad"), TLOT::AssetManager::Cache("material_box9patch")}
	{

	}

	void SetText(std::string formattedName, std::string formattedDescription)
	{
		if (m_name)
		{
			m_name->Destroy();
		}
		
		for (auto & line : m_lines)
		{
			line->Destroy();
		}
		


		auto const & transform = GetTransform();
		m_totalWidth  = 0;
		m_totalHeight = 0;
		float offsetY = 5;


		m_name = std::make_unique<TLOT::TextObject>(
			m_renderer,
			TLOT::AssetManager::Cache("font_hh_light"),
			TLOT::AssetManager::Cache("font_hh_it"),
			TLOT::AssetManager::Cache("font_hh_bold")
		);
		
		auto stringlines = TLOT::SplitString(formattedDescription, '\n');
		for (int index = stringlines.size() - 1; index >= 0; index--)
		{
			m_lines.emplace_back(std::make_unique<TLOT::TextObject>(
				m_renderer,
				TLOT::AssetManager::Cache("font_hh_light"),
				TLOT::AssetManager::Cache("font_hh_it"),
				TLOT::AssetManager::Cache("font_hh_bold")
			));
			
			auto & line = m_lines.back();
			line->SetText(stringlines.at(index), 20);

			offsetY += line->GetHeight() + 5;
			m_totalWidth = std::max(m_totalWidth, line->GetWidth());
		}

		m_name->SetText(formattedName, 22);
		m_totalWidth = std::max(m_totalWidth, m_name->GetWidth() + 10);
		offsetY += m_name->GetHeight() + 15;

		m_totalHeight = offsetY + 10;
		m_totalWidth  = m_totalWidth + 10;

		SetPosition(transform.position);
		SetScale({m_totalWidth, m_totalHeight, 0.});

		m_material.Set("containerSize", glm::vec2(m_totalWidth, m_totalHeight));
		m_material.Set("borderSize",    16.0f);
		m_material.Set("corner", m_renderer->Quad(TLOT::AssetManager::Cache("texture_box_corner")));
		m_material.Set("side",   m_renderer->Quad(TLOT::AssetManager::Cache("texture_box_side")));
		m_material.Set("center", m_renderer->Quad(TLOT::AssetManager::Cache("texture_box_center")));
		m_material.Set("color",  glm::vec3(1.0, 1.0, 1.0));

		UpdateTransform();
		UpdateMaterial();
	}

	void Destroy()
	{
		CommonObject::Destroy();
		m_name->Destroy();
		for (auto & line : m_lines)
		{
			line->Destroy();
		}
	}

	void SetPosition(glm::vec3 position)
	{
		CommonObject::SetPosition(position);

		auto const & transform = GetTransform();

		float offsetY = 10;
		float offsetX = 5;

		for (auto & line : m_lines)
		{
			line->SetPosition(transform.position + glm::vec3(offsetX, offsetY, 0.1f));
			offsetY += line->GetHeight() + 5;
		}

		offsetY += 15;
		
		if (m_name)
			m_name->SetPosition(transform.position + glm::vec3 {(m_totalWidth - m_name->GetWidth()) / 2, offsetY, 0.1f});

		UpdateTransform();
	}

	float GetWidth()
	{
		return m_totalWidth;
	}

	float GetHeight()
	{
		return m_totalHeight;
	}

	void Render()
	{
		CommonObject::Render();

		m_name->Render();
		for (auto & line : m_lines)
		{
			line->Render();
		}
	}

private:
	std::unique_ptr<TLOT::TextObject> m_name;
	std::vector<std::unique_ptr<TLOT::TextObject>> m_lines;

	float m_totalWidth  = 0;
	float m_totalHeight = 0;

};