#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

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
    #include "cad-base/geometry/viewport_grid.hpp"

    class Viewport: public std::enable_shared_from_this<Viewport> {
        public:
            Viewport(GLFWwindow *window, glm::vec3 background_col, int window_width, int window_height);
            ~Viewport() = default;

            std::shared_ptr<Viewport> GetSharedPtr() {
                return shared_from_this();
            }

            void Update(double deltaT);  
            
            void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
            void CursorCallback( GLFWwindow* window, double x, double y );
            void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
            void WindowSizeCallback(GLFWwindow* glfw_window, int width, int height);

            // TODO: most of these can be made private.
            GLFWwindow *glfw_window;
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> master_geo_renderable_pairs;
            //Viewport specific stuff that doesn't need i.e selecting etc.
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> viewport_geo_renderable_pairs;
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> debug_geo_renderable_pairs;

            GLuint t_shader;      
            GLuint shader_id;    
            GLuint basic_shader;
            GLint render_face = GL_FRONT, render_mode = GL_FILL;

            GLuint fbo;
            GLuint colour_texture;
            GLuint depth_texture;

            const int WIDTH = 1000, HEIGHT = 1000;

            glm::vec3 background_colour;

            std::shared_ptr<Geometry> render_axis;
            std::shared_ptr<ViewportGrid> grid;

            double time_elapsed = 0;
            int frames_elapsed = 0;

            Camera* camera;
        private:
            std::shared_ptr<glm::vec2> window_size_; 
            
            void SetupFBO(); 
            void SetupTransformShader(GLuint transformShader);  
            bool CheckFramebufferStatus(GLuint fbo);
    };

#endif
