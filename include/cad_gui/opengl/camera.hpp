#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cmath>
#include <glm/common.hpp>
#include <imgui.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <memory>

class Camera {
    public:
        Camera(glm::vec3 initial_target, float initial_zoom, std::shared_ptr<glm::vec2> window_size);

        void SetProjectionStyle(bool ortho_not_perspective_camera);
        bool IsOrthoCamera() const { return ortho_not_perspective_; }

        glm::mat4 GetDistanceMatrix();
        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix() { return projection_matrix_; }

        //Spherical Linerarly intERPolate camera rotation.
        void SLERPCameraRotation(glm::quat new_rotation, float time_duration);
        void UpdateSLERP();

        void SetRotation(glm::mat4 rotation) {
            rotation_ = rotation;
            view_matrix_needs_update_ = true;
        }

        glm::mat4 GetRotation() {
            return rotation_;
        }

        void ResetRotation() {
            rotation_ = initial_rotation_;
            view_matrix_needs_update_ = true;
        }  

        void SetTarget(glm::vec3 target) {
            target_ = target;
            view_matrix_needs_update_ = true;
        }

        void MoveTarget(glm::vec3 delta) {
            target_ += delta;
            view_matrix_needs_update_ = true;
        }

        glm::vec3 GetTarget() {
            return target_;
        }

        void ResetTarget() {
            target_ = initial_target_;
            view_matrix_needs_update_ = true;
        }

        void SetZoom(float zoom) {
            zoom_ = std::fmax(zoom, 1.0f);
            view_matrix_needs_update_ = true;
        }

        float GetZoom() const {
            return zoom_;
        }

        void ResetZoom() {
            zoom_ = initial_zoom_;
            view_matrix_needs_update_ = true;
        }            

        glm::mat4 GetCameraTransform() {
            return camera_transform_;
        }

        float GetOrthoFustrumWidth() {
            return ortho_fustrum_width_;
        }

        float GetOrthoFustrumHeight() {
            return ortho_fustrum_height_;
        }

        // Camera Settings
        bool can_pan = true;
        bool can_rotate = true;
        bool can_zoom = true;

        // SLERP variables
        float slerp_time_total;
        float slerp_time_elapsed;
        glm::quat slerp_initial_rotation;
        glm::quat slerp_target_rotation;
        bool is_slerping = false;

        // A bit of a special case - certain operations will fix the camera such that a face normal of the scene and the camera direction
        // Are perpendicular. In that case, we may want to do some special operations, that this bool will enable.
        // For example, 90 degree rotation buttons on the navicube when aligned with one of the navicube faces.
        bool aligned_to_face = false;

        constexpr const static float STANDARD_SLERP_TIME = 0.2f;

    private: 
        bool ortho_not_perspective_;

        glm::vec3 initial_target_, target_;
        float initial_zoom_, zoom_;

        glm::mat4 initial_rotation_, rotation_;

        bool view_matrix_needs_update_ = true;

        glm::mat4 view_matrix_;
        glm::mat4 camera_transform_;    //inverse of the View matrix, stored here so we don't need to recalculate it.

        glm::mat4 projection_matrix_;

        float ortho_fustrum_width_, ortho_fustrum_height_;

        std::shared_ptr<glm::vec2> window_size_;

        const float INITIAL_FOV = 90.0f, INITIAL_Z_NEAR = 1.0f, INITIAL_Z_FAR = 10000.0f;
        float fov_ = INITIAL_FOV, z_near_ = INITIAL_Z_NEAR, z_far_ = INITIAL_Z_FAR;  
    };
#endif