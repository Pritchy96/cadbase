    #ifndef CAMERA_HPP
    #define CAMERA_HPP

        #include <glm/fwd.hpp>
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
                glm::vec3 position, target, up;  
                glm::mat4 view_matrix, projection_matrix, default_view, rotation_matrix;

                Camera(glm::vec3 initial_position, glm::vec3 initial_target, glm::vec3 initial_up);
                ~Camera();

                glm::mat4 GetViewMatrix(); 
                glm::mat4 GetProjectionMatrix() const;

                void SetProjection(bool ortho_not_perspective_camera);
                bool GetProjection() const { return ortho_not_perspective_; }

                void  SetZoom(float new_zoom);
                float GetZoom();

                //Set the target to a unit vector from the camera in the requested direction 
                void SetDirection(glm::vec3 direction) {
                    target = position + glm::normalize(direction);   
                }

                //Get the direction of the target from the camera
                glm::vec3 GetDirection() const {
                    return target - position;
                }   
            private: 
                const float INITIAL_FOV = 90.0f, INITIAL_ASPECT_RATIO = 4.0f/3.0f, INITIAL_Z_NEAR = 0.0f, INITIAL_Z_FAR = 100.0f, 
                    INITIAL_ORTHO_LEFT = -10.0f, INITIAL_ORTHO_RIGHT = 10.0f, INITIAL_ORTHO_BOTTOM = -10.0f, INITIAL_ORTHO_TOP = 10.0f;
                float fov_ = INITIAL_FOV, aspect_ratio_ = INITIAL_ASPECT_RATIO, z_near_ = INITIAL_Z_NEAR, z_far_ = INITIAL_Z_FAR, 
                    ortho_left_ = INITIAL_ORTHO_LEFT, ortho_right_ = INITIAL_ORTHO_RIGHT, ortho_bottom_ = INITIAL_ORTHO_BOTTOM, ortho_top_ = INITIAL_ORTHO_TOP;

                bool ortho_not_perspective_;
        };

    #endif