#pragma once

#include <AssetManager.hpp>
#include <Material/MaterialInstance.hpp>

#include <Card.hpp>

inline void LoadTexture (std::string key, std::string path)
{
	TLOT::AssetManager::Cache (key, TLOT::AssetManager::LoadTexture (path));
}

inline void LoadShaderSource (std::string key, std::string path)
{
	TLOT::AssetManager::Cache (key, TLOT::AssetManager::LoadShaderSource (path));
}
