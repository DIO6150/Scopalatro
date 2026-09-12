#pragma once

#include <map>
#include <vector>

#include <Core/ResourceHandle.hpp>

#include <Internal/Hash.hpp>

namespace TLOT
{
	// TODO: should be able to "compress" list of textures
	// i.e. Insert ({0, 1, 2}) and Insert ({0, 1, 2}) should return the same index << solved
	// Insert ({1, 2}) could return the index+1 but idk seems kinda hard (or maybe xor magic can save me ???)
	// (many month later... -> i think it might be hard because I would need to break current hash system)
	//
	// Maybe add a "ref counter" that allow space reusing when deleting instances ?? idk
	class TextureIndexCache
	{
	public:
		uint32_t GetIndex (std::vector<ResourceHandle> textures)
		{
			if (textures.empty ()) return 0;

			ID64_t hash = GenerateOrderedSetHash (textures); // Generating a ordered hash from the handle allows to reuse space when instances need the same set of textures
			// i really like this, i thought i was very smart when i though of this ehehe

			if (m_texturesIndices.find (hash) != m_texturesIndices.end ())
			{
				return m_texturesIndices[hash];
			}

			uint32_t index = m_nextIndex;

			m_texturesIndices[hash] = index;

			m_nextIndex += textures.size ();

			return index;
		}

		ID64_t GetHash (std::vector<ResourceHandle> textures)
		{
			return GenerateOrderedSetHash (textures);
		}

	private:
		std::map<ID64_t, size_t> m_texturesIndices;
		uint32_t m_nextIndex = 0;
	};
}