#pragma once

#include <stdint.h>

#include <map>
#include <set>
#include <vector>

#include <Transform.hpp>

#include <MeshData/Mesh.hpp>

#include <Renderer/Camera.hpp>
#include <AssetManager.hpp>
#include <Renderer/IRenderer.hpp>
#include <Renderer/RendererCommon.hpp>
#include <Renderer/InstanceRenderer/DrawCommandCache.hpp>
#include <Renderer/InstanceRenderer/InstanceIndexCache.hpp>
#include <Renderer/InstanceRenderer/InstanceIndexOffsetCache.hpp>
#include <Renderer/InstanceRenderer/GeometryOffsetCache.hpp>
#include <Renderer/InstanceRenderer/TextureIndexCache.hpp>

#include <OpenGL/DrawCommand.hpp>
#include <OpenGL/ShaderProgram.hpp>
#include <OpenGL/VAO.hpp>
#include <OpenGL/BufferObject/VBO.hpp>
#include <OpenGL/BufferObject/EBO.hpp>
#include <OpenGL/BufferObject/SSBO.hpp>
#include <OpenGL/BufferObject/DIBO.hpp>
#include <OpenGL/BufferObject/FBO.hpp>
#include <OpenGL/TextureAtlas/TextureAtlas.hpp>



namespace TLOT
{
	enum class ProjectionMode
	{
		Perspective,
		Orthogonal
	};

	class InstanceRenderer : public IRenderer
	{
	public:
		InstanceRenderer (size_t windowWidth, size_t windowHeight, ProjectionMode mode);

		GeometryID RegisterGeometry (Mesh const & mesh) override;
		InstanceID RegisterInstance (GeometryID id, Transform const transform, Material const material) override;
		void RegisterTexture (ResourceHandle handle, Texture const & texture) override;
		void UpdateInstanceMaterial  (InstanceID id, Material const & material) override;
		void UpdateInstanceTransform (InstanceID id, Transform transform) override;
		void UnregisterInstance (InstanceID id) override;
		void Render (ResourceHandle shaderProgramHandle, Camera const & camera) override;

		glm::mat4 GetProjection () { return m_projection; }

		~InstanceRenderer ();

	private:
		uint32_t GetAtlasHandle () override
		{
			return m_atlas.Get ();
		}

		void SyncGPU ();

		struct alignas(16) TextureOffset
		{
			float x, y, w, h, d;

			TextureOffset (ResourceHandle handle, TextureAtlas & atlas)
			{
				TextureQuad const & textureQuad = atlas.Quad (handle);

				x = textureQuad.x / atlas.Width  ();
				w = textureQuad.w / atlas.Width  ();
				y = textureQuad.y / atlas.Height ();
				h = textureQuad.h / atlas.Height ();
				d = static_cast<float> (textureQuad.depth);
			}
		};

		struct TextureOffsetCollection
		{
			size_t index;
			std::vector<TextureOffset> textures;
		};

		struct Geometry
		{
			Vertices_t   const vertices;
			Indices_t    const indices;
		};

		struct alignas(16) InstanceMaterial
		{
			glm::vec3 color;
			uint32_t diffuseTextureIndex;
			uint32_t diffuseTextureCount;
		};

		struct alignas(16) Instance
		{
			glm::mat4 model;
			InstanceMaterial mat;
		};



		InstanceMaterial CreateInstanceMaterial (Material const & material);
		ShaderProgram CreateShader (ProtoShader protoShader);

		ProjectionMode m_mode;
		glm::mat4 m_projection;
	
		constexpr static const size_t s_initialVertexCount    = 1000000;
		constexpr static const size_t s_initialIndexCount     = 1000000;
		constexpr static const size_t s_initialInstanceCount  = 10000;

		VAO  m_vao;
		VBO  m_vbo  = {s_initialVertexCount  , GL_DYNAMIC_DRAW};
		EBO  m_ebo  = {s_initialIndexCount   , GL_DYNAMIC_DRAW};
		DIBO m_dibo = {s_initialInstanceCount, GL_DYNAMIC_DRAW};
		SSBO<uint32_t>      m_ssboInstanceIndex = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 0};
		SSBO<Instance>      m_ssboInstanceData  = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 1};
		SSBO<TextureOffset> m_ssboTextureOffset = {s_initialInstanceCount, GL_DYNAMIC_DRAW, 2};

		TextureAtlas m_atlas;



		GeometryID m_nextGeometryID = 0;
		InstanceID m_nextInstanceID = 0;
		
		std::map<size_t, Geometry> m_geometryToSend; // offset -> data
		std::map<size_t, Instance> m_instanceToSend; // index -> data
		std::map<size_t, InstanceMaterial> m_materialToUpdate;            // index -> data
		std::map<size_t, glm::mat4> m_transformToUpdate;                  // index -> data
		std::map<size_t, TextureOffsetCollection> m_texturesOffsetToSend; // data
		//std::map<size_t, DrawCommand> m_commandsToSend;


		std::map<ID64_t, GeometryID>    m_meshToGeometryID; // mesh hash -> geometryID

		std::set<GeometryID> m_registeredGeometry;
		std::set<InstanceID> m_registerdInstances;

		std::map<ID64_t, ShaderProgram> m_registeredShaders;
		
		
		GeometryOffsetCache      m_geometryOffsetCache;
		InstanceIndexOffsetCache m_instanceIndexOffsetCache;
		InstanceIndexCache       m_instanceIndexCache;
		TextureIndexCache        m_textureIndexCache;
		DrawCommandCache         m_drawCommandCache;


		std::map<InstanceID, GeometryID> m_instanceToGeometry; // cache what instance are from which gometry

		bool m_shouldSkipSync = false;
	};
}
