#ifndef GUI_PROJECT_HPP
#define GUI_PROJECT_HPP

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cad-base/gui/rendered_textures/viewport.hpp"

class GuiProject {
    public:
        GuiProject(std::string name, GLFWwindow* glfw_window);

        void Draw();

        std::string name;
        GLFWwindow* glfw_window;
    private:   
  
};

#endif