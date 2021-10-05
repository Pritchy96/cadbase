#ifndef GUI_RENDER_WINDOW_HPP
#define GUI_RENDER_WINDOW_HPP

#include <memory>
#include <string>

#include "cad-base/viewport.hpp"

struct GuiRenderWindow {
        public:
            explicit GuiRenderWindow(std::string name, std::shared_ptr<Viewport> viewport);
            // ~GuiRenderWindow();

            std::string name;
            bool is_alive = true;

            void Draw(double deltaTime);
        private:   
            std::shared_ptr<Viewport> viewport_;   
    };

#endif