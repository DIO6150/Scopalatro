#pragma once

#include <map>

#include <Renderer/RendererCommon.hpp>

#include <cassert>

namespace TLOT
{
	class InstanceIndexCache {
	public:
		size_t Insert (InstanceID instanceID) {
			auto index = m_nextIndex++;
			
			m_instances.emplace (instanceID, index);

			return index;
		}

		void Remove (InstanceID instanceID) {
			m_instances.erase (instanceID);
		}

		size_t GetIndex (InstanceID instanceID) {
			assert (m_instances.find (instanceID) != m_instances.end ());
			return m_instances[instanceID];
		}

		auto begin () { return m_instances.begin (); }
		auto end   () { return m_instances.end   (); }

	private:

		std::map<InstanceID, size_t> m_instances;

		size_t m_nextIndex = 0;
		
	};
}