#include <memory>

#include "cad_gui/opengl/renderables/renderable.hpp"

using std::shared_ptr;

namespace cad_gui {
	GLuint Renderable::GetFeatureVAO() {
		if (!valid_geometry_vao) {

			glGenVertexArrays(1, &geometry_vao);
			glBindVertexArray(geometry_vao);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glGenBuffers(1, &geometry_pos_vbo); 
			glGenBuffers(1, &geometry_col_vbo);

			glBindBuffer(GL_ARRAY_BUFFER, geometry_pos_vbo);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glBufferData(GL_ARRAY_BUFFER, feature->flat_verts.size() * sizeof(float), feature->flat_verts.data(), GL_STREAM_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, geometry_col_vbo);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glBufferData(GL_ARRAY_BUFFER, feature->flat_cols.size() * sizeof(float), feature->flat_cols.data(), GL_STREAM_DRAW);

			// Deselect VAO (good practice)
			glBindVertexArray(0);

			valid_geometry_vao = true;
		}
		return geometry_vao;
	}

	GLuint Renderable::GetAABoundingBoxVao() {
		if (!valid_aa_bounding_box_vao) {

			glGenVertexArrays(1, &aa_bounding_box_vao);
			glBindVertexArray(aa_bounding_box_vao);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glGenBuffers(1, &aa_bounding_box_pos_vbo); 
			glGenBuffers(1, &aa_bounding_box_col_vbo);

			glBindBuffer(GL_ARRAY_BUFFER, aa_bounding_box_pos_vbo);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glBufferData(GL_ARRAY_BUFFER, feature->aa_bounding_box.flat_verts.size() * sizeof(float), feature->aa_bounding_box.flat_verts.data(), GL_STREAM_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, aa_bounding_box_col_vbo);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glBufferData(GL_ARRAY_BUFFER, feature->aa_bounding_box.flat_cols.size() * sizeof(float), feature->aa_bounding_box.flat_cols.data(), GL_STREAM_DRAW);

			// Deselect VAO (good practice)
			glBindVertexArray(0);

			valid_aa_bounding_box_vao = true;
		}
		return aa_bounding_box_vao;
	}

	void Renderable::Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix){
		// //If we can't draw anything, return
		// if (!(feature->draw_feature && draw_feature) && !(feature->draw_aa_bounding_box && draw_aa_bounding_box)) {
		// 	return;
		// }

		// glUseProgram(basic_shader);
		// GLuint shader_id;

		// // TODO: Pass through and do multiplication GPU side?
		// glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
		// shader_id = glGetUniformLocation(basic_shader, "MVP"); 
		// glUniformMatrix4fv(shader_id, 1, GL_FALSE, &mvp[0][0]);

		// if (feature->draw_feature && draw_feature) {
		// 	glBindVertexArray(GetFeatureVAO());
		// 	glDrawArrays(render_type, 0, feature->flat_verts.size()/3);
		// }
		
		// //TODO: Is this better to be "feature AND renderable bool" or "feature OR renderable bool"
		// //Maybe make it "and" but have a seperate "override" for each which is or'd
		// //I.e ((geometry->draw_aa_bounding_box && draw_aa_bounding_box) || geometry->draw_aa_bounding_box_force || draw_aa_bounding_box_force)
		// if (feature->draw_aa_bounding_box && draw_aa_bounding_box) {
		// 	//TODO: is there a better way to do this than to have two VAOs?
		// 	glBindVertexArray(GetAABoundingBoxVao());
		// 	glDrawArrays(GL_LINES, 0, feature->aa_bounding_box.flat_verts.size()/3);
		// }
	}

	Renderable::Renderable(int shader_index, shared_ptr<cad_data::Feature> geo_ptr, GLuint render_primative) : feature(geo_ptr), next_shader(shader_index), render_type(render_primative) {};
}