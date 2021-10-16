#include "cad-base/camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 initial_target, float initial_zoom) 
    :   initial_target_(initial_target), target_(initial_target), initial_zoom_(initial_zoom), zoom_(initial_zoom), initial_rotation_(glm::mat4(1.0f)), rotation_(glm::mat4(1.0f)) {
    SetProjection(false);

    //TODO: Construct an initial rotation from position + target parameter.
    view_matrix_ = GetViewMatrix();
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

//Esesentially just returns a matrix representing the distance from the target to the camera.
//This is then rotated by the rotation matrix and translated by the target position 
//(In that order!) to give the final camera pos. Draw out the order of operations to make sense of it :)
glm::mat4 Camera::GetDistanceMatrix() {
    return {
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),  //Left //TODO: replace this with a stored left/up?
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),     //Up
        glm::vec4(glm::normalize(glm::vec3(0.0f, -zoom_, 0.0f)), 0.0f), //Forward
        glm::vec4(glm::vec3(0.0f, -zoom_, 0.0f), 1.0f) //Position
    };
}

 glm::mat4 Camera::GetViewMatrix() {
     if (matrix_needs_update) {
        camera_transform_ = glm::translate(target_) * (rotation_ * GetDistanceMatrix());
        view_matrix_ = glm::inverse(camera_transform_);
        matrix_needs_update = false;
    }

    return view_matrix_;
}