#include "cad-base/geometry/geometry.hpp"
#include <glm/glm.hpp>
#include <glm/common.hpp>
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
	aa_bounding_box.max = vertexes[0];
	aa_bounding_box.min = vertexes[0];

	for (auto vertex : vertexes) {
		flat_verts.push_back(vertex.x);
		flat_verts.push_back(vertex.y);
		flat_verts.push_back(vertex.z);

		if (vertex.x < aa_bounding_box.min.x) aa_bounding_box.min.x = vertex.x;
		if (vertex.y < aa_bounding_box.min.y) aa_bounding_box.min.y = vertex.y;
		if (vertex.z < aa_bounding_box.min.z) aa_bounding_box.min.z = vertex.z;
		if (vertex.x > aa_bounding_box.max.x) aa_bounding_box.max.x = vertex.x;
		if (vertex.y > aa_bounding_box.max.y) aa_bounding_box.max.y = vertex.y;
		if (vertex.z > aa_bounding_box.max.z) aa_bounding_box.max.z = vertex.z;
	}

	aa_bounding_box.GenerateBoundingBox();

	for (auto colour : colours) {
		flat_cols.push_back(colour.x);
		flat_cols.push_back(colour.y);
		flat_cols.push_back(colour.z);
	}

	buffers_invalid = false;
	return vertexes.size();
}