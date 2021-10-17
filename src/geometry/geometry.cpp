#include "cad-base/geometry/geometry.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <vector>

using std::vector;
using glm::vec3;

Geometry::Geometry(vector<vec3> vert_data, glm::vec3 origin) : origin_(origin) {
	vertexes = vert_data;
	colours = vert_data;

	GenerateFlatBuffers();
}

Geometry::Geometry(vector<vec3> vert_data, vector<vec3> colour_data, glm::vec3 origin) : origin_(origin) {
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

		glm::vec3 offset_vertex = vertex + origin_;	//TODO: should we pass this through to GLSL and do this there or something?
		flat_verts.push_back(offset_vertex.x);
		flat_verts.push_back(offset_vertex.y);
		flat_verts.push_back(offset_vertex.z);

		if (offset_vertex.x < aa_bounding_box.min.x) aa_bounding_box.min.x = offset_vertex.x;
		if (offset_vertex.y < aa_bounding_box.min.y) aa_bounding_box.min.y = offset_vertex.y;
		if (offset_vertex.z < aa_bounding_box.min.z) aa_bounding_box.min.z = offset_vertex.z;
		if (offset_vertex.x > aa_bounding_box.max.x) aa_bounding_box.max.x = offset_vertex.x;
		if (offset_vertex.y > aa_bounding_box.max.y) aa_bounding_box.max.y = offset_vertex.y;
		if (offset_vertex.z > aa_bounding_box.max.z) aa_bounding_box.max.z = offset_vertex.z;
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