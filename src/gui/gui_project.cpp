#include "cad-base/gui/gui_project.hpp"
#include "cad-base/viewport.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>

using std::vector;
using std::shared_ptr;

GuiProject::GuiProject(std::string name, GLFWwindow* glfw_window, shared_ptr<vector<shared_ptr<Viewport>>> viewports) : viewports_(viewports), GuiBase(name, glfw_window) {
  
}

void GuiProject::Draw(double deltaTime) {
    is_alive = ImGui::Begin(name.c_str());

    ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::End();
}