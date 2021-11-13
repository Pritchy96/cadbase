#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>

#include "cad-base/gui/gui_project.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"

using std::vector;
using std::shared_ptr;

GuiProject::GuiProject(std::string name, GLFWwindow* glfw_window) : name(name), glfw_window(glfw_window) {
  
}

void GuiProject::Draw() {
    ImGui::Begin(name.c_str());
    ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::End();
}