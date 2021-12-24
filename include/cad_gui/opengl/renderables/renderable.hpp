#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad_data/feature.hpp"

namespace cad_gui {
    class Renderable {
        public:
            Renderable(int shader_index, std::shared_ptr<cad_data::Feature> geo_ptr, GLuint render_primative = GL_POINTS);
            Renderable() = default;

            virtual GLuint GetFeatureVAO();
            GLuint geometry_pos_vbo, geometry_col_vbo, geometry_vao;
            bool valid_geometry_vao = false;

            //Axis aligned bounding box
            virtual GLuint GetAABoundingBoxVao();
            GLuint aa_bounding_box_pos_vbo, aa_bounding_box_col_vbo, aa_bounding_box_vao;
            bool valid_aa_bounding_box_vao = false;

            //Only if corresponding bool in feature is true.
            //Setting these false here will make whatever it is invisible in this viewport ONLY.
            bool draw_feature = true;
            bool draw_aa_bounding_box = true;

            virtual void Draw(glm::mat4 projection_matrix, glm::mat4 view_matrix, GLuint shader_program);

            std::shared_ptr<cad_data::Feature> feature;
            int next_shader = 0; //Which shader in the shader array.
            int shader = -1; //The current shader - will be changed to next_shader in the next update loop.

            GLuint render_type;
            
            glm::mat4 model_matrix = glm::mat4(1.0f);

            bool is_dead = false;   
        private:
            int vert_buffer_size_;
    };
}
#endif