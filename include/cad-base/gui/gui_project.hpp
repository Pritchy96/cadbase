#ifndef GUI_PROJECT_HPP
#define GUI_PROJECT_HPP

#include "cad-base/gui/rendered_textures/viewport.hpp"

#include <memory>
#include <string>

#include <GLFW/glfw3.h>


class GuiProject {
        public:
            GuiProject(std::string name, GLFWwindow* glfw_window);
            // ~GuiProject();

            void Draw();

            std::string name;
            GLFWwindow* glfw_window;
        private:   
        
        };

#endif