
#include <functional>
#include <optional>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <set>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Renderer/RendererCommon.hpp>
#include <Core/ID.hpp>

#include <Core/ResourceHandle.hpp>

#include <AssetManager.hpp>
#include <DrawCommand.hpp>

struct TextureQuad
{
	float x, y, w, h;
	float z;
};

enum mType
{
	CARD
};

template<mType>
class Model
{
	void Create();
	void Instanciate();
};

template<>
class Model<CARD>
{
	void Create()
	{
		std::string key = "texture_card_" + Card::GetKey(suit, value);
		auto symbolHandle = AssetManager::Cache(key);

		material = AssetManager::CreateMaterial(AssetManager::Cache("material_card"));
		material.Set("symbol",     AssetManager::GetTexture(symbolHandle    ));
		material.Set("back",       AssetManager::GetTexture(backHandle      ));
		material.Set("background", AssetManager::GetTexture(backgroundHandle));
	}

	void Instanciate(Renderer & renderer)
	{
		renderer.Instanciate(AssetManager::Cache("geometry_card"), material);
	}

	Suit suit;
	CardValue value;

	ResourceHandle backHandle;
	ResourceHandle backgroundHandle;

	MaterialInstance material;
};