#include "Mesh.h"

namespace va {
	Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices) : _vertices(vertices), _indices(indices) {
	}

	Mesh::~Mesh() {
		this->_vertices.clear();
		this->_indices.clear();
	}
}
