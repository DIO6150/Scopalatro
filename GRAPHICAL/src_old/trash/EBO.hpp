#pragma once

#include <glad/glad.h>
#include <stdint.h>

namespace TLOT
{
	class EBO
    {
    public:
		EBO (uint32_t indexCount, GLenum usage):
			m_indexCount {indexCount},
			m_usage {usage}
        {
			
		}

		void Create ()
		{
			Logger::log (LogLevel::Info, "EBO::Create () call begin.");

			glGenBuffers (1, &m_object);
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_object);

			glNamedBufferData (m_object, m_indexCount * sizeof (uint32_t), NULL, m_usage);

			Logger::log (LogLevel::Info, "EBO::Create () call end.");
		}
        
        // TODO-fix: index and size checking + resizing
		void Push (std::vector<uint32_t> const & indices, size_t offset)
		{
			glNamedBufferSubData (m_object, offset * sizeof (uint32_t), indices.size () * sizeof (uint32_t), (void *) indices.data ());
		}
        
		void Bind ()
		{
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_object);
		}
        
		void Unbind ()
		{
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		
    private:
        uint32_t m_object;

		uint32_t m_indexCount = 0;
		GLenum m_usage;
	};
}