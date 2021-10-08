#ifndef GUI_RENDER_WINDOW_HPP
#define GUI_RENDER_WINDOW_HPP

#include "cad-base/viewport.hpp"
#include "cad-base/gui/gui_base.hpp"

#include <glm/fwd.hpp>
#include <memory>
#include <string>

#include <imgui.h>

class GuiRenderWindow : GuiBase {
        public:
            GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport) ;
            // ~GuiRenderWindow();

            bool is_dragging = false;
            const float OPTIONS_DROPDOWN_OFFSET = 20; 
            
            //TODO: replace with bool?
            int ortho_not_persp = 1;    //0 is ortho, 1 is perspective.

            glm::vec2 mouse_pos_last = glm::vec2(220.0f, 220.0f);
            glm::vec2 mouse_pos = glm::vec2(240.0f, 240.0f); 
            bool ascending = true;

            void HandleUI();
            void Draw(double deltaTime) override;
            void DrawRenderWindowSettings(double deltaTime);
            glm::vec3 GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size);
        private:   
            std::shared_ptr<Viewport> viewport_;   
            ImVec2 window_size_;
    };

#endif