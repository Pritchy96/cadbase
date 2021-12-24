#ifndef RAY_HPP
#define RAY_HPP

#include <glm/common.hpp>
#include <glm/fwd.hpp>
namespace cad_gui {
    struct Ray {
        glm::vec3 origin;
        glm::vec3 direction;
    };
}
#endif