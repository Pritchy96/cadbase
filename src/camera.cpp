#include "cad-base/camera.hpp"
#include <glm/fwd.hpp>

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 initial_position, vec3 initial_target, vec3 initial_up) 
    :   position(initial_position), target(initial_target), up(initial_up) {
    SetProjection(false);
    view_matrix = glm::lookAt(position, target, up);
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

mat4 Camera::GetViewMatrix() {
    // Todo; some way of conditionally re-calculating this (assuming it's expensive to do so).
    view_matrix = glm::lookAt(position, target, up); 
    return view_matrix;
}
mat4 Camera::GetProjectionMatrix() const {
    return projection_matrix;
}