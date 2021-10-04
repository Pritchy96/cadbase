#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

    // #include "stdafx.h"
    #include <vector>
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
    #define GLM_ENABLE_EXPERIMENTAL
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtx/transform.hpp>

    class Geometry {
        public:
            Geometry() = default;
            explicit Geometry(std::vector<glm::vec3> vert_data);
            Geometry(std::vector<glm::vec3> vert_data, std::vector<glm::vec3> colour_data);

            void Update(double deltaT);

            virtual ~Geometry() = default;
            virtual int GenerateFlatBuffers();

            //TODO: replace these with accessors into flatverts.
            std::vector<glm::vec3> vertexes, colours;
            std::vector<float> flat_verts, flat_cols;
            //If true, the flatvert buffers no longer match the vert buffers
            bool buffers_invalid = true;
            bool visible = true, is_dead = false;
    };

#endif