#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <string>
#include <algorithm>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/trigonometric.hpp>


#include "cad-base/gui/gui_render_window.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"
#include "cad-base/raycast/ray.hpp"
#include "cad-base/gui/rendered_textures/navicube.hpp"

GuiRenderWindow::GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport) 
                                    : name(name), glfw_window(glfw_window), viewport_(viewport) {
    // navicube_ = std::make_shared<NaviCube>(glfw_window, viewport_->background_colour, 200, 200);
}

void GuiRenderWindow::DrawRenderWindowSettings() {
    //Set options dropdown position
    ImVec2 settings_button_pos = ImVec2(OPTIONS_DROPDOWN_OFFSET, OPTIONS_DROPDOWN_OFFSET);
    ImGui::SetCursorPos(settings_button_pos);

    if (ImGui::Button("Options")) {
        ImVec2 settings_popup_pos = settings_button_pos;
        
        //Align settings popup under button
        //No arithmatic operaters for ImVecs
        settings_popup_pos.x += ImGui::GetItemRectSize().x + ImGui::GetWindowPos().x - ImGui::GetItemRectSize().x;
        settings_popup_pos.y += ImGui::GetItemRectSize().y + ImGui::GetWindowPos().y;
        
        ImGui::SetNextWindowPos(settings_popup_pos);
        ImGui::OpenPopup("gui_render_window_settings");
    }

    if (ImGui::BeginPopup("gui_render_window_settings")) {
        ImGui::Text("Render Style:");

        if (ImGui::RadioButton("Orthogonal", &ortho_not_persp, 0)) {
            viewport_->camera->SetProjectionStyle(true);
        }
        
        ImGui::SameLine();
        
        if (ImGui::RadioButton("Perspective", &ortho_not_persp, 1)) {
            viewport_->camera->SetProjectionStyle(false);
        }

        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &viewport_->grid->draw_geometry);
        ImGui::Checkbox("Show Render Axis", &viewport_->render_axis->draw_geometry);
        //TODO: Implement proper "Render all geo's bounding boxes in this viewport" option
        // ImGui::Checkbox("Show Bounding Boxes", &viewport_->geo_renderable_pairs.back().second->draw_aa_bounding_box);
        
        ImGui::Separator();
        if (ImGui::Button("Reset Pan")) {
            viewport_->camera->ResetTarget();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Zoom")) {
            viewport_->camera->ResetZoom();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Rotation")) {
            viewport_->camera->ResetRotation();
        }

        ImGui::Separator();
        if (ImGui::Button("Reset Entire View")) {
            viewport_->camera->ResetRotation();
            viewport_->camera->ResetTarget();
            viewport_->camera->ResetZoom();
        }

        ImGui::EndPopup();
    }
}


void GuiRenderWindow::Draw() {
    is_alive = ImGui::Begin(name.c_str());
    // Using a Child allow to fill all the space of the window.
    ImGui::BeginChild("Render Window Child");

    // Get the size of the child (i.e. the whole draw size of the windows).
    window_size_ = ImGui::GetWindowSize();

    if (viewport_->window_size->x != window_size_.x || viewport_->window_size->y != window_size_.y) {
        viewport_->window_size->x = window_size_.x;
        viewport_->window_size->y = window_size_.y;
        viewport_->SetupFBO();  //Need to reset the texture sizes.
    }

    // Because we use the texture from OpenGL, we need to invert the V from the UV.
    ImGui::Image((ImTextureID)viewport_->colour_texture, window_size_, ImVec2(0, 1), ImVec2(1, 0));
    viewport_->HandleIO();

    // navicube->Update();
    // ImGui::SetCursorPos(ImVec2(window_size_.x - 220, 20));
    // ImGui::Image((ImTextureID)viewport_->navicube->colour_texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));
    // HandleNaviCubeIO();

    DrawRenderWindowSettings();

    ImGui::EndChild();
    ImGui::End();
}