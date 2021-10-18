#ifndef GUI_MAIN_HPP
#define GUI_MAIN_HPP

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

#include "cad-base/gui/gui_project.hpp"
#include "cad-base/gui/gui_render_window.hpp"


class GuiMain {
        public:
            explicit GuiMain(GLFWwindow* glfw_window);
            // ~GuiBase();

            bool SetupImgui(); 
            void RenderGuiMainMenu();
            void SetupGuiTheme();
            void SetLayout();

            void Update(double deltaTime);

            float gui_scale_factor = GUI_INITIAL_SCALE_FACTOR;
            GLFWwindow* glfw_window;

            std::vector<std::shared_ptr<GuiRenderWindow>> gui_render_windows;

            const ImVec4 BACKGROUND_COLOUR = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);


        private:
            // Pass through input.
            ImGuiDockNodeFlags dockspace_flags_ = ImGuiDockNodeFlags_PassthruCentralNode;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each other.
            ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            ImGuiViewport* main_imgui_viewport_;
            ImGuiIO* imgui_io_;

            //GUI
            std::unique_ptr<GuiProject> gui_settings_;
            bool show_demo_window_ = false;

            constexpr static const float GUI_INITIAL_SCALE_FACTOR = 2.0f;

            bool first_time_ = true;
        };

#endif