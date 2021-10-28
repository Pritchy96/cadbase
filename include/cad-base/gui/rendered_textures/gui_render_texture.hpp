#ifndef GUIRENDERTEXTURE_HPP
#define GUIRENDERTEXTURE_HPP

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

    class GuiRenderTexture {
        public:
            GuiRenderTexture(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height);
            ~GuiRenderTexture() = default;

            void Update();  
            virtual void Draw();

            std::shared_ptr<glm::vec2> window_size;

            Camera* camera;
            glm::vec4 background_colour;
            GLuint shader_id;    
            GLuint basic_shader;
            GLint render_face = GL_FRONT, render_mode = GL_FILL;

            GLuint fbo;
            GLuint colour_texture;
            GLuint depth_texture;
        
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> geo_renderable_pairs;
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> debug_geo_renderable_pairs;

            bool clicked_on_texture[5] = {false, false, false, false, false};
            bool texture_has_focus = false;
            float arcball_rotate_sensitivity, arcball_pan_sensitivity;

        private:
            GLFWwindow *glfw_window_;
            
            void SetupFBO();
            bool CheckFramebufferStatus(GLuint fbo);
    };

#endif
