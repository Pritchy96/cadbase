#include "cad-base/camera.hpp"
#include <glm/fwd.hpp>

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 initial_position, vec3 initial_target, vec3 initial_up) 
    :   initial_position_(initial_position), initial_target_(initial_target), initial_up_(initial_up) {
    SetProjection(false);
    view_matrix = glm::lookAt(initial_position, initial_target_, initial_up_);
}

void Camera::SetProjection(bool ortho_not_perspective_camera) {
    ortho_not_perspective_ = ortho_not_perspective_camera;
    if (ortho_not_perspective_) {

        // TODO: make these externally modifiable, regenerate matrix when i.e FoV is changed.
        projection_matrix = glm::ortho(ortho_left_, ortho_right_, ortho_bottom_, ortho_top_, z_near_, z_far_);
    } else {
        projection_matrix = glm::perspective(fov_, aspect_ratio_, z_near_, z_far_);
    }
}