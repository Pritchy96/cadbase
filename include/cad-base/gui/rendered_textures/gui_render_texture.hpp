#ifndef GUIRENDERTEXTURE_HPP
#define GUIRENDERTEXTURE_HPP

    #include "cad-base/gui/gui_data.hpp"
#include "cad-base/raycast/ray.hpp"
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

    class NaviCube; //Forward declaration of NaviCube.

    class GuiRenderTexture {
        public:
                GuiRenderTexture(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height, std::shared_ptr<GuiData> gui_data);
            ~GuiRenderTexture() = default;

            void Update();  
            virtual void Draw();

            glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);
            bool RayCubeIntersection(Ray ray, std::array<glm::vec3, 2> boxBounds);
            void HandleIO();
            void SetupFBO();

            std::shared_ptr<glm::vec2> window_size;
            std::shared_ptr<GuiData> gui_data;

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
            
            bool CheckFramebufferStatus(GLuint fbo);

            const float ARCBALL_ROTATE_SENSITIVITY_INITIAL = 4.0f; 
            const float ARCBALL_PAN_SENSITIVITY_INITIAL = 100.0f; 
    };

#endif
