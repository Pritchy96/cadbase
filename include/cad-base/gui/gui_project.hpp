#ifndef GUI_PROJECT_HPP
#define GUI_PROJECT_HPP

#include "cad-base/viewport.hpp"
#include "cad-base/gui/gui_base.hpp"

#include <memory>
#include <string>

#include <GLFW/glfw3.h>


class GuiProject : GuiBase {
        public:
            GuiProject(std::string name, GLFWwindow* glfw_window);
            // ~GuiProject();

            void Draw(double deltaTime) override;
        private:   
        
        };

#endif