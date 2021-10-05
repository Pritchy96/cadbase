#include "cad-base/gui/gui_settings.hpp"
#include "cad-base/viewport.hpp"

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>

using std::vector;
using std::shared_ptr;

GuiSettings::GuiSettings(std::string name, shared_ptr<vector<shared_ptr<Viewport>>> viewports) : name(name), viewports_(viewports) {
  
}

void GuiSettings::Draw(double deltaTime) {
    // // Using a Child allow to fill all the space of the window.
    // ImGui::BeginChild("Render Window 0 Child");
    // // Get the size of the child (i.e. the whole draw size of the windows).
    // ImVec2 wsize = ImGui::GetWindowSize();

    // // Because we use the texture from OpenGL, we need to invert the V from the UV.
    // ImGui::Image((ImTextureID)viewport_->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
    // ImGui::EndChild();
    // ImGui::End();

        is_alive = ImGui::Begin(name.c_str());

        ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();

        if (ImGui::Checkbox("Ortho Render", &orthogonol_rendering)) {
            for (const auto& v : (*viewports_)) {
                v->camera->SetProjection(orthogonol_rendering);
            }
        }
        ImGui::Separator();

        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Separator();

        const char* render_types[] = { "Orthogonal", "Perspective"};

        if (ImGui::Button("Options")) {
            ImGui::OpenPopup("gui_render_window_settings");
        }

        if (ImGui::BeginPopup("gui_render_window_settings")) {
            ImGui::Text("Render Style");
            ImGui::Separator();

        for (int i = 0; i < IM_ARRAYSIZE(render_types); i++) {
            if (ImGui::MenuItem(render_types[i], "", i == selected_render_type)) {
                selected_render_type = i;
            }
        }
        ImGui::Separator();
        ImGui::Separator();
        
        ImGui::EndPopup();
    }
        ImGui::End();
}