#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <imgui.h>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>

    class Camera {
        public:

            Camera(glm::vec3 initial_target, float initial_zoom);
            ~Camera();

            glm::mat4 projection_matrix;

            void SetProjection(bool ortho_not_perspective_camera);
            bool GetProjection() const { return ortho_not_perspective_; }


            glm::mat4 GetDistanceMatrix();

            glm::mat4 GetViewMatrix();

            bool matrix_needs_update = true;

            void SetRotation(glm::mat4 rotation) {
                rotation_ = rotation;
                matrix_needs_update = true;
            }

            glm::mat4 GetRotation() {
                return rotation_;
            }

            void ResetRotation() {
                rotation_ = initial_rotation_;
                matrix_needs_update = true;
            }  

            void SetTarget(glm::vec3 target) {
                target_ = target;
                matrix_needs_update = true;
            }

            glm::vec3 GetTarget() {
                return target_;
            }

            void ResetTarget() {
                target_ = initial_target_;
                matrix_needs_update = true;
            }

            void SetZoom(float zoom) {
                zoom_ = zoom;
                matrix_needs_update = true;
            }

            float GetZoom() const {
                return zoom_;
            }

            void ResetZoom() {
                zoom_ = initial_zoom_;
                matrix_needs_update = true;
            }            

            glm::mat4 GetCameraTransform() {
                return camera_transform_;
            }
    

        private: 
            bool ortho_not_perspective_;

            glm::vec3 initial_target_, target_;
            float initial_zoom_, zoom_;

            glm::mat4 initial_rotation_, rotation_;

            glm::mat4 view_matrix_;
            glm::mat4 camera_transform_;    //inverse of the View matrix, stored here so we don't need to recalculate it.

            const float INITIAL_FOV = 70.0f, INITIAL_ASPECT_RATIO = 4.0f/3.0f, INITIAL_Z_NEAR = 0.0f, INITIAL_Z_FAR = 10000.0f, 
                INITIAL_ORTHO_LEFT = -300.0f, INITIAL_ORTHO_RIGHT = 300.0f, INITIAL_ORTHO_BOTTOM = -300.0f, INITIAL_ORTHO_TOP = 300.0f;
            float fov_ = INITIAL_FOV, aspect_ratio_ = INITIAL_ASPECT_RATIO, z_near_ = INITIAL_Z_NEAR, z_far_ = INITIAL_Z_FAR, 
                ortho_left_ = INITIAL_ORTHO_LEFT, ortho_right_ = INITIAL_ORTHO_RIGHT, ortho_bottom_ = INITIAL_ORTHO_BOTTOM, ortho_top_ = INITIAL_ORTHO_TOP;
    };
    
#endif