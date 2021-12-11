#ifndef PROJECT_WINDOW_HPP
#define PROJECT_WINDOW_HPP

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cad_gui/scene_data.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/rendered_textures/viewport.hpp"

class ProjectWindow {
    public:
        ProjectWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<SceneData> scene_data);

        void Draw();

        std::string name;
        GLFWwindow* glfw_window;
        std::shared_ptr<SceneData> scene_data;
        
    private:   
  
};

#endif