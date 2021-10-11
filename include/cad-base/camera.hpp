#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "imgui.h"
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

    struct Camera {
        public:

            Camera(glm::vec3 initial_position, glm::vec3 initial_target, glm::vec3 initial_up, float initial_distance);
            ~Camera();

            glm::mat4 projection_matrix;

            void SetProjection(bool ortho_not_perspective_camera);
            bool GetProjection() const { return ortho_not_perspective_; }

            void  SetZoom(float new_zoom);
            float GetZoom();

            //TODO: have setters on all the components which call getviewmatrix, as opposed to updating it if it's not needed.
            glm::mat4 GetViewMatrix();

            // glm::mat4 DecomposeMatrix(glm::mat4 matrix) {
            // }

            glm::mat4 rotation;

            float distance;


            // Set the target to a unit vector from the camera in the requested direction 
            void SetDirection(glm::vec3 direction) {
                initial_target_ = initial_position_ + glm::normalize(direction);   
            }

            // Get the direction of the target from the camera
            glm::vec3 GetDirection() const {
                return initial_target_ - initial_position_;
            }   
        private: 

            glm::vec3 initial_position_, initial_target_, initial_up_;
            glm::vec3 target_;


            const float INITIAL_FOV = 70.0f, INITIAL_ASPECT_RATIO = 4.0f/3.0f, INITIAL_Z_NEAR = 0.0f, INITIAL_Z_FAR = 10000.0f, 
                INITIAL_ORTHO_LEFT = -300.0f, INITIAL_ORTHO_RIGHT = 300.0f, INITIAL_ORTHO_BOTTOM = -300.0f, INITIAL_ORTHO_TOP = 300.0f;
            float fov_ = INITIAL_FOV, aspect_ratio_ = INITIAL_ASPECT_RATIO, z_near_ = INITIAL_Z_NEAR, z_far_ = INITIAL_Z_FAR, 
                ortho_left_ = INITIAL_ORTHO_LEFT, ortho_right_ = INITIAL_ORTHO_RIGHT, ortho_bottom_ = INITIAL_ORTHO_BOTTOM, ortho_top_ = INITIAL_ORTHO_TOP;

            bool ortho_not_perspective_;

            glm::mat4 view_matrix_;
    };

#endif