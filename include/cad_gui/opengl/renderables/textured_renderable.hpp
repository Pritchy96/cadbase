#ifndef TEXTURED_RENDERABLE_HPP
#define TEXTURED_RENDERABLE_HPP

#include <glm/fwd.hpp>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad_data/feature.hpp"
#include "cad_gui/opengl/renderables/renderable.hpp"

namespace cad_gui {
    class TexturedRenderable : public Renderable {
        public:
            TexturedRenderable(int texture_shader, GLuint texture, std::shared_ptr<cad_data::Feature> geo_ptr, glm::vec4 texture_tint = glm::vec4(1.0f), GLuint render_primative = GL_POINTS);
            TexturedRenderable() = default;

            virtual void Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix) override;
            virtual GLuint GetFeatureVAO() override;

            GLuint render_type;
            GLuint texture, geometry_uv_vbo;
            glm::vec4 texture_tint;
    };
}

#endif