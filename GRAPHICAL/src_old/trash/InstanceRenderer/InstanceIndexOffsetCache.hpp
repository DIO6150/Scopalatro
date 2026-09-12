#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include <Renderer/RendererCommon.hpp>

//TODO : idea: PullChanges () <- returns all the changes from last call, nothing if no change and all data if never called before

namespace TLOT
{
	class InstanceIndexOffsetCache
	{
	public:
		void Insert (GeometryID geometry, size_t offset)
		{
			m_blocks[geometry].push_back (offset);
			m_pullable = true;
		}

		void Remove (GeometryID geometry, size_t offset)
		{
			auto it = m_blocks.find(geometry);
			if (it != m_blocks.end()) 
			{
				auto& v = it->second;
				auto findIt = std::find(v.begin(), v.end(), offset);

				if (findIt != v.end())
				{
					*findIt = v.back();
					v.pop_back();
				}
			}

			m_pullable = true;
		}

		std::vector<std::pair<size_t, size_t>> Pull ()
		{
			if (!m_pullable) return {};
			
			std::vector<std::pair<size_t, size_t>> result;

			size_t index = 0;
			for (auto const & [geometry, offsets] : m_blocks)
			{
				for (auto const & offset : offsets)
				{
					result.push_back ({index++, offset});
				}
			}

			m_pullable = false;
			return result;
		}

	private:
		bool m_pullable = true;
		std::map<GeometryID, std::vector<size_t>> m_blocks;
		
	};
}