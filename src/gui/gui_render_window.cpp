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
    glm::vec2 viewport_pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    glm::vec2 mouse_pos =    glm::vec2(io.MousePos.x, io.MousePos.y)  - viewport_pos;
    glm::vec2 mouse_pos_last = mouse_pos - mouse_delta;

    glm::vec2 ratio = glm::vec2(((mouse_pos .x/window_size_.x) * 2.0f) - 1.0f, ((mouse_pos.y/window_size_.y) * 2.0f) - 1.0f);

    bool image_hovered = ImGui::IsItemHovered();

    std::cout << "\nviewport pos: " << glm::to_string(glm::vec2(viewport_pos.x, viewport_pos.y)) << std::endl;
    std::cout << "mouse pos (pixels): " << glm::to_string(glm::vec2(mouse_pos.x, mouse_pos.y)) << std::endl;
    std::cout << "mouse pos prev (pixels): " << glm::to_string(glm::vec2(mouse_pos_last.x, mouse_pos_last.y)) << std::endl;
    std::cout << "window_size: " << glm::to_string(glm::vec2(window_size_.x, window_size_.y)) << std::endl;
    std::cout << "ratio: " << glm::to_string(ratio) << std::endl;
    
    Camera* camera = viewport_->camera;

    //Allow for mouse dragging outside of the render window once clicked & held.
    if(image_hovered || is_dragging) {
        if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            if (mouse_pos_last != mouse_pos) {
                is_dragging = true;

                std::cout << "mouse_pos_last: " << glm::to_string(mouse_pos_last) << std::endl;
                std::cout << "mouse_pos: " <<  glm::to_string(mouse_pos) << std::endl;

                glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(window_size_.x, window_size_.y));
                glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(window_size_.x, window_size_.y));

                //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
                glm::vec3 cross_vector = glm::cross(last_pos_vec, pos_vec);

                //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
                float angle = acos(glm::dot(last_pos_vec, pos_vec));

                //Create normalised rotation quaternion from the axis and the angle.
                glm::quat rotation_quat = glm::angleAxis(angle, cross_vector);
                glm::normalize(rotation_quat);

                glm::mat4 rotation_matrix = glm::toMat4(rotation_quat);
                camera->view_matrix *= rotation_matrix;
            }
        }
    }

    if(ImGui::IsMouseReleased(0)) {
        is_dragging = false;
    }
}

glm::vec3 GuiRenderWindow::GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size) {
    //Convert mouse pos to homogenous coordinates (-1 to 1)
    glm::vec3 vector = glm::vec3(((screen_pos.x/(screen_size.x) * 2.0f) - 1.0f), ((screen_pos.y/(screen_size.y) * 2.0f) - 1.0f), 0);

    vector.y = -vector.y;   //TODO: Why?

    //Perform Pythagoras to get Z.
    float squared = pow(vector.x, 2) + pow(vector.y, 2);
    
    if (squared < 1)
        vector.z = sqrt(1 - squared);  // Pythagoras
    else
        vector = glm::normalize(vector);  // Nearest point
    
    std::cout << "vector: " << glm::to_string(vector) << std::endl;

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