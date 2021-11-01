#ifndef ARCBALL_HPP
#define ARCBALL_HPP

#include <vector>

#include "cad-base/camera.hpp"
#include "cad-base/viewport_input.hpp"

class Arcball : public ViewportInput {
    public:
        explicit Arcball(std::vector<std::shared_ptr<Camera>> affected_cameras);

        glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);
        void Rotate(std::shared_ptr<glm::vec2> window_size);
        void Pan();
        void Zoom();

        float arcball_rotate_sensitivity, arcball_pan_sensitivity;
        
    private:
        const float ARCBALL_ROTATE_SENSITIVITY_INITIAL = 4.0f; 
        const float ARCBALL_PAN_SENSITIVITY_INITIAL = 100.0f; 
};

#endif