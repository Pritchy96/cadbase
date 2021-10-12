#include "cad-base/renderable.hpp"
#include <memory>

using std::shared_ptr;

GLuint Renderable::GetVAO() {
	if (!valid_vao) {

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glGenBuffers(1, &pos_vbo); 
		glGenBuffers(1, &col_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, geometry->flat_verts.size() * sizeof(float), geometry->flat_verts.data(), GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, col_vbo);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, geometry->flat_cols.size() * sizeof(float), geometry->flat_cols.data(), GL_STREAM_DRAW);

		// Deselect VAO (good practice)
		glBindVertexArray(0);

		valid_vao = true;
	}
	return vao;
}

void Renderable::Draw(double deltaT, glm::mat4 projectionMatrix, glm::mat4 viewMatrix){
		if (!geometry->visible) {
			return;
		}

		glUseProgram(shader);
		GLuint shader_id = glGetUniformLocation(shader, "scale");

		// TODO: Pass through and do multiplication GPU side?
		glm::mat4 mvp = projectionMatrix * viewMatrix * model_matrix;
		shader_id = glGetUniformLocation(shader, "MVP"); 
		glUniformMatrix4fv(shader_id, 1, GL_FALSE, &mvp[0][0]);
	
		glBindVertexArray(GetVAO());

		glDrawArrays(render_type, 0, geometry->flat_verts.size());
}

Renderable::Renderable(GLuint Shader, shared_ptr<Geometry> geo_ptr, GLuint renderPrimative) {
	shader = Shader;
	geometry = geo_ptr;
	render_type = renderPrimative;
}