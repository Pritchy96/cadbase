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
    
    ImGui::BeginChild("Render Window Settings Child");

        // Because we use the texture from OpenGL, we need to invert the V from the UV.
        ImGui::Image((ImTextureID)viewport_->texture, wsize, ImVec2(0, 1), ImVec2(1, 0));

        //Set options dropdown position
        ImGui::SetCursorPos(ImVec2(wsize.x - (ImGui::CalcTextSize("Options").x + ImGui::GetStyle().WindowPadding.x + OPTIONS_DROPDOWN_OFFSET), OPTIONS_DROPDOWN_OFFSET));

        if (ImGui::Button("Options")) {
            ImGui::OpenPopup("gui_render_window_settings");
        }

        if (ImGui::BeginPopup("gui_render_window_settings")) {
            ImGui::Text("Render Style:");

            if (ImGui::RadioButton("Orthogonal", &ortho_not_persp, 0)) {
                viewport_->camera->SetProjection(true);
            }

            ImGui::SameLine();
            
            if (ImGui::RadioButton("Perspective", &ortho_not_persp, 1)) {
                viewport_->camera->SetProjection(false);
            }

            ImGui::Separator();
            ImGui::Checkbox("Show Grid", &viewport_->grid->visible);
            ImGui::Checkbox("Show Render Axis", &viewport_->render_axis->visible);

            ImGui::EndPopup();
        }


    ImGui::EndChild();
    ImGui::EndChild();

    ImGui::End();
}