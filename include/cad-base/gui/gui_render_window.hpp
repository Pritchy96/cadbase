#ifndef GUI_RENDER_WINDOW_HPP
#define GUI_RENDER_WINDOW_HPP

#include "cad-base/viewport.hpp"
#include "cad-base/gui/gui_data.hpp"

#include <glm/fwd.hpp>
#include <memory>
#include <string>

#include <imgui.h>

class GuiRenderWindow {
        public:
            GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport, std::shared_ptr<GuiData> gui_data);
            // ~GuiRenderWindow();

            std::string name;
            GLFWwindow* glfw_window;
            bool is_alive = true;
            
            bool viewport_has_focus = false;
            bool clicked_on_image[5] = {false, false, false, false, false};

            std::shared_ptr<GuiData> gui_data;
            
            float arcball_rotate_sensitivity, arcball_pan_sensitivity;

            const float OPTIONS_DROPDOWN_OFFSET = 20.0f; 
            
            //TODO: replace with bool?
            int ortho_not_persp = 1;    //0 is ortho, 1 is perspective.

            void HandleIO();
            void Draw(double deltaTime);
            void DrawRenderWindowSettings(double deltaTime);
            glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);
            bool RayCubeIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, std::array<glm::vec3, 2> boxBounds);
        private:   
            std::shared_ptr<Viewport> viewport_;   
            ImVec2 window_size_;

            const float ARCBALL_ROTATE_SENSITIVITY_INITIAL = 4.0f; 
            const float ARCBALL_PAN_SENSITIVITY_INITIAL = 100.0f; 
    };

#endif