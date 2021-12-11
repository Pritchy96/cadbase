#include <glm/fwd.hpp>
#include <memory>

#include "cad_gui/opengl/arcball.hpp"
#include "cad_gui/opengl/camera.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport_input.hpp"

Arcball::Arcball(std::vector<std::shared_ptr<Camera>> affected_cameras) : ViewportInput(affected_cameras) {
    arcball_rotate_sensitivity = ARCBALL_ROTATE_SENSITIVITY_INITIAL;
    arcball_pan_sensitivity = ARCBALL_PAN_SENSITIVITY_INITIAL;
}


glm::vec3 Arcball::GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size) {
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

void Arcball::Rotate(std::shared_ptr<glm::vec2> window_size) {   
    ImGuiIO& io = ImGui::GetIO();

    //TODO: we can apparently declare a macro in IMGUI.cpp and specify glm vectors to be used by ImGui? Investigate.
    glm::vec2 window_offset = glm::vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);

    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

    glm::vec2 mouse_pos =    glm::vec2(io.MousePos.x, io.MousePos.y)  - window_offset;
    glm::vec2 mouse_pos_last = mouse_pos - (mouse_delta * arcball_rotate_sensitivity);

    glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(window_size->x, window_size->y));
    glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(window_size->x, window_size->y));

    //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
    glm::vec3 cross_vector = glm::cross(pos_vec, last_pos_vec);

    //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
    float angle = acos(glm::dot(last_pos_vec, pos_vec));
    
    // //Apply this multiplication to the pre-existing rotation applied when generating the view matrix. 
    for (std::shared_ptr<Camera> c :affected_cameras) {
        if(c->can_rotate) {
            c->SetRotation(glm::rotate(c->GetRotation(), angle, cross_vector));
            c->aligned_to_face = false; //Only special 'snap to' operations can set this to true.
        }
    }
}

void Arcball::Pan() {
    ImGuiIO& io = ImGui::GetIO();
    glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);
    
    for (std::shared_ptr<Camera> c :affected_cameras) {
        if (c->can_pan) {
            c->MoveTarget(glm::vec3(c->GetCameraTransform() * glm::vec4(mouse_delta.x/arcball_pan_sensitivity, -mouse_delta.y/arcball_pan_sensitivity, 0.0f, 0.0f)));
        }
    }
}

void Arcball::Zoom() {
    ImGuiIO& io = ImGui::GetIO();
    
    for (std::shared_ptr<Camera> c :affected_cameras) {
        if (c->can_zoom) {
            c->SetZoom(c->GetZoom() + io.MouseWheel);
        }
    }
}
