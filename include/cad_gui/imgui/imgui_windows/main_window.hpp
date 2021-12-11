#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <memory>
#include <string>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui_internal.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cad_gui/imgui/imgui_windows/project_window.hpp"
#include "cad_gui/imgui/gui_data.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport_window.hpp"
#include "cad_gui/imgui/imgui_windows/log_window.hpp"
#include "cad_gui/imgui/app_style.hpp"
#include "cad_gui/scene_data.hpp"

class MainWindow {
    public:
        explicit MainWindow(GLFWwindow* glfw_window, std::shared_ptr<LogWindow> gui_logger_sink_, std::shared_ptr<SceneData> scene_data);
        // ~GuiBase();

        bool SetupImgui(); 
        void RenderGuiMainMenu();
        void SetupGuiTheme();
        void SetLayout();

        void Update();

        float gui_scale_factor = GUI_INITIAL_SCALE_FACTOR;
        GLFWwindow* glfw_window;

        std::vector<std::shared_ptr<ViewportWindow>> gui_render_windows;

    private:
        // Pass through input.
        ImGuiDockNodeFlags dockspace_flags_ = ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each other.
        ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport* main_imgui_viewport_;
        ImGuiIO* imgui_io_;
        AppStyle app_style_;

        //GUI
        std::unique_ptr<ProjectWindow> gui_project_;
        bool show_demo_window_ = false;

        std::shared_ptr<LogWindow> gui_logger_sink_; //Need a reference to this so we can draw it.

        constexpr static const float GUI_INITIAL_SCALE_FACTOR = 2.0f;

        bool first_time_ = true;
};
#endif