#pragma once

#include <modules/AssetManager.hpp>
#include <modules/Renderable.hpp>
#include <modules/Sprite.hpp>

namespace TLOT
{
	class RenderableManager
	{
	public:
		RenderableManager (AssetManager & assetManager, Renderer & renderer):
			m_assetManager {assetManager},
			m_renderer {renderer}
		{
			
		}

		Renderable Create (GeometryID geometry, Transform transform, Material material)
		{
			if (material.diffuseTextures.size () == 0)
			{
				material.diffuseTextures.push_back ((ResourceHandle)0);
			}

			InstanceID instance = m_renderer.RegisterInstance (geometry, transform, material, m_assetManager);

			Renderable value;
			value.geometry  = geometry;
			value.instance  = instance;
			value.material  = material;
			value.transform = transform;
			value.renderer  = &m_renderer;

			return value;
		}

		Renderable Create (Mesh const & mesh, Transform transform)
		{
			GeometryID geometry = m_renderer.RegisterGeometry (mesh);
			return Create (geometry, transform, mesh.material);
		}

		Sprite CreateSprite ()
		{
			auto quadID = m_assetManager.GetQuadMeshID ();
			auto quad   = m_assetManager.GetMesh (quadID);
			auto renderableBase = Create (quad, {});

			return Sprite {renderableBase, m_assetManager};
		}

		Renderer & GetRenderer () { return m_renderer; }
		AssetManager & GetAssetManager () { return m_assetManager; }

	private:
		AssetManager & m_assetManager;
		Renderer & m_renderer;
	};

	inline static std::vector<ResourceHandle> GetTexturesFromKeys (std::vector<std::string> textureKeys, RenderableManager & manager)
	{
		std::vector<ResourceHandle> textureIDs;

		for (auto & key : textureKeys)
		{
			auto id = manager.GetAssetManager ().GetTextureID (key);
			textureIDs.push_back (id);
			manager.GetRenderer ().RegisterTexture (id, manager.GetAssetManager ().GetTexture (id));
		}

		return textureIDs;
	}
}
