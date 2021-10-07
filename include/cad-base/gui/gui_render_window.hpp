#ifndef GUI_RENDER_WINDOW_HPP
#define GUI_RENDER_WINDOW_HPP

#include <memory>
#include <string>

#include "cad-base/viewport.hpp"

#include <imgui.h>

struct GuiRenderWindow {
        public:
            explicit GuiRenderWindow(std::string name, std::shared_ptr<Viewport> viewport);
            // ~GuiRenderWindow();

            std::string name;
            bool is_alive = true;
            const float OPTIONS_DROPDOWN_OFFSET = 20; 
            
            //TODO: replace with bool?
            int ortho_not_persp = 1;    //0 is ortho, 1 is perspective.

            void Draw(double deltaTime);
            void DrawRenderWindowSettings(double deltaTime);
        private:   
            std::shared_ptr<Viewport> viewport_;   
            ImVec2 window_size_;
    };

#endif