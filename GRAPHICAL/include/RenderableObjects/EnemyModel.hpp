#pragma once

#include <AssetManager.hpp>
#include <RenderableObject.hpp>
#include <HoverMap.hpp>
#include <RenderableObjects/ToolTipModel.hpp>


class EnemyModel : public TLOT::CommonObject
{
public:
	EnemyModel(TLOT::Renderer * renderer)
	: CommonObject{renderer, TLOT::AssetManager::Cache("geometry_card"), TLOT::AssetManager::Cache("material_enemy")}
	{
		m_material.Set("texture", renderer->Quad(TLOT::AssetManager::Cache("texture_enemy_badalisc")));

		SetScale(glm::vec3{354.0f, 200.0f, 1.0f});

		UpdateMaterial();
		UpdateTransform();
	}
};