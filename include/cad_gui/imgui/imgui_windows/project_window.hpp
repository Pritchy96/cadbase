#ifndef PROJECT_WINDOW_HPP
#define PROJECT_WINDOW_HPP

#include <memory>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cad_data/scene_data.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"
namespace cad_gui {
    class ProjectWindow {
        public:
            ProjectWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<cad_data::SceneData> scene_data);

            void Draw();

            std::string name;
            GLFWwindow* glfw_window;
            std::shared_ptr<cad_data::SceneData> scene_data;
            
        private:   
    
    };
}

#endif