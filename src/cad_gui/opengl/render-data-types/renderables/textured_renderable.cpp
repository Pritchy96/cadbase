#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad_data/feature.hpp"
#include "cad_gui/opengl/shader.hpp"
#include "cad_gui/opengl/renderables/textured_renderable.hpp"
#include "cad_gui/opengl/renderables/renderable.hpp"

namespace cad_gui {
	TexturedRenderable::TexturedRenderable(GLuint texture_shader, GLuint basic_shader, GLuint texture, std::shared_ptr<cad_data::Feature> geo_ptr, glm::vec4 texture_tint, GLuint render_primative) 
		: cad_gui::Renderable(basic_shader, geo_ptr, render_primative), texture(texture), texture_shader(texture_shader),
		texture_tint(texture_tint) {
		//TODO: figure out some static store for shaders.
	}

	GLuint TexturedRenderable::GetFeatureVAO() {
		if (valid_geometry_vao) {	
			return Renderable::GetFeatureVAO();
		}

		Renderable::GetFeatureVAO();

		glBindVertexArray(geometry_vao);

		glEnableVertexAttribArray(2);
		glGenBuffers(1, &geometry_uv_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, geometry_uv_vbo);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBufferData(GL_ARRAY_BUFFER, feature->flat_uvs.size() * sizeof(float), feature->flat_uvs.data(), GL_STREAM_DRAW);

		// Deselect VAO (good practice)
		glBindVertexArray(0);

		return geometry_vao;
	}

	void TexturedRenderable::Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix) {
		//If we can't draw anything, return
		if (!(feature->draw_feature && draw_feature) && !(feature->draw_aa_bounding_box && draw_aa_bounding_box)) {
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

		// Set texture tint
		GLuint texture_tint_id = glGetUniformLocation(texture_shader, "texture_tint");
		glUniform4fv(texture_tint_id, 1, glm::value_ptr(texture_tint));

		// TODO: Pass through and do multiplication GPU side?
		glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
		GLuint shader_id = glGetUniformLocation(texture_shader, "MVP"); 
		glUniform3f(texture_tint_id, texture_tint.r, texture_tint.g, texture_tint.b	);
		glUniformMatrix4fv(shader_id, 1, GL_FALSE, &mvp[0][0]);

		if (feature->draw_feature && draw_feature) {
			glBindVertexArray(GetFeatureVAO());
			glDrawArrays(GL_TRIANGLES, 0, feature->flat_verts.size()/3);
		}

		//TODO: Is this better to be "geo AND renderable bool" or "geo OR renderable bool"
		//Maybe make it "and" but have a seperate "override" for each which is or'd
		//I.e ((feature->draw_aa_bounding_box && draw_aa_bounding_box) || feature->draw_aa_bounding_box_force || draw_aa_bounding_box_force)
		if (feature->draw_aa_bounding_box && draw_aa_bounding_box) {
			//TODO: is there a better way to do this than to have two VAOs?
			glUseProgram(shader);

			glBindVertexArray(GetAABoundingBoxVao());
			glDrawArrays(GL_LINES, 0, feature->aa_bounding_box.flat_verts.size()/3);
		}
	}
}