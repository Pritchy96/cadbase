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
#include "cad-base/raycast/ray.hpp"
#include "cad-base/navicube.hpp"

GuiRenderWindow::GuiRenderWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<Viewport> viewport, std::shared_ptr<GuiData> gui_data) 
                                    : name(name), glfw_window(glfw_window), viewport_(viewport), gui_data(gui_data) {
    arcball_rotate_sensitivity = ARCBALL_ROTATE_SENSITIVITY_INITIAL;
    arcball_pan_sensitivity = ARCBALL_PAN_SENSITIVITY_INITIAL;

    navicube_ = std::make_shared<NaviCube>(glfw_window, glm::vec4(viewport_->background_colour, 0.0f), 200, 200);
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
        //TODO: Implement proper "Render all geo's bounding boxes in this viewport" option
        // ImGui::Checkbox("Show Bounding Boxes", &viewport_->master_geo_renderable_pairs.back().second->draw_aa_bounding_box);
        
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
void GuiRenderWindow::HandleViewportIO() {
    ImGuiIO& io = ImGui::GetIO();
    //TODO: we can apparently declare a macro in IMGUI.cpp and specify glm vectors to be used by ImGui? Investigate.
    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

    glm::vec2 offset = glm::vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
    //Get mouse pos in terms of the viewport window space.
    glm::vec2 mouse_pos =    glm::vec2(io.MousePos.x, io.MousePos.y)  - offset;
    glm::vec2 mouse_pos_last = mouse_pos - (mouse_delta * arcball_rotate_sensitivity);

    bool image_hovered = ImGui::IsItemHovered();

    //Store a bool to see if the mouse has clicked on the image with any given mouse button.
    //This is because clicking + dragging may mean we're hovered over another image but we still want to rotate
    //the image we've initially clicked on.
    for (int i = 0; i < 5; i++) {
        if (io.MouseClicked[i] && image_hovered) {
            clicked_on_viewport[i] = true;
        }
    }

    viewport_has_focus = (clicked_on_viewport[ImGuiMouseButton_Left] 
        || clicked_on_viewport[ImGuiMouseButton_Middle] 
        || clicked_on_viewport[ImGuiMouseButton_Right]);

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
            navicube_->camera->SetRotation(glm::rotate(navicube_->camera->GetRotation(), angle, cross_vector));
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
            glm::mat4 camera_transform = glm::inverse(viewport_->camera->GetProjectionMatrix() * viewport_->camera->GetViewMatrix());
            glm::vec3 camera_pos = camera_transform[3];
            Ray ray;
             
            if (viewport_->camera->IsOrthoCamera()) {  //Ortho
                //Get mouse coordinates in frustrum dimensions, rotated by the camera rotation
                glm::vec3 mouse_pos_viewport = viewport_->camera->GetCameraTransform()
                * glm::vec4((mouse_homo.x * viewport_->camera->GetOrthoFustrumWidth()) / 2.0f, 
                    (-mouse_homo.y * viewport_->camera->GetOrthoFustrumHeight()) / 2.0f,
                        0.0f, 1.0f);

                ray.origin = camera_pos + mouse_pos_viewport;
                //Ray direction is just along the camera direction: ortho view has no distortion like perspective cameras
                ray.direction =  glm::normalize(viewport_->camera->GetTarget() - glm::vec3(viewport_->camera->GetCameraTransform()[3]));
            } else {    //Perspective
                ray.origin = camera_pos;
                glm::vec4 mouse_homo_world_pos = camera_transform * glm::vec4(mouse_homo.x, -mouse_homo.y, 1.0f, 1.0f);
                mouse_homo_world_pos /= mouse_homo_world_pos.w;
                ray.direction = glm::normalize(glm::vec3(mouse_homo_world_pos));
            }

            // {   //DEBUG
            //     spdlog::info("Ray Origin: {0}, Ray Dir: {1}", glm::to_string(ray.origin), glm::to_string(ray.direction));
            //     // Debug: draw raycast lines
            //     std::vector<glm::vec3> line;
            //     line.emplace_back(ray.origin);
            //     line.emplace_back(ray.origin + (ray.direction * 100000.0f));
            //     std::vector<glm::vec3> line_colour;
            //     line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
            //     line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
            //     std::shared_ptr<Geometry> line_geo = std::make_shared<Geometry>(line, line_colour, "");
            //     viewport_->debug_geo_renderable_pairs.emplace_back(line_geo, std::make_unique<Renderable>(viewport_->basic_shader, line_geo, GL_LINES));
            // }

            //Visually unselect previously selected renderable, clear out the selection.
            if (gui_data->selected_renderable != nullptr) {
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = false;
                gui_data->selected_renderable = nullptr;
            }

            float closest_renderable_distance = MAXFLOAT;
            std::shared_ptr<Renderable> closest_renderable;

            for (const auto& grp : viewport_->master_geo_renderable_pairs) {
            //TODO: Ensure closest_renderable_distance > minimum distance? stops selecting objects that are < min clipping distance.
                if (RayCubeIntersection(ray, {grp.first->aa_bounding_box.min, grp.first->aa_bounding_box.max})) {

                    //We only want to pick the intersecting renderable closest to the camera.
                    glm::vec3 box_center = (glm::abs((grp.first->aa_bounding_box.max - grp.first->aa_bounding_box.min)) / 2.0f) + grp.first->aa_bounding_box.min;    //NOLINT: Not a magic number.

                    float distance_from_camera = glm::distance(box_center, camera_pos);

                    if (distance_from_camera < closest_renderable_distance) {
                        closest_renderable = grp.second;
                        closest_renderable_distance = distance_from_camera;
                    }
                }
            }

            if (closest_renderable != nullptr) {
                gui_data->selected_renderable = closest_renderable;
                spdlog::info(closest_renderable->geometry->name);
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = true;
            }
        }
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        if (image_hovered) {
            //TODO: use Persp matrix and distance from camera > selected object to make movement of object 1:1 with movement of object.
            glm::vec4 mouse_delta_world = viewport_->camera->GetRotation() * glm::vec4(mouse_delta.x, 0.0f, -mouse_delta.y, 1.0f);
            mouse_delta_world /= mouse_delta_world.w;

            if (gui_data->selected_renderable != nullptr) {
                gui_data->selected_renderable->geometry->MoveOrigin(mouse_delta_world);
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        if (ImGui::IsMouseReleased(i)) {
            clicked_on_viewport[i] = false;
        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        viewport_has_focus = false;
    }

    if (ImGui::IsKeyDown('C')) {
        navicube_->debug_geo_renderable_pairs.clear();
        viewport_->debug_geo_renderable_pairs.clear();
    }
}

//TODO: Move to dedicated IO handler class
void GuiRenderWindow::HandleNaviCubeIO() {
    ImGuiIO& io = ImGui::GetIO();
    //TODO: we can apparently declare a macro in IMGUI.cpp and specify glm vectors to be used by ImGui? Investigate,
    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);
    glm::vec2 offset = glm::vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
    //Get mouse pos in terms of the viewport window space.
    glm::vec2 mouse_pos = glm::vec2(io.MousePos.x, io.MousePos.y) - offset;
    glm::vec2 mouse_pos_last = mouse_pos - (mouse_delta * arcball_rotate_sensitivity);

    glm::vec2 cursorPos = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

    ImGui::GetCurrentWindow()->DrawList->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32( 255, 255, 0, 255 ));

    bool image_hovered = ImGui::IsItemHovered();

    //Store a bool to see if the mouse has clicked on the image with any given mouse button.
    //This is because clicking + dragging may mean we're hovered over another image but we still want to rotate
    //the image we've initially clicked on.
    for (int i = 0; i < 5; i++) {
        if (io.MouseClicked[i] && image_hovered) {
            clicked_on_navicube[i] = true;
        }
    }

    navicube_has_focus = (clicked_on_navicube[ImGuiMouseButton_Left] 
        || clicked_on_navicube[ImGuiMouseButton_Middle] 
        || clicked_on_navicube[ImGuiMouseButton_Right]);

    //Rotate
    if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        //Allow for mouse dragging outside of the render window once clicked & held.
        if(navicube_has_focus && mouse_pos != mouse_pos_last) {
            glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(window_size_.x, window_size_.y));
            glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(window_size_.x, window_size_.y));

            //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
            glm::vec3 cross_vector = glm::cross(pos_vec, last_pos_vec);

            //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
            float angle = acos(glm::dot(last_pos_vec, pos_vec));
            
            //Apply this multiplication to the pre-existing rotation applied when generating the view matrix. 
            viewport_->camera->SetRotation(glm::rotate(viewport_->camera->GetRotation(), angle, cross_vector));
            navicube_->camera->SetRotation(glm::rotate(navicube_->camera->GetRotation(), angle, cross_vector));
        }
    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {  //Object Selection
        if (image_hovered) {
            //Convert mouse pos to homogenous coordinates (-1 to 1)
            //We currently have z set as up, so we need to find y (examples generally have x, y and find z)
            glm::vec2 mouse_homo = glm::vec2(((mouse_pos.x/(navicube_->window_size->x) * 2.0f) - 1.0f), ((mouse_pos.y/(navicube_->window_size->y) * 2.0f) - 1.0f));  //NOLINT - not magic numbers just basic math
            glm::mat4 camera_transform = navicube_->camera->GetCameraTransform();
            glm::vec3 camera_pos = camera_transform[3];
            Ray ray;

            spdlog::info("mouse_homo: {0}", glm::to_string(mouse_homo));
            spdlog::info("mouse_homo: {0}", glm::to_string(mouse_homo));
            spdlog::info("camera_pos: {0}", glm::to_string(camera_pos));

            if (navicube_->camera->IsOrthoCamera()) {  //Ortho
                //Get mouse coordinates in frustrum dimensions, rotated by the camera rotation
                glm::vec3 mouse_pos_viewport = navicube_->camera->GetCameraTransform()
                * glm::vec4((mouse_homo.x * navicube_->camera->GetOrthoFustrumWidth()) / 2.0f, 
                    (-mouse_homo.y * navicube_->camera->GetOrthoFustrumHeight()) / 2.0f,
                        0.0f, 1.0f);

                ray.origin = camera_pos + mouse_pos_viewport;
                //Ray direction is just along the camera direction: ortho view has no distortion like perspective cameras
                ray.direction =  glm::normalize(navicube_->camera->GetTarget() - glm::vec3(navicube_->camera->GetCameraTransform()[3]));
            } else {    //Perspective
                ray.origin = camera_pos;
                glm::vec4 mouse_homo_world_pos = camera_transform * glm::vec4(mouse_homo.x, -mouse_homo.y, 1.0f, 1.0f);
                mouse_homo_world_pos /= mouse_homo_world_pos.w;
                ray.direction = glm::normalize(glm::vec3(mouse_homo_world_pos));
            }

            // {   //DEBUG
            //     spdlog::info("Ray Origin: {0}, Ray Dir: {1}", glm::to_string(ray.origin), glm::to_string(ray.direction));
            //     // Debug: draw raycast lines
            //     std::vector<glm::vec3> line;
            //     line.emplace_back(ray.origin);
            //     line.emplace_back(ray.origin + (ray.direction * 100000.0f));
            //     std::vector<glm::vec3> line_colour;
            //     line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
            //     line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
            //     std::shared_ptr<Geometry> line_geo = std::make_shared<Geometry>(line, line_colour, "");
            //     navicube_->debug_geo_renderable_pairs.emplace_back(line_geo, std::make_unique<Renderable>(viewport_->basic_shader, line_geo, GL_LINES));
            // }

            //Visually unselect previously selected renderable, clear out the selection.
            if (gui_data->selected_renderable != nullptr) {
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = false;
                gui_data->selected_renderable = nullptr;
            }

            float closest_renderable_distance = MAXFLOAT;
            std::shared_ptr<Renderable> closest_renderable;

            for (const auto& grp : navicube_->navicube_geo_renderable_pairs) {
            //TODO: Ensure closest_renderable_distance > minimum distance? stops selecting objects that are < min clipping distance.
                if (RayCubeIntersection(ray, {grp.first->aa_bounding_box.min, grp.first->aa_bounding_box.max})) {

                    //We only want to pick the intersecting renderable closest to the camera.
                    glm::vec3 box_center = (glm::abs((grp.first->aa_bounding_box.max - grp.first->aa_bounding_box.min)) / 2.0f) + grp.first->aa_bounding_box.min;    //NOLINT: Not a magic number.
                    float distance_from_camera = glm::distance(box_center, camera_pos);

                    if (distance_from_camera < closest_renderable_distance) {
                        closest_renderable = grp.second;
                        closest_renderable_distance = distance_from_camera;
                    }
                }
            }

            if (closest_renderable != nullptr) {
                gui_data->selected_renderable = closest_renderable;
                spdlog::info(closest_renderable->geometry->name);
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = true;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        if (ImGui::IsMouseReleased(i)) {
            clicked_on_navicube[i] = false;
        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        navicube_has_focus = false;
    }
}

//TODO: Move (into static raycasting util class?)
bool GuiRenderWindow::RayCubeIntersection(Ray ray, std::array<glm::vec3, 2> boxBounds) {
    glm::vec3 ray_direction_inv = 1.0f / ray.direction;  //Using the inverse of the ray direction allows for avoidance of some divide-by-zero issues.
    std::array<int, 3> sign;    //We can use the sign of each ray direction component to select the min or max box bound.

    sign[0] = (ray_direction_inv.x < 0) ? 1 : 0; 
    sign[1] = (ray_direction_inv.y < 0) ? 1 : 0; 
    sign[2] = (ray_direction_inv.z < 0) ? 1 : 0; 

    float tmin, tmax, tymin, tymax, tzmin, tzmax; //NOLINT: multiple declarations.
 
    tmin = (boxBounds[sign[0]].x - ray.origin.x) * ray_direction_inv.x; 
    tmax = (boxBounds[1 - sign[0]].x - ray.origin.x) * ray_direction_inv.x; 
    tymin = (boxBounds[sign[1]].y - ray.origin.y) * ray_direction_inv.y; 
    tymax = (boxBounds[1 - sign[1]].y - ray.origin.y) * ray_direction_inv.y; 
 
    if ((tmin > tymax) || (tymin > tmax)) { 
        return false; 
    }
    if (tymin > tmin) { 
        tmin = tymin; 
    }
    if (tymax < tmax) { 
        tmax = tymax; 
    }
 
    tzmin = (boxBounds[sign[2]].z - ray.origin.z) * ray_direction_inv.z; 
    tzmax = (boxBounds[1 - sign[2]].z - ray.origin.z) * ray_direction_inv.z; 
 
    if ((tmin > tzmax) || (tzmin > tmax)) { 
        return false; 
    }
    if (tzmin > tmin) { 
        tmin = tzmin; 
    }
    if (tzmax < tmax) { 
        tmax = tzmax; 
    }
 
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
    HandleViewportIO();


    navicube_->Update(deltaTime);
    ImGui::SetCursorPos(ImVec2(window_size_.x - 220, 20));
    ImGui::Image((ImTextureID)navicube_->colour_texture, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0));
    HandleNaviCubeIO();

    DrawRenderWindowSettings(deltaTime);

    ImGui::EndChild();
    ImGui::End();
}