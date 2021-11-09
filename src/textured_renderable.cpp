#include <imgui.h>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad-base/geometry/geometry.hpp"
#include "cad-base/shader.hpp"
#include "cad-base/textured_renderable.hpp"
#include "cad-base/renderable.hpp"

TexturedRenderable::TexturedRenderable(GLuint shader, GLuint texture, std::shared_ptr<Geometry> geo_ptr, GLuint render_primative) : texture(texture), Renderable(shader, geo_ptr, render_primative) {
	//TODO: figure out some static store for shaders.
	texture_shader = shader::LoadShaders((char*)"./shaders/basic_textured.vertshader", (char*)"./shaders/basic_textured.fragshader");
}

GLuint TexturedRenderable::GetGeometryVAO() {
	if (valid_geometry_vao) {	
		return Renderable::GetGeometryVAO();
	}

	Renderable::GetGeometryVAO();

	glBindVertexArray(geometry_vao);

	glEnableVertexAttribArray(2);
	glGenBuffers(1, &geometry_uv_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, geometry_uv_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBufferData(GL_ARRAY_BUFFER, geometry->flat_uvs.size() * sizeof(float), geometry->flat_uvs.data(), GL_STREAM_DRAW);

	// Deselect VAO (good practice)
	glBindVertexArray(0);

	return geometry_vao;
}

void TexturedRenderable::Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix) {
    //If we can't draw anything, return
	if (!(geometry->draw_geometry && draw_geometry) && !(geometry->draw_aa_bounding_box && draw_aa_bounding_box)) {
		return;
	}

	glUseProgram(texture_shader);

	// Get a handle for texture uniform
	GLuint texture_id = glGetUniformLocation(texture_shader, "texture_sampler");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our texture sampler to use Texture Unit 0
	glUniform1i(texture_id, 0);

	// // // TODO: Pass through and do multiplication GPU side?
	glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
	GLuint shader_id = glGetUniformLocation(texture_shader, "MVP"); 
	glUniformMatrix4fv(shader_id, 1, GL_FALSE, &mvp[0][0]);

	if (geometry->draw_geometry && draw_geometry) {
		glBindVertexArray(GetGeometryVAO());
	 	glDrawArrays(GL_TRIANGLES, 0, geometry->flat_verts.size()/3);
	}

	// //TODO: Is this better to be "geo AND renderable bool" or "geo OR renderable bool"
	// //Maybe make it "and" but have a seperate "override" for each which is or'd
	// //I.e ((geometry->draw_aa_bounding_box && draw_aa_bounding_box) || geometry->draw_aa_bounding_box_force || draw_aa_bounding_box_force)
	if (geometry->draw_aa_bounding_box && draw_aa_bounding_box) {
		//TODO: is there a better way to do this than to have two VAOs?
		glUseProgram(shader);

		glBindVertexArray(GetAABoundingBoxVao());
		glDrawArrays(GL_LINES, 0, geometry->aa_bounding_box.flat_verts.size()/3);
	}
}