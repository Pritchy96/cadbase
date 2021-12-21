#ifndef FEATURE_HPP
#define FEATURE_HPP 

// #include "stdafx.h"
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "cad_gui/opengl/renderables/aa_bounding_box.hpp"

namespace cad_data {
    class Feature {
        public:
            Feature() = default;    
            explicit Feature(std::vector<glm::vec3> vert_data, std::string name, std::shared_ptr<glm::vec3> part_origin = std::make_shared<glm::vec3>(0.0f));
            Feature(std::vector<glm::vec3> vert_data, std::vector<glm::vec2> uv_data, std::string name, std::shared_ptr<glm::vec3> part_origin = std::make_shared<glm::vec3>(0.0f));
            Feature(std::vector<glm ::vec3> vert_data, std::vector<glm::vec3> colour_data, std::string name, std::shared_ptr<glm::vec3> part_origin = std::make_shared<glm::vec3>(0.0f));
            Feature(std::vector<glm::vec3> vert_data, std::vector<glm::vec3> colour_data, std::vector<glm::vec2> uv_data, std::string name, std::shared_ptr<glm::vec3> part_origin = std::make_shared<glm::vec3>(0.0f));

            void Update();

            virtual ~Feature() = default;
            virtual int GenerateFlatBuffers();

            std::string name;

            std::vector<Feature> dependent_features;

            std::shared_ptr<glm::vec3> part_origin;    //Where the part is in world space.

            // TODO: replace these with accessors into flatverts.
            // TODO: or do we even need flatverts? we never access it again after creating and loading it on to the GPU, so we could just create the flat structure and load it into the VAO, never storing it CPU side.
            std::vector<glm::vec3> vertexes, colours;
            std::vector<glm::vec2> uvs;
            std::vector<float> flat_verts, flat_cols, flat_uvs;
            // If true, the flatvert buffers no longer match the vert buffers
            bool buffers_invalid = true;
            
            // Only if corresponding bool in renderable is true.
            // Setting these false here will make whatever it is invisible in ALL viewports
            bool draw_feature = true;
            bool draw_aa_bounding_box = false;
            
            bool is_dead = false;

            cad_gui::AABoundingBox aa_bounding_box;  
        private:
    };
}
#endif