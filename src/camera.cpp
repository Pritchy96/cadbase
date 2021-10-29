#include "cad-base/camera.hpp"
#include "spdlog/spdlog.h"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

using glm::vec3;
using glm::mat4;

Camera::Camera(vec3 initial_target, float initial_zoom, std::shared_ptr<glm::vec2> window_size) 
    :   initial_target_(initial_target), target_(initial_target), initial_zoom_(initial_zoom), zoom_(initial_zoom), initial_rotation_(glm::mat4(1.0f)), rotation_(glm::mat4(1.0f)) {

    window_size_ = window_size;
    SetProjectionStyle(false);

    //TODO: Construct an initial rotation from position + target parameter.
    view_matrix_ = GetViewMatrix();
}

void Camera::SetProjectionStyle(bool ortho_not_perspective_camera) {
    ortho_not_perspective_ = ortho_not_perspective_camera;
    if (ortho_not_perspective_) {
        float size_depth_ratio = std::atan(glm::radians(fov_ / 2.0f) * 2.0f);   //NOLINT: no magic numbers.
        auto distance = glm::distance(target_, glm::vec3(camera_transform_[3]));

        ortho_fustrum_width_ = (size_depth_ratio * distance * (window_size_->x/window_size_->y)) * 2;
        ortho_fustrum_height_ = (size_depth_ratio * distance) * 2.0f;

        projection_matrix_ = glm::ortho(-ortho_fustrum_width_/2, ortho_fustrum_width_/2, -ortho_fustrum_height_/2, ortho_fustrum_height_/2, z_near_, z_far_);
    } else {
        projection_matrix_ = glm::perspective(glm::radians(fov_), (window_size_->x/window_size_->y), z_near_, z_far_);
    }
}

//Esesentially just returns a matrix representing the distance from the target to the camera.
//This is then rotated by the rotation matrix and translated by the target position 
//(In that order!) to give the final camera pos. Draw out the order of operations to make sense of it :)
glm::mat4 Camera::GetDistanceMatrix() {
    return {
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),  //Left //TODO: replace this with a stored left/up/forward?
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),     //Up
        glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), //Forward
        glm::vec4(glm::vec3(0.0f, -zoom_, 0.0f), 1.0f) //Position
    };
}

 glm::mat4 Camera::GetViewMatrix() {
     if (view_matrix_needs_update_) {
        camera_transform_ = glm::translate(target_) * (rotation_ * GetDistanceMatrix());
        view_matrix_ = glm::inverse(camera_transform_);

        //Ortho matrix needs updating when camera zoom changes.
        if (ortho_not_perspective_) {
            SetProjectionStyle(ortho_not_perspective_);
        }

        view_matrix_needs_update_ = false;
    }

    return view_matrix_;
}