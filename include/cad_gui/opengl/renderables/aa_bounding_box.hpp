#ifndef AA_BOUNDING_BOX_HPP
#define AA_BOUNDING_BOX_HPP

#include <glm/glm.hpp>
#include <vector>

namespace cad_gui {
    struct AABoundingBox {
        glm::vec3 min = glm::vec3(INT16_MAX);
        glm::vec3 max = glm::vec3(-INT16_MAX);
        glm::vec3 colour = glm::vec3(1.0f, 1.0f, 1.0f);

        std::vector<float> flat_verts;
        std::vector<float> flat_cols;

        void GenerateBoundingBox() {
            flat_verts = {
                min.x, min.y, min.z, min.x, min.y, max.z,   //Y min
                min.x, min.y, max.z, max.x, min.y, max.z,
                max.x, min.y, max.z, max.x, min.y, min.z,
                max.x, min.y, min.z, min.x, min.y, min.z,
                min.x, min.y, max.z, max.x, min.y, min.z,
                min.x, max.y, min.z, min.x, max.y, max.z,   //Y max
                min.x, max.y, max.z, max.x, max.y, max.z,
                max.x, max.y, max.z, max.x, max.y, min.z,
                max.x, max.y, min.z, min.x, max.y, min.z,
                min.x, max.y, max.z, max.x, max.y, min.z,
                min.x, min.y, min.z, min.x, min.y, max.z,   //X min
                min.x, min.y, max.z, min.x, max.y, max.z,
                min.x, max.y, max.z, min.x, max.y, min.z,
                min.x, max.y, min.z, min.x, min.y, min.z,
                min.x, max.y, min.z, min.x, min.y, max.z,
                max.x, min.y, min.z, max.x, min.y, max.z,   //X max
                max.x, min.y, max.z, max.x, max.y, max.z,
                max.x, max.y, max.z, max.x, max.y, min.z,
                max.x, max.y, min.z, max.x, min.y, min.z,
                max.x, max.y, min.z, max.x, min.y, max.z,
                min.x, min.y, min.z, min.x, max.y, min.z,   //Z min
                min.x, max.y, min.z, max.x, max.y, min.z,
                max.x, max.y, min.z, max.x, min.y, min.z,
                max.x, min.y, min.z, min.x, min.y, min.z,
                max.x, min.y, min.z, min.x, max.y, min.z,
                min.x, min.y, max.z, min.x, max.y, max.z,   //Z max
                min.x, max.y, max.z, max.x, max.y, max.z,
                max.x, max.y, max.z, max.x, min.y, max.z,
                max.x, min.y, max.z, min.x, min.y, max.z,
                max.x, min.y, max.z, min.x, max.y, max.z
            };

            for (int i = 0; i < 60; i++) {
                flat_cols.push_back(colour.r);
                flat_cols.push_back(colour.g);
                flat_cols.push_back(colour.b);
            }
        }
    };
}
#endif
