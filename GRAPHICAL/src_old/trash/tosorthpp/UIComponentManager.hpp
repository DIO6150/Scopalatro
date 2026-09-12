#pragma once

#include <concepts>

#include <modules/AssetManager.hpp>
#include <modules/RenderableManager.hpp>
#include <modules/UIComponent.hpp>



namespace TLOT
{
	class UIComponentManager
	{
	public:
		template<std::derived_from<UIComponent> Component, class ... Args>
		static Component GenerateComponent (RenderableManager & uiRenderableManager, Args && ... args)
		{
			// component will die here
			Component component {std::forward<Args> (args)...};

			for (auto & animations : component.RequestTextures ())
			{
				for (auto & [animationName, textures] : animations)
				{
					auto textureIDs = GetTexturesFromKeys (textures, uiRenderableManager);
					
					if (component.m_sprites.count (animationName) < 1)
					{
						Sprite sprite = uiRenderableManager.CreateSprite ();
						sprite.SetPosition (glm::vec3 {0.0, 0.0, -1000.0});
						
						component.m_sprites.emplace(spriteName, std::move (sprite));
					}
					component.m_sprites.RegisterAnimation (actionName, textureIDs);
				}
			}

			return component;
		}
	};
}