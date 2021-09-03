#pragma once

#include <vector>
#include "Vertex.h"


namespace va {
	class Mesh
	{
	public:
		Mesh() {}
		Mesh(std::vector<Vertex>&&, std::vector<uint16_t>&&);
		~Mesh();

		inline const std::vector<Vertex>& getVertices() const { return this->_vertices; }
		inline const std::vector<uint16_t>& getIndices() const { return this->_indices; }

	private:
		std::vector<Vertex> _vertices;
		std::vector<uint16_t> _indices;
	};
}

