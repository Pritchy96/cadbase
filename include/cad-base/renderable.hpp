#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad-base/geometry/geometry.hpp"

class Renderable {
    public:
        Renderable(GLuint shader, std::shared_ptr<Geometry> geo_ptr, GLuint render_primative = GL_POINTS);
        Renderable() = default;

        virtual GLuint GetGeometryVAO();
        GLuint geometry_pos_vbo, geometry_col_vbo, geometry_vao;
        bool valid_geometry_vao = false;

        //Axis aligned bounding box
        virtual GLuint GetAABoundingBoxVao();
        GLuint aa_bounding_box_pos_vbo, aa_bounding_box_col_vbo, aa_bounding_box_vao;
        bool valid_aa_bounding_box_vao = false;

        //Only if corresponding bool in geometry is true.
        //Setting these false here will make whatever it is invisible in this viewport ONLY.
        bool draw_geometry = true;
        bool draw_aa_bounding_box = true;

        virtual void Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix);

        std::shared_ptr<Geometry> geometry;
        GLuint basic_shader, render_type;
        glm::mat4 model_matrix = glm::mat4(1.0f);

        bool is_dead = false;
    private:
        int vert_buffer_size_;
};

#endif