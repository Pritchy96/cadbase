#ifndef NAVICUBE_HPP
#define NAVICUBE_HPP

    #include <glm/fwd.hpp>
    #include <iostream>
    #include <cstdio>
    #include <cstdlib>
    #include <vector>
    #include <memory>
    #include <map>

    #define GLM_ENABLE_EXPERIMENTAL
    #include <GL/glew.h>
    #include <GL/glxmd.h>
    #include <GLFW/glfw3.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include <glm/gtx/transform.hpp>
    #include <glm/gtx/string_cast.hpp>

    #include "cad-base/shader.hpp"
    #include "cad-base/geometry/geometry.hpp"
    #include "cad-base/renderable.hpp"
    #include "cad-base/camera.hpp"

    class NaviCube {
        public:
            NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height);
            ~NaviCube() = default;

            void Update(double deltaT);  
        
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> navicube_geo_renderable_pairs;
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> debug_geo_renderable_pairs;

            GLuint t_shader;      
            GLuint shader_id;    
            GLuint basic_shader;
            GLint render_face = GL_FRONT, render_mode = GL_FILL;

            GLuint fbo;
            GLuint colour_texture;
            GLuint depth_texture;

            std::shared_ptr<glm::vec2> window_size;

            glm::vec4 background_colour;    //TODO: can probably hardcode this as clear (alpha channel = 0.0f)

            double time_elapsed = 0;
            int frames_elapsed = 0;

            Camera* camera;
        private:

            GLFWwindow *glfw_window_;

            constexpr const static float CUBE_SIZE = 100;

            //1 & 2, 3 & 4, 5 & 6 are opposite pairs.
            std::vector<glm::vec3> face_1_ = {
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
            }, 
            face_2_ = {
                glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
            },
            face_3_ = {
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
            }, 
            face_4_ = {
                glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
            },
            face_5_ = {
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
                glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
                glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
                glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
            }, 
            face_6_ = {
                glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
                glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
                glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE)  
            };
            
            void SetupFBO();
            bool CheckFramebufferStatus(GLuint fbo);
    };

#endif
