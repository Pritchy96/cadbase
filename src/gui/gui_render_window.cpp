#include "cad-base/gui/gui_render_window.hpp"
#include "imgui_internal.h"

#include <cmath>
#include <cstdio>
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
#include <vector>

GuiRenderWindow::GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport) : GuiBase(name, glfw_window), viewport_(viewport) {
    arcball_rotate_sensitivity = ARCBALL_ROTATE_SENSITIVITY_INITIAL;
    arcball_pan_sensitivity = ARCBALL_PAN_SENSITIVITY_INITIAL;
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
            viewport_->camera->SetProjectionStyle(true);
        }
        
        ImGui::SameLine();
        
        if (ImGui::RadioButton("Perspective", &ortho_not_persp, 1)) {
            viewport_->camera->SetProjectionStyle(false);
        }

        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &viewport_->grid->draw_geometry);
        ImGui::Checkbox("Show Render Axis", &viewport_->render_axis->draw_geometry);
        ImGui::Checkbox("Show Bounding Boxes", &viewport_->master_geo_renderable_pairs.back().second->draw_aa_bounding_box);
        
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

//TODO: Move to dedicated IO handler class
void GuiRenderWindow::HandleIO() {
    ImGuiIO& io = ImGui::GetIO();
    //TODO: we can apparently declare a macro in IMGUI.cpp and specify glm vectors to be used by ImGui? Investigate,
    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);
    glm::vec2 viewport_pos = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    //Get mouse pos in terms of the viewport window space.
    glm::vec2 mouse_pos =    glm::vec2(io.MousePos.x, io.MousePos.y)  - viewport_pos;
    glm::vec2 mouse_pos_last = mouse_pos - (mouse_delta * arcball_rotate_sensitivity);

    bool image_hovered = ImGui::IsItemHovered();

    //Store a bool to see if the mouse has clicked on the image with any given mouse button.
    //This is because clicking + dragging may mean we're hovered over another image but we still want to rotate
    //the image we've initially clicked on.
    for (int i = 0; i < 5; i++) {
        if (io.MouseClicked[i] && image_hovered) {
            clicked_on_image[i] = true;
        }
    }

    viewport_has_focus = (clicked_on_image[ImGuiMouseButton_Left] 
        || clicked_on_image[ImGuiMouseButton_Middle] 
        || clicked_on_image[ImGuiMouseButton_Right]);


    //Zoom
    if ((image_hovered || viewport_has_focus) && io.MouseWheel != 0) {
        viewport_->camera->SetZoom(viewport_->camera->GetZoom() + io.MouseWheel);
    }

    //Rotate
    if(ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        //Allow for mouse dragging outside of the render window once clicked & held.
        if(viewport_has_focus && mouse_pos != mouse_pos_last) {
            glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(window_size_.x, window_size_.y));
            glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(window_size_.x, window_size_.y));

            //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
            glm::vec3 cross_vector = glm::cross(pos_vec, last_pos_vec);

            //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
            float angle = acos(glm::dot(last_pos_vec, pos_vec));
            
            //Apply this multiplication to the pre-existing rotation applied when generating the view matrix. 
            viewport_->camera->SetRotation(glm::rotate(viewport_->camera->GetRotation(), angle, cross_vector));
        }

    //Pan
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        if (viewport_has_focus) {
            viewport_->camera->MoveTarget(glm::vec3(viewport_->camera->GetCameraTransform() * glm::vec4(mouse_delta.x/arcball_pan_sensitivity, -mouse_delta.y/arcball_pan_sensitivity, 0.0f, 0.0f)));
        }

    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {  //Object Selection
        if (image_hovered) {
            //Convert mouse pos to homogenous coordinates (-1 to 1)
            //We currently have z set as up, so we need to find y (examples generally have x, y and find z)
            glm::vec2 mouse_homo = glm::vec2(((mouse_pos.x/(window_size_.x) * 2.0f) - 1.0f), ((mouse_pos.y/(window_size_.y) * 2.0f) - 1.0f));  //NOLINT - not magic numbers just basic math
            glm::mat4 to_world =   glm::inverse(viewport_->camera->GetProjectionMatrix() * viewport_->camera->GetViewMatrix());

            glm::vec3 camera_pos = viewport_->camera->GetCameraTransform()[3];

            glm::vec4 mouse_world_pos = to_world * glm::vec4(mouse_homo.x, -mouse_homo.y, 1.0f, 1.0f);
            mouse_world_pos /= mouse_world_pos.w;
            glm::vec3 ray_dir = glm::normalize(glm::vec3(mouse_world_pos));

            std::vector<glm::vec3> line;
            line.emplace_back(camera_pos);
            line.emplace_back(camera_pos + (ray_dir * 10000.0f));
            std::shared_ptr<Geometry> line_geo = std::make_shared<Geometry>(line, line);
            viewport_->viewport_geo_renderable_pairs.emplace_back(line_geo, std::make_unique<Renderable>(viewport_->basic_shader, line_geo, GL_LINES));
                    
            std::cout << "Mouse Homo Coords: " << glm::to_string(mouse_homo) << std::endl;
            std::cout << "Camera Position: " << glm::to_string(camera_pos) << std::endl;
            std::cout << "Ray Direction: " << glm::to_string(ray_dir) << std::endl;

            for (const auto& grp : viewport_->master_geo_renderable_pairs) {
                glm::vec3 min = grp.first->aa_bounding_box.min;
                glm::vec3 max = grp.first->aa_bounding_box.max;

                grp.second->draw_aa_bounding_box = RayCubeIntersection(camera_pos, ray_dir, min, max);
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        if (ImGui::IsMouseReleased(i)) {
            clicked_on_image[i] = false;
        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        viewport_has_focus = false;
    }
}

//TODO: Obviously, temporary.
bool GuiRenderWindow::RayCubeIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 boxMin, glm::vec3 boxMax) {
    float tmin = (boxMin.x - rayOrigin.x) / rayDirection.x; 
    float tmax = (boxMax.x - rayOrigin.x) / rayDirection.x; 
 
    if (tmin > tmax) { std::swap(tmin, tmax); }
 
    float tymin = (boxMin.y - rayOrigin.y) / rayDirection.y; 
    float tymax = (boxMax.y - rayOrigin.y) / rayDirection.y; 
 
    if (tymin > tymax) std::swap(tymin, tymax); 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (boxMin.z - rayOrigin.z) / rayDirection.z; 
    float tzmax = (boxMax.z - rayOrigin.z) / rayDirection.z; 
 
    if (tzmin > tzmax) std::swap(tzmin, tzmax); 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true; 
}

glm::vec3 GuiRenderWindow::GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size) {
    //Convert mouse pos to homogenous coordinates (-1 to 1)
    //We currently have z set as up, so we need to find y (examples generally have x, y and find z)
    glm::vec3 vector = glm::vec3(((screen_pos.x/(screen_size.x) * 2.0f) - 1.0f), 0, ((screen_pos.y/(screen_size.y) * 2.0f) - 1.0f));

    vector.z = -vector.z;

    //Perform Pythagoras to get Y
    float squared = pow(vector.x, 2) + pow(vector.z, 2);
    
    if (squared < 1) {
        vector.y = sqrt(1 - squared);  // Pythagoras
    } else {
        vector = glm::normalize(vector);  // Nearest point
    }

    return vector;
}

void GuiRenderWindow::Draw(double deltaTime) {
    is_alive = ImGui::Begin(name.c_str());
    // Using a Child allow to fill all the space of the window.
    ImGui::BeginChild("Render Window Child");

    // Get the size of the child (i.e. the whole draw size of the windows).
    window_size_ = ImGui::GetWindowSize();

    // Because we use the texture from OpenGL, we need to invert the V from the UV.
    ImGui::Image((ImTextureID)viewport_->colour_texture, window_size_, ImVec2(0, 1), ImVec2(1, 0));

    HandleIO();

    DrawRenderWindowSettings(deltaTime);

    ImGui::EndChild();
    ImGui::End();
}