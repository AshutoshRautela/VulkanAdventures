#include "Mesh.h"

namespace va {
	Mesh::Mesh(std::vector<Vertex>& vertices) : _vertices(vertices) {

	}

	Mesh::~Mesh() {
		this->_vertices.clear();
	}
}
