#include "cad-base/gui/gui_render_window.hpp"
#include "imgui_internal.h"

#include <cmath>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/trigonometric.hpp>
#include <memory>
#include <string>
#include <algorithm>

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GuiRenderWindow::GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport) : viewport_(viewport), GuiBase(name, glfw_window){

}

void GuiRenderWindow::DrawRenderWindowSettings(double deltaTime) {
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
}

void GuiRenderWindow::HandleUI() {
    ImGuiIO& io = ImGui::GetIO();
    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.y, io.MouseDelta.x);
    
    ImVec2 monitor_size = ImGui::GetViewportPlatformMonitor(ImGui::GetMainViewport())->MainSize;
    ImVec2 screen_size = ImVec2(500, 500);
    
    // glm::vec2 mouse_pos_last = glm::vec2(io.MousePosPrev.x, io.MousePosPrev.y);
    // glm::vec2 mouse_pos =    glm::vec2(io.MousePos.x, io.MousePos.y);

    if (ascending) {
        if (mouse_pos.x + 490.0f > screen_size.x) {
            ascending = false;
        } else {
            mouse_pos += glm::vec2(20.0f, 20.0f);
            mouse_pos_last += glm::vec2(20.0f, 20.0f);
        }
    } 
    // else {
    //     if (mouse_pos.x - 20.0f < 0.0f) {
    //         ascending = true;
    //     } else {
    //         mouse_pos -= glm::vec2(20.0f, 0);
    //         mouse_pos_last -= glm::vec2(20.0f, 0);
    //     }
    // }

    std::cout << "mouse_pos_last: " << glm::to_string(mouse_pos_last) << std::endl;
    std::cout << "mouse_pos: " <<  glm::to_string(mouse_pos) << std::endl;

    bool image_hovered = ImGui::IsItemHovered();

    // int window_origin_x, window_origin_y;
    // glfwGetWindowPos(glfw_window, &window_origin_x, &window_origin_y);

    // if (mouse_pos_last != float)

    // std::cout << "Mouse pos: " << glm::to_string(glm::vec2(io.MousePos.x, io.MousePos.y)) << std::endl;
    // std::cout << "Window pos: " << glm::to_string(glm::vec2(window_origin_x, window_origin_y)) << std::endl;
    // std::cout << "Monitor Size: x: " << monitor_size.x << ", y: " << monitor_size.y << std::endl;
    // std::cout << "mouse delta `(pixels): " << glm::to_string(mouse_delta) << std::endl;

    glm::vec2 mouse_delta_ratio = glm::vec2(mouse_delta.x / monitor_size.x, mouse_delta.y / monitor_size.y);

    //std::cout << "mouse delta (0-1): " << glm::to_string(mouse_delta_ratio) << std::endl;
    
    Camera* camera = viewport_->camera;

    //Allow for mouse dragging outside of the render window once clicked & held.
    // if(image_hovered || is_dragging) {
        // if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            is_dragging = true;

                glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(screen_size.x, screen_size.y));
                glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(screen_size.x, screen_size.y));

                //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
                glm::vec3 cross_vector = glm::cross(last_pos_vec, pos_vec);

                //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
                float angle = acos(glm::dot(last_pos_vec, pos_vec));

                //Create normalised rotation quaternion from the axis and the angle.
                glm::quat rotation_quat = glm::angleAxis(angle, cross_vector);
                glm::normalize(rotation_quat);

                glm::mat4 rotation_matrix = glm::toMat4(rotation_quat);
                camera->view_matrix *= rotation_matrix;
    //     }
    // }

    if(ImGui::IsMouseReleased(0)) {
        is_dragging = false;
    }
}

glm::vec3 GuiRenderWindow::GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size) {
    //Convert mouse pos to homogenous coordinates (-1 to 1)
    glm::vec3 vector = glm::vec3(((screen_pos.x/(screen_size.x) * 2.0f) - 1.0f), ((screen_pos.y/(screen_size.y) * 2.0f) - 1.0f), 0);

    vector.y = -vector.y;   //TODO: check this

    //Perform Pythag to get Z.
    float squared = pow(vector.x, 2) + pow(vector.y, 2);
    
    if (squared < 1)
        vector.z = sqrt(squared);  // Pythagoras
    else
        vector = glm::normalize(vector);  // nearest point
    
    return vector;
}


void GuiRenderWindow::Draw(double deltaTime) {
    is_alive = ImGui::Begin(name.c_str());
    // Using a Child allow to fill all the space of the window.
    ImGui::BeginChild("Render Window Child");

    // Get the size of the child (i.e. the whole draw size of the windows).
    window_size_ = ImGui::GetWindowSize();

    // Because we use the texture from OpenGL, we need to invert the V from the UV.
    ImGui::Image((ImTextureID)viewport_->texture, window_size_, ImVec2(0, 1), ImVec2(1, 0));

    HandleUI();

    DrawRenderWindowSettings(deltaTime);

    ImGui::EndChild();
    ImGui::End();
}