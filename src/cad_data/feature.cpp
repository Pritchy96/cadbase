#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>
#include <vector>

#include "cad_data/feature.hpp"
#include "spdlog/spdlog.h"

using std::vector;
using glm::vec3;

namespace cad_data {
	Feature::Feature(vector<vec3> vert_data, std::string name, std::shared_ptr<glm::vec3> part_origin) : name(name), part_origin(part_origin) {
		vertexes = vert_data;
		colours = vert_data;

		GenerateFlatBuffers();
	}

	Feature::Feature(vector<vec3> vert_data, std::vector<glm::vec2> uv_data, std::string name, std::shared_ptr<glm::vec3> part_origin) : name(name), part_origin(part_origin) {
		vertexes = vert_data;
		uvs = uv_data;

		GenerateFlatBuffers();
	}


	Feature::Feature(vector<vec3> vert_data, vector<vec3> colour_data, std::string name, std::shared_ptr<glm::vec3> part_origin) : name(name), part_origin(part_origin) {
		vertexes = vert_data;
		colours = colour_data;

		GenerateFlatBuffers();
	}

	Feature::Feature(vector<vec3> vert_data, vector<vec3> colour_data, std::vector<glm::vec2> uv_data, std::string name, std::shared_ptr<glm::vec3> part_origin) : name(name), part_origin(part_origin) {
		vertexes = vert_data;
		colours = colour_data;
		uvs = uv_data;

		GenerateFlatBuffers();
	}

	void Feature::Update() {
		if (buffers_invalid) {
			spdlog::info("Buffers invalid");
			GenerateFlatBuffers();
		}
	}

	int Feature::GenerateFlatBuffers() {
		flat_verts.clear();
		flat_cols.clear();
		aa_bounding_box.min = glm::vec3(INT16_MAX);
		aa_bounding_box.max = glm::vec3(-INT16_MAX);

		glm::vec3 offset;

		// //TODO: get offset from part.	
		if (part_origin == nullptr) {
			spdlog::info("Null pointer");

			offset = glm::vec3(0.0f);
		} else {
			spdlog::info("Not a null pointer");
			offset = *part_origin;
		}

		spdlog::info("offset: " + glm::to_string(offset));


		for (auto vertex : vertexes) {
			glm::vec3 offset_vertex = vertex + offset;	//TODO: should we pass this through to GLSL and do this there or something?
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


		for (auto uv : uvs) {
			flat_uvs.push_back(uv.x);
			flat_uvs.push_back(uv.y);
		}

		buffers_invalid = false;
		return vertexes.size();
	}
}