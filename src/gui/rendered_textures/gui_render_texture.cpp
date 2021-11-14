#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <vector>

#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"
#include "cad-base/camera.hpp"
#include "cad-base/gui/gui_data.hpp"
#include "cad-base/raycast/ray.hpp"
#include "imgui.h"

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

GuiRenderTexture::GuiRenderTexture(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height) {
	glfw_window_ = window;
	window_size = make_shared<glm::vec2>(window_width, window_width);
	background_colour = background_col;

	camera = make_shared<Camera>(Camera(glm::vec3(0.0f, 0.0f, 0.0f), 200.0f, window_size));

    std::vector<shared_ptr<Camera>> cameras = {camera};

    arcball = make_shared<Arcball>(cameras);

	// TODO: Move everything dependent on this (gl calls, shader loads etc) to an init() function so this context setting can be done 
	// in main.cpp
	glfwMakeContextCurrent(glfw_window_);

	// TODO: file paths are currently relative to excution path, not main location.
	// TODO: load these once, keep in static file?
	basic_shader = shader::LoadShaders((char*)"./shaders/basic_camera.vertshader", (char*)"./shaders/basic_camera.fragshader");

    SetupFBO();
}

void GuiRenderTexture::Update() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glEnable(GL_DEPTH_TEST); // Enable depth-testing
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, window_size->x, window_size->y);
    glClearColor(background_colour.r, background_colour.g, background_colour.b, background_colour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(render_face, render_mode);

    if (camera->is_slerping) {
        camera->UpdateSLERP();
    }

    Draw();

    // Bind default framebuffer again.
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST); // Disable depth testing for rest of UI.
    glDisable(GL_CULL_FACE);
}

void GuiRenderTexture::Draw() {
    //Render Master Geo.
    auto geo_renderable = geo_renderable_pairs.begin();

    while(geo_renderable != geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        renderable->Draw(camera->GetProjectionMatrix(), camera->GetViewMatrix());

        //Do this after drawing so geo updated this frame is loaded into the VAO 
        //(Geo is updated after renderables are drawn above)
        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
            renderable->valid_aa_bounding_box_vao = false;
        }
        ++geo_renderable;
    }

    //Render Debug Geo.
    geo_renderable = debug_geo_renderable_pairs.begin();

    while(geo_renderable != debug_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = debug_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        renderable->Draw(camera->GetProjectionMatrix(), camera->GetViewMatrix());

        //Do this after drawing so geo updated this frame is loaded into the VAO 
        //(Geo is updated after renderables are drawn above)
        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
            renderable->valid_aa_bounding_box_vao = false;
        }
        ++geo_renderable;
    }
}

