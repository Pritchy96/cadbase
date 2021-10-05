#include "cad-base/gui/gui_render_window.hpp"
#include "imgui_internal.h"

#include <memory>
#include <string>

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GuiRenderWindow::GuiRenderWindow(std::string name, std::shared_ptr<Viewport> viewport) : name(name), viewport_(viewport) {
  
}

void GuiRenderWindow::Draw(double deltaTime) {
    is_alive = ImGui::Begin(name.c_str());
    // Using a Child allow to fill all the space of the window.
    ImGui::BeginChild("Render Window 0 Child");

    // Get the size of the child (i.e. the whole draw size of the windows).
    ImVec2 wsize = ImGui::GetWindowSize();

    // Because we use the texture from OpenGL, we need to invert the V from the UV.
    ImGui::Image((ImTextureID)viewport_->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
    ImGui::End();
}