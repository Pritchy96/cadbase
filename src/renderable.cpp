#include "cad-base/renderable.hpp"
#include <memory>

using std::shared_ptr;

GLuint Renderable::GetGeometryVAO() {
	if (!valid_geometry_vao) {

		glGenVertexArrays(1, &geometry_vao);
		glBindVertexArray(geometry_vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glGenBuffers(1, &geometry_pos_vbo); 
		glGenBuffers(1, &geometry_col_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, geometry_pos_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, geometry->flat_verts.size() * sizeof(float), geometry->flat_verts.data(), GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, geometry_col_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, geometry->flat_cols.size() * sizeof(float), geometry->flat_cols.data(), GL_STREAM_DRAW);

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
		glBufferData(GL_ARRAY_BUFFER, geometry->aa_bounding_box.flat_verts.size() * sizeof(float), geometry->aa_bounding_box.flat_verts.data(), GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, aa_bounding_box_col_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, geometry->aa_bounding_box.flat_cols.size() * sizeof(float), geometry->aa_bounding_box.flat_cols.data(), GL_STREAM_DRAW);

		// Deselect VAO (good practice)
		glBindVertexArray(0);

		valid_aa_bounding_box_vao = true;
	}
	return aa_bounding_box_vao;
}


void Renderable::Draw(double deltaT, glm::mat4 projectionMatrix, glm::mat4 viewMatrix){
	//If we can't draw anything, return
	if (!(geometry->draw_geometry && draw_geometry) && !(geometry->draw_aa_bounding_box && draw_aa_bounding_box)) {
		return;
	}

	glUseProgram(shader);
	GLuint shader_id = glGetUniformLocation(shader, "scale");

	// TODO: Pass through and do multiplication GPU side?
	glm::mat4 mvp = projectionMatrix * viewMatrix * model_matrix;
	shader_id = glGetUniformLocation(shader, "MVP"); 
	glUniformMatrix4fv(shader_id, 1, GL_FALSE, &mvp[0][0]);

	if (geometry->draw_geometry && draw_geometry) {
		glBindVertexArray(GetGeometryVAO());
		glDrawArrays(render_type, 0, geometry->flat_verts.size()/3);
	}
	
	//TODO: Is this better to be "geo AND renderable bool" or "geo OR renderable bool"
	//Maybe make it "and" but have a seperate "override" for each which is or'd
	//I.e ((geometry->draw_aa_bounding_box && draw_aa_bounding_box) || geometry->draw_aa_bounding_box_force || draw_aa_bounding_box_force)
	if (geometry->draw_aa_bounding_box && draw_aa_bounding_box) {
		//TODO: is there a better way to do this than to have two VAOs?
		glBindVertexArray(GetAABoundingBoxVao());
		glDrawArrays(GL_LINES, 0, geometry->aa_bounding_box.flat_verts.size()/3);
	}
}

Renderable::Renderable(GLuint Shader, shared_ptr<Geometry> geo_ptr, GLuint renderPrimative) {
	shader = Shader;
	geometry = geo_ptr;
	render_type = renderPrimative;
}