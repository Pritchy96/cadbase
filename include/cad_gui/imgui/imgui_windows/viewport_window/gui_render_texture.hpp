#ifndef GUI_RENDER_TEXTURE_HPP
#define GUI_RENDER_TEXTURE_HPP

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

#include "cad_gui/imgui/gui_data.hpp"
#include "cad_gui/opengl/raycast/ray.hpp"
#include "cad_gui/opengl/renderables/renderable.hpp"
#include "cad_gui/opengl/camera.hpp"
#include "cad_gui/opengl/arcball.hpp"

#include "cad_data/feature.hpp"

namespace cad_gui {
    class GuiRenderTexture {
        public:
            GuiRenderTexture(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height);
            ~GuiRenderTexture() = default;

            void Update();  
            virtual void Draw();

            void SetupFBO();

            virtual void HandleIO();

            glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);

            void CastRay(glm::vec2 mouse_pos);
            void DrawDebugRay(Ray ray, glm::vec4 ray_colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            bool RayCubeIntersection(Ray ray, std::array<glm::vec3, 2> boxBounds);
            virtual void SelectRenderable(std::shared_ptr<Renderable> closest_renderable) = 0;
            virtual void SelectNothing() = 0;   //A click not on an object.

            std::shared_ptr<glm::vec2> window_size;
            std::shared_ptr<Camera> camera;
            std::shared_ptr<Arcball> arcball;

            glm::vec4 background_colour;
            GLint render_face = GL_FRONT, render_mode = GL_FILL;

            GLuint fbo;
            GLuint colour_texture;
            GLuint depth_texture;
        
            std::vector<std::pair<std::shared_ptr<cad_data::Feature>, std::shared_ptr<Renderable>>> feature_renderable_pairs;
            std::vector<std::pair<std::shared_ptr<cad_data::Feature>, std::shared_ptr<Renderable>>> debug_feature_renderable_pairs;

            bool clicked_on_texture[5] = {false, false, false, false, false};
            bool texture_has_focus = false;

        private:            
            bool CheckFramebufferStatus(GLuint fbo);

            GLFWwindow *glfw_window_;
    };
}
#endif
