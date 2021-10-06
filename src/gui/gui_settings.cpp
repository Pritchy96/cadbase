#include "cad-base/gui/gui_settings.hpp"
#include "cad-base/viewport.hpp"

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>

using std::vector;
using std::shared_ptr;

//TODO: GuiSettings shouldn't autohide its tab bar.

GuiSettings::GuiSettings(std::string name, shared_ptr<vector<shared_ptr<Viewport>>> viewports) : name(name), viewports_(viewports) {
  
}

void GuiSettings::Draw(double deltaTime) {
    is_alive = ImGui::Begin(name.c_str());

    ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::End();
}