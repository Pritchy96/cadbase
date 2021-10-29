#ifndef GUI_RENDER_WINDOW_HPP
#define GUI_RENDER_WINDOW_HPP

#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <imgui.h>

#include "cad-base/raycast/ray.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"
#include "cad-base/gui/rendered_textures/navicube.hpp"
#include "cad-base/gui/gui_data.hpp"

class GuiRenderWindow {
    public:
        GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport);
        
        std::string name;
        GLFWwindow* glfw_window;
        bool is_alive = true;

        const float OPTIONS_DROPDOWN_OFFSET = 20.0f; 
        
        //TODO: replace with bool?
        int ortho_not_persp = 1;    //0 is ortho, 1 is perspective.

        void Draw();
        void DrawRenderWindowSettings();
    private:   
        std::shared_ptr<Viewport> viewport_;
        std::shared_ptr<NaviCube> navicube_;
        ImVec2 window_size_;
};

#endif