#pragma once

#include <Renderer/IRenderer.hpp>
#include <Renderer/Renderables/ModelFactory.hpp>

#include <AssetManager.hpp>

#include <Card.hpp>

class CardModel
{
public:
	static TLOT::Model Create (Suit suit, CardValue value, TLOT::IRenderer & cardRenderer)
	{
		auto key = Card::GetKey (suit, value);
		
		auto cardModel = TLOT::ModelFactory::CreateSprite (cardRenderer);
		cardModel.SetDiffuseTexture (0, TLOT::AssetManager::Cache ("texture_card_back"));
		cardModel.SetDiffuseTexture (1, TLOT::AssetManager::Cache ("texture_card_" + key));

		return cardModel;
	}
};