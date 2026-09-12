#pragma once

#include <glad/glad.h>
#include <stdint.h>

#include <MeshData/Vertex.hpp>

#include <Core/Logger.hpp>

namespace TLOT {
	class VBO
	{
	public:

		// maybe will support other vertex type in the future
		VBO (uint32_t vertexCount, GLenum usage):
			m_vertexCount {vertexCount},
			m_usage {usage}
		{
			
		}

		void Create ()
		{
			Logger::log (LogLevel::Info, "VBO::Create () call begin.");
			
			glGenBuffers (1, &m_object);
			glBindBuffer (GL_ARRAY_BUFFER, m_object);
			
			// POSITIONS
			glVertexAttribPointer (
				0, 
				3,
				GL_FLOAT,
				GL_FALSE,
				11 * sizeof (float),
				(void *) 0
			);

			// NORMALS
			glVertexAttribPointer (
				1, 
				3,
				GL_FLOAT,
				GL_FALSE,
				11 * sizeof (float),
				(void *) (3 * sizeof (float))
			);

			// TANGEANTS
			glVertexAttribPointer (
				2, 
				3,
				GL_FLOAT,
				GL_FALSE,
				11 * sizeof (float),
				(void *) (6 * sizeof (float))
			);

			// UV COORDINATES
			glVertexAttribPointer (
				3, 
				2,
				GL_FLOAT,
				GL_FALSE,
				11 * sizeof (float),
				(void *) (9 * sizeof (float))
			);

			glEnableVertexAttribArray (0);
			glEnableVertexAttribArray (1);
			glEnableVertexAttribArray (2);
			glEnableVertexAttribArray (3);

			glNamedBufferData (m_object, sizeof (Vertex) * m_vertexCount, NULL, m_usage);

			Logger::log (LogLevel::Info, "VBO::Create () call end.");
		}
		
		// TODO-fix: index and size checking + resizing
		void Push (std::vector<Vertex> const vertices, size_t offset)
		{
			glNamedBufferSubData (m_object, offset * sizeof (Vertex), vertices.size () * sizeof (Vertex), (void *) vertices.data ());
		}
		
		void Bind ()
		{
			glBindBuffer (GL_ARRAY_BUFFER, m_object);
		}
		
		void Unbind ()
		{
			glBindBuffer (GL_ARRAY_BUFFER, 0);
		}

	private:
		uint32_t m_object;

		uint32_t m_vertexCount = 0;
		GLenum   m_usage;
	};

}
