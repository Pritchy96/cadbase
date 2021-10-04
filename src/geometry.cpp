#include "cad-base/geometry.hpp"
#include <vector>

using std::vector;
using glm::vec3;

Geometry::Geometry(vector<vec3> vert_data) {
	vertexes = vert_data;
	colours = vert_data;
	GenerateFlatBuffers();
}

Geometry::Geometry(vector<vec3> vert_data, vector<vec3> colour_data) {
	vertexes = vert_data;
	colours = colour_data;
	GenerateFlatBuffers();
}

void Geometry::Update(double deltaT) {
	if (buffers_invalid) {
		GenerateFlatBuffers();
	}
}

int Geometry::GenerateFlatBuffers() {
	for (auto vertex : vertexes) {
		flat_verts.push_back(vertex.x);
		flat_verts.push_back(vertex.y);
		flat_verts.push_back(vertex.z);
	}

	for (auto colour : colours) {
		flat_cols.push_back(colour.x);
		flat_cols.push_back(colour.y);
		flat_cols.push_back(colour.z);
	}

	buffers_invalid = false;
	return vertexes.size();
}