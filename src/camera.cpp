#include "cad-base/camera.hpp"
#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 initial_position, vec3 initial_target, vec3 initial_up, float initial_distance) 
    :   initial_position_(initial_position), initial_target_(initial_target), initial_up_(initial_up), distance(initial_distance) {
    SetProjection(false);
    rotation = glm::mat4(1.0f);



    view_matrix_ = GetViewMatrix();  //(initial_position_, initial_target_, initial_up_);
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

 glm::mat4 Camera::GetViewMatrix() {
    glm::vec3 unrotated_position = target_ - glm::vec3(0.0f, distance, 0.0f);

    glm::mat4 result = rotation * glm::mat4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),  //TODO: replace this with a stored left/up?
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),     //Up
        glm::vec4(glm::normalize(unrotated_position - target_), 0.0f), //Forward
        glm::vec4(unrotated_position, 1.0f)) ;

    return glm::inverse(result);
}