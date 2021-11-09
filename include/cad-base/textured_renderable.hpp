#ifndef TEXTUREDRENDERABLE_HPP
#define TEXTUREDRENDERABLE_HPP

#include <glm/fwd.hpp>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad-base/geometry/geometry.hpp"
#include "cad-base/renderable.hpp"

class TexturedRenderable : public Renderable {
    public:
        TexturedRenderable(GLuint texture_shader, GLuint basic_shader, GLuint texture, std::shared_ptr<Geometry> geo_ptr, glm::vec4 texture_tint = glm::vec4(1.0f), GLuint render_primative = GL_POINTS);
        TexturedRenderable() = default;

        virtual void Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix) override;
        virtual GLuint GetGeometryVAO() override;

        GLuint shader, render_type;
        GLuint texture, geometry_uv_vbo;
        GLuint texture_shader;
        glm::vec4 texture_tint;
};  

#endif