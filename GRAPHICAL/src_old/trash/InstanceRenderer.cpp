#include <Renderer/InstanceRenderer/InstanceRenderer.hpp>

#include <Core/Logger.hpp>

#include <chrono>

using namespace std::chrono;

using namespace TLOT;

InstanceRenderer::InstanceRenderer (size_t windowWidth, size_t windowHeight, ProjectionMode mode):
	m_mode {mode}
{
	if (mode == ProjectionMode::Perspective)
	{
		m_projection = glm::perspective (glm::radians (45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
	}
	else if (mode == ProjectionMode::Orthogonal)
	{
		m_projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
	}

	m_vao .Bind ();
	m_vbo .Create ();
	m_ebo .Create ();
	m_dibo.Create ();

	m_ssboInstanceIndex.Create ();
	m_ssboInstanceData .Create ();
	m_ssboTextureOffset.Create ();
}

GeometryID InstanceRenderer::RegisterGeometry (Mesh const & mesh)
{
	if (m_registeredGeometry.find (mesh.hash) != m_registeredGeometry.end ())
	{
		return m_meshToGeometryID.at (mesh.hash);
	}

	GeometryID const geometryID = m_nextGeometryID++;

	m_registeredGeometry.emplace (geometryID);
	m_meshToGeometryID.emplace (mesh.hash, geometryID);
	
	m_geometryOffsetCache.Insert (geometryID, mesh.vertices.size (), mesh.indices.size ());
	size_t indexCount = mesh.indices.size ();
	size_t indexOffset  = m_geometryOffsetCache.GetIndexOffset  (geometryID);
	size_t vertexOffset = m_geometryOffsetCache.GetVertexOffset (geometryID);
	
	m_drawCommandCache.RegisterCommand (geometryID, indexCount, vertexOffset, indexOffset);
	
	m_geometryToSend.emplace (geometryID, Geometry {mesh.vertices, mesh.indices});

	Logger::log (LogLevel::Info, "Created geometry with id= {}", geometryID);
	m_shouldSkipSync = false;
	
	return geometryID;
}

InstanceID InstanceRenderer::RegisterInstance (GeometryID geometryID, Transform const transform, Material const material)
{
	if (m_registeredGeometry.find (geometryID) == m_registeredGeometry.end ())
	{
		Logger::log (LogLevel::Error, "Geometry does not exists.");
		return InvalidInstance;
	}

	InstanceID const instanceID = m_nextInstanceID++;

	m_registerdInstances.emplace (instanceID);
	size_t index = m_instanceIndexCache.Insert (instanceID);
	m_instanceIndexOffsetCache.Insert (geometryID, index);

	m_instanceToSend.emplace
	(
		std::piecewise_construct,
		std::forward_as_tuple (index),
		std::forward_as_tuple
		(
			transform.GetModelMatrix (),
			CreateInstanceMaterial (material)
		)
	);

	m_instanceToGeometry.emplace (instanceID, geometryID);

	m_drawCommandCache.AddInstance (geometryID);

	m_shouldSkipSync = false;

	return instanceID;
}

void InstanceRenderer::RegisterTexture (ResourceHandle handle, Texture const & texture)
{
	if (handle == 0) Logger::log (LogLevel::Warning, "Handle is null, be careful : defaultTexture from RegisterTexture");
	m_atlas.Feed (handle, texture);
}

void InstanceRenderer::UpdateInstanceMaterial (InstanceID id, Material const & material)
{
	size_t index = m_instanceIndexCache.GetIndex (id);

	m_materialToUpdate[index] = CreateInstanceMaterial (material);

	m_shouldSkipSync = false;
}

void InstanceRenderer::UpdateInstanceTransform (InstanceID id, Transform transform)
{
	size_t index = m_instanceIndexCache.GetIndex (id);

	m_transformToUpdate[index] = transform.GetModelMatrix ();

	m_shouldSkipSync = false;
}

void InstanceRenderer::UnregisterInstance (InstanceID instanceID)
{
	if (m_registerdInstances.find (instanceID) == m_registerdInstances.end ())
	{
		Logger::log (LogLevel::Warning, "Can't remove Instance, InstanceID was not registered");
		return;
	}

	size_t index = m_instanceIndexCache.GetIndex (instanceID);
	GeometryID geometryID = m_instanceToGeometry.at (instanceID);

	m_registerdInstances.erase (instanceID);
	m_instanceToGeometry.erase (instanceID);
	m_instanceIndexCache.Remove (instanceID);
	m_instanceIndexOffsetCache.Remove (geometryID, index);

	m_drawCommandCache.RemoveInstance (geometryID);

	m_shouldSkipSync = false;
}

void InstanceRenderer::Render (ResourceHandle shaderProgramHandle, Camera const & camera)
{
	if (m_registerdInstances.size () == 0) return;

	ShaderProgram * currentShader;

	if (m_registeredShaders.find (shaderProgramHandle) == m_registeredShaders.end ())
	{
		ProtoShader protoShader = AssetManager::GetShaderCollection (shaderProgramHandle);

		CreateShader (protoShader);
	}
	
	currentShader = &m_registeredShaders.at (shaderProgramHandle);

	SyncGPU ();
	
	//TODO: restore frame buffers functionnalities
	//glBindFramebuffer (GL_FRAMEBUFFER, m_sceneFrameBuffer.Get ());
	glEnable (GL_DEPTH_TEST);

	m_vao.Bind  ();
	m_vbo.Bind  ();
	m_ebo.Bind  ();
	m_dibo.Bind ();

	m_ssboInstanceIndex.Bind ();
	m_ssboInstanceData .Bind ();
	m_ssboTextureOffset.Bind ();

	glActiveTexture(GL_TEXTURE0);

	// TODO-fix: add "ShaderInfo" field to change name (not urgent)
	// TODO-fix: add "ShaderParams" field to send custom uniforms (kinda not urgent but probably won't take too long)
	// both are like the same thing
	currentShader->Use ();
	currentShader->Upload1i ("uAtlas", 0);
	currentShader->UploadMatrix4 ("uProjection", m_projection);
	if (m_mode == ProjectionMode::Perspective) currentShader->UploadMatrix4 ("uView", camera.getView ());
	
	glBindTexture (GL_TEXTURE_2D_ARRAY, m_atlas.Get ());

	glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, m_drawCommandCache.GetCommandCount (), 0);

	currentShader->Cancel ();
	m_vao.Unbind ();
}

InstanceRenderer::InstanceMaterial InstanceRenderer::CreateInstanceMaterial (Material const & material)
{
	// create an index pointing at the begining of a range of textures
	uint32_t diffuseTextureIndex = m_textureIndexCache.GetIndex (material.diffuseTextures);
	uint32_t diffuseTextureCount = material.diffuseTextures.size ();

	ID64_t texturesHash = m_textureIndexCache.GetHash (material.diffuseTextures);

	if (!texturesHash)
	{
		Logger::log (LogLevel::Warning, "Hash is null, texture offset is incorrect");
	}

	InstanceMaterial value;
	value.color = material.color;
	value.diffuseTextureIndex = diffuseTextureIndex;
	value.diffuseTextureCount = diffuseTextureCount;

	// maybe cache it
	std::vector<TextureOffset> diffuseOffsets;
	for (auto const & textureHandle : material.diffuseTextures)
	{
		Texture const & texture = AssetManager::GetTexture (textureHandle);
		m_atlas.Feed (textureHandle, texture);
		diffuseOffsets.emplace_back (textureHandle, m_atlas);
	}

	m_texturesOffsetToSend[diffuseTextureIndex] = TextureOffsetCollection {diffuseTextureIndex, diffuseOffsets};

	return value;
}

ShaderProgram InstanceRenderer::CreateShader (ProtoShader protoShader)
{
	ShaderProgram shader;

	if (protoShader.vertex != InvalidResource)
	{
		ShaderSource vertex = AssetManager::GetShaderSource (protoShader.vertex);
		shader.CompileVertex (vertex.source);
	}

	if (protoShader.fragment != InvalidResource)
	{
		ShaderSource fragment = AssetManager::GetShaderSource (protoShader.fragment);
		shader.CompileFragment (fragment.source);
	}

	shader.Link ();
	
	m_registeredShaders.emplace (protoShader.handle, std::move (shader));

	return m_registeredShaders.at (protoShader.handle);
}

void InstanceRenderer::SyncGPU ()
{
	if (m_shouldSkipSync) return;

	m_atlas.Generate ();

	// one way to optimize this is to merge all consecutive data beforehand and send that instead of every little things

	for (auto const & [handle, geometry] : m_geometryToSend)
	{
		size_t vertexOffset = m_geometryOffsetCache.GetVertexOffset (handle);
		size_t indexOffset  = m_geometryOffsetCache.GetIndexOffset  (handle);

		//Logger::log (LogLevel::Info, "(VBO) Sending Geometry Data (size={}) to gpu at offset={}.", geometry.vertices.size (), vertexOffset);

		m_vbo.Push (geometry.vertices, vertexOffset);
		m_ebo.Push (geometry.indices , indexOffset);
	}

	for (auto const & [index, instance] : m_instanceToSend)
	{
		m_ssboInstanceData.Push (instance, index);
	}

	// reason its like this right now is to update buffer when instances are REMOVED
	// it is, as of now, updated every time this method is called
	for (auto const & [index, offset] : m_instanceIndexOffsetCache.Pull ())
	{

		//Logger::log (LogLevel::Info, "Trying to push instance offset={} at index={}", offset, index);
		m_ssboInstanceIndex.Push (offset, index);
	}

	for (auto const & [_, collection] : m_texturesOffsetToSend)
	{
		size_t index = 0;
		for (auto const & TextureOffset : collection.textures)
		{
			m_ssboTextureOffset.Push (TextureOffset, collection.index + index++);
		}
	}

	for (auto const & [index, material] : m_materialToUpdate)
	{
		//Logger::log (LogLevel::Info, "Trying to update material: Textures : ({} {})", material.diffuseTextureIndex, material.diffuseTextureCount);
		m_ssboInstanceData.PushPart (material, index, sizeof (glm::mat4));
	}
	
	for (auto const & [index, transform] : m_transformToUpdate)
	{
		m_ssboInstanceData.PushPart (transform, index, 0);
	}

	m_dibo.ReplaceAll (m_drawCommandCache.Pull ()); // change that probably

	m_geometryToSend      .clear ();
	m_instanceToSend      .clear ();
	m_texturesOffsetToSend.clear ();
	m_materialToUpdate    .clear ();
	m_transformToUpdate   .clear ();

	m_shouldSkipSync = true;
}

InstanceRenderer::~InstanceRenderer ()
{
	
}