void GuiRenderTexture::SetupFBO() {
    //Colour texture
	glGenTextures(1, &colour_texture);
	glBindTexture(GL_TEXTURE_2D, colour_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_size->x, window_size->y, 0, GL_RGBA, GL_UNSIGNED_BYTE , 0);
	glBindTexture(GL_TEXTURE_2D, 0);

    // Depth texture - Slower than depth buffer, but can sample it later in shader
    // TODO: If we don't end up using this texture and neeed a fps boost, we can change this to a buffer
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_size->x, window_size->y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //Attach textures to FBO context.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    // Check that framebuffer is ok
    CheckFramebufferStatus(fbo);

    // Unbind framebuffer (bind default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool GuiRenderTexture::CheckFramebufferStatus(GLuint fbo) {
    // check FBO status
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))  {
        case GL_FRAMEBUFFER_COMPLETE:
            spdlog::info("Framebuffer construction complete.");
            return true;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            spdlog::error("Framebuffer incomplete: Attachment is NOT complete.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            spdlog::error("Framebuffer incomplete: No image is attached to FBO.");
            return false;
        /*
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            spdlog::error("Framebuffer incomplete: Attached images have different dimensions.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
            spdlog::error("Framebuffer incomplete: Color attached images have different internal formats.");
            return false;
        */
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            spdlog::error("Framebuffer incomplete: Draw buffer.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            spdlog::error("Framebuffer incomplete: Read buffer.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            spdlog::error("Framebuffer incomplete: Multisample.");
            return false;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            spdlog::error("Framebuffer incomplete: Unsupported by FBO implementation.");
            return false;

        default:
            spdlog::error("Framebuffer incomplete: Unknown error.");
            return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GuiRenderTexture::HandleIO() {
    ImGuiIO& io = ImGui::GetIO();
    bool image_hovered = ImGui::IsItemHovered();
    
    //Store a bool to see if the mouse has clicked on the image with any given mouse button.
    //This is because clicking + dragging may mean we're hovered over another image but we still want to rotate
    //the image we've initially clicked on.
    for (int i = 0; i < 5; i++) {
        if (io.MouseClicked[i] && image_hovered) {
            clicked_on_texture[i] = true;
        }
    }

    texture_has_focus = (clicked_on_texture[ImGuiMouseButton_Left] 
        || clicked_on_texture[ImGuiMouseButton_Middle]
        || clicked_on_texture[ImGuiMouseButton_Right]);

    if(ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {  //Rotate
        //Allow for mouse dragging outside of the render window once clicked & held.
        if(texture_has_focus && io.MouseDelta.x != 0) {
            arcball->Rotate(window_size);
        }
    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {  //Object Selection
        if (image_hovered) {
            glm::vec2 window_offset = glm::vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
            glm::vec2 mouse_pos =     glm::vec2(io.MousePos.x, io.MousePos.y)  - window_offset;
            CastRay(mouse_pos);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (ImGui::IsMouseReleased(i)) {
            clicked_on_texture[i] = false;
        }
    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        texture_has_focus = false;
    }

    if (ImGui::IsKeyDown('C')) {
        debug_geo_renderable_pairs.clear();
    }
}

void GuiRenderTexture::CastRay(glm::vec2 mouse_pos) {
    //Convert mouse pos to homogenous coordinates (-1 to 1)
    //We currently have z set as up, so we need to find y (examples generally have x, y and find z)
    glm::vec2 mouse_homo = glm::vec2(((mouse_pos.x/(window_size->x) * 2.0f) - 1.0f), ((mouse_pos.y/(window_size->y) * 2.0f) - 1.0f));  //NOLINT - not magic numbers just basic math
    glm::mat4 view_to_world = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
    glm::vec3 camera_pos = camera->GetCameraTransform()[3];
    Ray ray;
        
    if (camera->IsOrthoCamera()) {  //Ortho
        //Get mouse coordinates in frustrum dimensions, rotated by the camera rotation
        glm::vec3 mouse_pos_viewport = camera->GetCameraTransform()
        * glm::vec4((mouse_homo.x * camera->GetOrthoFustrumWidth()) / 2.0f, 
            (-mouse_homo.y * camera->GetOrthoFustrumHeight()) / 2.0f,
                0.0f, 1.0f);

        ray.origin = camera_pos + mouse_pos_viewport;
        //Ray direction is just along the camera direction: ortho view has no distortion like perspective cameras
        ray.direction =  glm::normalize(camera->GetTarget() - glm::vec3(camera->GetCameraTransform()[3]));
    } else {    //Perspective
        ray.origin = camera_pos;
        glm::vec4 mouse_homo_world_pos = view_to_world * glm::vec4(mouse_homo.x, -mouse_homo.y, 1.0f, 1.0f);
        mouse_homo_world_pos /= mouse_homo_world_pos.w;
        ray.direction = glm::normalize(glm::vec3(mouse_homo_world_pos));
    }

    float closest_renderable_distance = MAXFLOAT;
    std::shared_ptr<Renderable> closest_renderable;

    for (const auto& grp : geo_renderable_pairs) {
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
        SelectRenderable(closest_renderable);
    } else {
        SelectNothing();
    }
}

void GuiRenderTexture::DrawDebugRay(Ray ray, glm::vec4 ray_colour) {
    // spdlog::info("Ray Origin: {0}, Ray Dir: {1}", glm::to_string(ray.origin), glm::to_string(ray.direction));
    // Debug: draw raycast lines
    std::vector<glm::vec3> line;
    line.emplace_back(ray.origin);
    line.emplace_back(ray.origin + (ray.direction * 100000.0f));
    std::vector<glm::vec3> line_colour;
    line_colour.emplace_back(ray_colour);
    line_colour.emplace_back(ray_colour);
    std::shared_ptr<Geometry> line_geo = std::make_shared<Geometry>(line, line_colour, "");
    debug_geo_renderable_pairs.emplace_back(line_geo, std::make_unique<Renderable>(basic_shader, line_geo, GL_LINES));
}

//TODO: Move (into static raycasting util class?)
bool GuiRenderTexture::RayCubeIntersection(Ray ray, std::array<glm::vec3, 2> boxBounds) {
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
