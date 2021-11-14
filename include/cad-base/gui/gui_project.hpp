#ifndef GUI_PROJECT_HPP
#define GUI_PROJECT_HPP

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cad-base/scene_data.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"

class GuiProject {
    public:
        GuiProject(std::string name, GLFWwindow* glfw_window, std::shared_ptr<SceneData> scene_data);

        void Draw();

        std::string name;
        GLFWwindow* glfw_window;
        std::shared_ptr<SceneData> scene_data;
        
    private:   
  
};

#endif