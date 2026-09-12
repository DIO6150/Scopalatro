#pragma once

#include <MeshData/Mesh.hpp>

#include <Renderer/Camera.hpp>
#include <Renderer/RendererCommon.hpp>

#include <Resources/Texture.hpp>

#include <Transform.hpp>

namespace TLOT
{
	class IRenderer
	{
	public:
		virtual GeometryID RegisterGeometry (Mesh const & mesh) = 0;
		virtual InstanceID RegisterInstance (GeometryID id, Transform const transform, Material const material) = 0;
		virtual void RegisterTexture (ResourceHandle handle, Texture const & texture) = 0;
		virtual void UpdateInstanceMaterial  (InstanceID id, Material const & material)= 0;
		virtual void UpdateInstanceTransform (InstanceID id, Transform transform) = 0;
		virtual void UnregisterInstance (InstanceID id) = 0;
		virtual void Render (ResourceHandle shaderHandle, Camera const & camera) = 0;

	private:
		virtual uint32_t GetAtlasHandle () = 0;

		friend class SceneEditor;
	};
}