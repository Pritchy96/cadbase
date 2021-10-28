#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"
#include "cad-base/gui/gui_data.hpp"
#include "cad-base/raycast/ray.hpp"
#include "imgui.h"

#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

GuiRenderTexture::GuiRenderTexture(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height, shared_ptr<GuiData> gui_data) : gui_data(gui_data) {
    arcball_rotate_sensitivity = ARCBALL_ROTATE_SENSITIVITY_INITIAL;
    arcball_pan_sensitivity = ARCBALL_PAN_SENSITIVITY_INITIAL;
    
	glfw_window_ = window;
	window_size = make_shared<glm::vec2>(window_width, window_width);
	background_colour = background_col;

	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 200.0f, window_size);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_size->x, window_size->y, 0, GL_RGBA,GL_UNSIGNED_BYTE , 0);
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
            clicked_on_texture[i] = true;
        }
    }

    texture_has_focus = (clicked_on_texture[ImGuiMouseButton_Left] 
        || clicked_on_texture[ImGuiMouseButton_Middle] 
        || clicked_on_texture[ImGuiMouseButton_Right]);

    //Zoom
    if ((image_hovered || texture_has_focus) && io.MouseWheel != 0) {
        camera->SetZoom(camera->GetZoom() + io.MouseWheel);
    }

    //Rotate
    if(ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        //Allow for mouse dragging outside of the render window once clicked & held.
        if(texture_has_focus && mouse_pos != mouse_pos_last) {
            glm::vec3 last_pos_vec = GetArcballVector(mouse_pos_last, glm::vec2(window_size->x, window_size->y));
            glm::vec3 pos_vec = GetArcballVector(mouse_pos, glm::vec2(window_size->x, window_size->y));

            //Take the cross product of your start and end points (unit length vectors from the centre of the sphere) to form a rotational axis perpendicular to both of them. 
            glm::vec3 cross_vector = glm::cross(pos_vec, last_pos_vec);

            //Then to find the angle it must be rotated about this axis, take their dot product, which gives you the cosine of that angle.
            float angle = acos(glm::dot(last_pos_vec, pos_vec));
            
            //Apply this multiplication to the pre-existing rotation applied when generating the view matrix. 
            camera->SetRotation(glm::rotate(camera->GetRotation(), angle, cross_vector));
        }

    //Pan
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        if (texture_has_focus) {
            camera->MoveTarget(glm::vec3(camera->GetCameraTransform() * glm::vec4(mouse_delta.x/arcball_pan_sensitivity, -mouse_delta.y/arcball_pan_sensitivity, 0.0f, 0.0f)));
        }

    } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {  //Object Selection
        if (image_hovered) {
            //Convert mouse pos to homogenous coordinates (-1 to 1)
            //We currently have z set as up, so we need to find y (examples generally have x, y and find z)
            glm::vec2 mouse_homo = glm::vec2(((mouse_pos.x/(window_size->x) * 2.0f) - 1.0f), ((mouse_pos.y/(window_size->y) * 2.0f) - 1.0f));  //NOLINT - not magic numbers just basic math
            glm::mat4 camera_transform = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
            glm::vec3 camera_pos = camera_transform[3];
            Ray ray;
             

            spdlog::info("Mouse Homo: {0}", glm::to_string(mouse_homo));
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
                glm::vec4 mouse_homo_world_pos = camera_transform * glm::vec4(mouse_homo.x, -mouse_homo.y, 1.0f, 1.0f);
                mouse_homo_world_pos /= mouse_homo_world_pos.w;
                ray.direction = glm::normalize(glm::vec3(mouse_homo_world_pos));
            }

            {   //DEBUG
                spdlog::info("Ray Origin: {0}, Ray Dir: {1}", glm::to_string(ray.origin), glm::to_string(ray.direction));
                // Debug: draw raycast lines
                std::vector<glm::vec3> line;
                line.emplace_back(ray.origin);
                line.emplace_back(ray.origin + (ray.direction * 100000.0f));
                std::vector<glm::vec3> line_colour;
                line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
                line_colour.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
                std::shared_ptr<Geometry> line_geo = std::make_shared<Geometry>(line, line_colour, "");
                debug_geo_renderable_pairs.emplace_back(line_geo, std::make_unique<Renderable>(basic_shader, line_geo, GL_LINES));
            }

            //Visually unselect previously selected renderable, clear out the selection.
            if (gui_data->selected_renderable != nullptr) {
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = false;
                gui_data->selected_renderable = nullptr;
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
                gui_data->selected_renderable = closest_renderable;
                spdlog::info(closest_renderable->geometry->name);
                gui_data->selected_renderable->geometry->draw_aa_bounding_box = true;
            }
        }
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        if (image_hovered) {
            //TODO: use Persp matrix and distance from camera > selected object to make movement of object 1:1 with movement of object.
            glm::vec4 mouse_delta_world = camera->GetRotation() * glm::vec4(mouse_delta.x, 0.0f, -mouse_delta.y, 1.0f);
            mouse_delta_world /= mouse_delta_world.w;

            if (gui_data->selected_renderable != nullptr) {
                gui_data->selected_renderable->geometry->MoveOrigin(mouse_delta_world);
            }
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

glm::vec3 GuiRenderTexture::GetArcballVector(glm::vec2 screen_pos, glm::vec2 screen_size) {
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