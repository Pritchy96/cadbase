#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include <vector>

#include "cad-base/camera.hpp"

class Arcball {
    public:
        Arcball(std::vector<std::shared_ptr<Camera>> affected_cameras);
        Arcball() = default;

        glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);
        void Rotate(std::shared_ptr<glm::vec2> window_size);
        void Pan();
        void Zoom();

        std::vector<std::shared_ptr<Camera>> affected_cameras;

        float arcball_rotate_sensitivity, arcball_pan_sensitivity;
        
    private:
        const float ARCBALL_ROTATE_SENSITIVITY_INITIAL = 4.0f; 
        const float ARCBALL_PAN_SENSITIVITY_INITIAL = 100.0f; 
};

#endif