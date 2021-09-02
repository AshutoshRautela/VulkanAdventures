#pragma once

#include <vector>
#include "Vertex.h"


namespace va {
	class Mesh
	{
	public:
		Mesh() {}
		Mesh(std::vector<Vertex>&);
		~Mesh();

		inline const std::vector<Vertex>& getVertices() const {
			return this->_vertices;
		}

	private:
		std::vector<Vertex> _vertices;
	};
}

