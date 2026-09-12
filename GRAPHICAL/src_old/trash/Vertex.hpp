#pragma once

#include <ostream>

namespace TLOT
{
	struct Vertex
	{
		float px, py, pz;
		float nx, ny, nz;
		float tx, ty, tz;
		float u, v;

		// TODO: add this elsewhere to keep class clean
		friend std::ostream& operator<< (std::ostream &out, const Vertex & data)
		{
			out << "([" << data.px << ", " << data.py << ", " << data.pz << "], [" <<
			data.nx << ", " << data.ny << ", " << data.nz << "], [" <<
			data.tx << ", " << data.ty << ", " << data.tz << "], [" <<
			data.px << ", " << data.py << "])";
			return out;
		}
	};
}

/*
{
		VertexTemplate _template {{
			VertexAtom::Vec3,
			VertexAtom::Vec3,
			VertexAtom::Vec3,
			VertexAtom::Vec2,
		}};

		
		std::vector<float> const quadVertices = {
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f
		};
		
		Indices_t const quadIndices = {0, 1, 2, 3, 4, 5};

		m_quadMesh = Mesh {quadVertices, quadIndices, Material {}, 0};
	}
*/