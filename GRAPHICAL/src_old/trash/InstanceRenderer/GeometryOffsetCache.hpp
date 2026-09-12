#pragma once

#include <stdint.h>

#include <map>

#include <Renderer/RendererCommon.hpp>


namespace TLOT
{
	class GeometryOffsetCache {
	private:
		struct GeometryData
		{
			uint32_t offsetVertex;
			uint32_t offsetIndex;
			size_t   indexSize;
		};

	public:
		GeometryOffsetCache () {}

		uint32_t GetVertexOffset (GeometryID geometryID)
		{
			return m_geometryBuffer.at (geometryID).offsetVertex;
		}

		uint32_t GetIndexOffset (GeometryID geometryID) 
		{
			return m_geometryBuffer.at (geometryID).offsetIndex;
		}

		size_t GetIndexCount (GeometryID geometryID)
		{
			return m_geometryBuffer.at (geometryID).indexSize;
		}
		
		void Insert (GeometryID geometryID, uint32_t const vertexSize, uint32_t const indexSize)
		{
			m_geometryBuffer.emplace (geometryID, GeometryData {m_currentVertIndex, m_currentIndIndex, indexSize});
			m_currentVertIndex += vertexSize;
			m_currentIndIndex  += indexSize;
		}

		
		auto find  (GeometryID geometryID) { return m_geometryBuffer.find (geometryID); }
		auto begin () { return m_geometryBuffer.begin (); }
		auto end   () { return m_geometryBuffer.end   (); }

	private:
		std::map<GeometryID, GeometryData> m_geometryBuffer;
		
		uint32_t m_currentVertIndex = 0;
		uint32_t m_currentIndIndex  = 0;
	};
}
