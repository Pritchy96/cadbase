#include <imgui.h>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/fwd.hpp>
#include <glm/ext.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>

#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_include.h>
#include <stb_image.h>

#include <cmath>
#include <memory>
#include <vector>

#include "cad_gui/imgui/imgui_windows/viewport_window/navicube.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/gui_render_texture.hpp"
#include "cad_gui/opengl/raycast/ray.hpp"
#include "cad_gui/opengl/render_data_types/renderable/textured_renderable.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport_input.hpp"
#include "cad_gui/imgui/app_style.hpp"

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

namespace CadGui {
    NaviCube::NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height, std::vector<std::shared_ptr<Camera>> affected_cameras) 
        : CadGui::GuiRenderTexture(window, background_col, window_width, window_height),
        CadGui::ViewportInput(affected_cameras) {
        
        camera->aligned_to_face = true; //TODO: do some logic to check if the initial target & position of camera means it's facing a navicube face.
        camera->SetProjectionStyle(true);
        camera->can_pan = false;
        camera->can_zoom = false;
        this->affected_cameras.emplace_back(camera);
        
        LoadTextures();

        //TODO: we need to restructure the program so we get data from the scene and render it, Calls to glUseShader are apparently extremely expensive,
        //So we need to figure out some way to sort data within a GuiRenderTexture/Renderable/whatever so we can just loop through the list of them at a top level,
        //Grab all the renderables that want to be rendered in a specific way, and then render them all in one batch.
        GLuint texture_shader = Shader::LoadShaders((char*)"./shaders/basic_textured.vertshader", (char*)"./shaders/basic_textured.fragshader");
        //TODO: feed this in from the ImGui Theming.

        AppStyle app_style;
        ImVec4 im_tint = app_style.ACCENT_COLOUR_DIM;
        
        glm::vec4 tint = glm::vec4(im_tint.x, im_tint.y, im_tint.z, im_tint.w);

        shared_ptr<Geometry> geo = make_shared<Geometry>(FACE_VERTS_1, SQUARE_UVS, "Face 1");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[0], geo, tint, GL_TRIANGLES));

        geo = make_shared<Geometry>(FACE_VERTS_2, SQUARE_UVS, "Face 2");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[1], geo, tint, GL_TRIANGLES));

        geo = make_shared<Geometry>(FACE_VERTS_3, SQUARE_UVS, "Face 3");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[2], geo, tint, GL_TRIANGLES));

        geo = make_shared<Geometry>(FACE_VERTS_4, SQUARE_UVS, "Face 4");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[3], geo, tint, GL_TRIANGLES));

        geo = make_shared<Geometry>(FACE_VERTS_5, SQUARE_UVS, "Face 5");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[4], geo, tint, GL_TRIANGLES));

        geo = make_shared<Geometry>(FACE_VERTS_6, SQUARE_UVS, "Face 6");
        geo_renderable_pairs.emplace_back(geo, make_unique<TexturedRenderable>(texture_shader, basic_shader, face_textures_[5], geo, tint, GL_TRIANGLES));
    }

    void NaviCube::LoadTextures() {
        glGenTextures(NUM_FACES, face_textures_);
        
        for (unsigned int i = 0; i < NUM_FACES; i++) {

            glBindTexture(GL_TEXTURE_2D, face_textures_[i]);

            int width, height, comp;
            unsigned char* image = stbi_load(FACE_TEXTURE_PATHS[i].c_str(), &width, &height, &comp, STBI_rgb_alpha);

            if(image == nullptr) {
                throw(std::string("Failed to load texture"));
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

            // Trilinear filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            // Generate Mipmaps
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glGenTextures(1, &arrow_plus_rotate);
        glBindTexture(GL_TEXTURE_2D, arrow_plus_rotate);
        int width, height, comp;
        unsigned char* image = stbi_load(ARROW_PLUS_ROTATE_TEXTURE_PATH.c_str(), &width, &height, &comp, STBI_rgb_alpha);
        
        if(image == nullptr) {
            throw(std::string("Failed to load texture"));
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // Generate Mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1, &arrow_neg_rotate);
        glBindTexture(GL_TEXTURE_2D, arrow_neg_rotate);

        image = stbi_load(ARROW_NEG_ROTATE_TEXTURE_PATH.c_str(), &width, &height, &comp, STBI_rgb_alpha);
        
        if(image == nullptr) {
            throw(std::string("Failed to load texture"));
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        // Trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // Generate Mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //From: https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
    //TODO: Why does this work, but camera->SetRotation(glm::rotate(camera->GetRotation(), angle, cross_vector)); doesn't?
    glm::quat NaviCube::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
        start = normalize(start);
        dest = normalize(dest);

        float cos_theta = dot(start, dest);
        glm::vec3 rotation_axis;

        if (cos_theta < -1 + 0.001f) {
            // Special case when vectors in opposite directions - there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotation_axis = cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
            if (glm::length2(rotation_axis) < 0.01) { //Parallel, try again
                rotation_axis = cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
            }

            rotation_axis = normalize(rotation_axis);
            return glm::angleAxis(glm::radians(180.0f), rotation_axis);
        }

        rotation_axis = glm::cross(start, dest);

        //Optimised Implementation from Stan Melax's Game Programming Gems 1 article
        float s = std::sqrt((1+cos_theta) * 2);
        float invs = 1 / s;

        return {
            s * 0.5f, 
            rotation_axis.x * invs,
            rotation_axis.y * invs,
            rotation_axis.z * invs
        };
    }

    void NaviCube::SelectRenderable(std::shared_ptr<Renderable> selected_renderable) {
        selected_face_ = selected_renderable;
        spdlog::info("Selected NaviCube Face: {0}", selected_renderable->geometry->name);

        glm::vec3 camera_forward = glm::normalize(camera->GetCameraTransform()[2]);
        glm::vec3 camera_up = camera->GetCameraTransform()[1];

        //Some (kinda dumb) maths to figure out the surface normal of the face, taking advantage of the fact that the navicube is just a box with center (0, 0, 0)
        //also figure out the new up direction
        glm::vec3 summed_triangle = selected_face_->geometry->vertexes.at(0) + selected_face_->geometry->vertexes.at(1) + selected_face_->geometry->vertexes.at(2);
        glm::vec3 surface_normal;

        //Cardinal direction of up vector of camera.
        //We use this to snap to the nearest cardinal direction as our new up vector.
        glm::vec3 new_up;
        glm::vec3 camera_up_abs = glm::abs(camera_up);

        if (std::abs(summed_triangle.x) == (3 * CUBE_SIZE)) {
            surface_normal = glm::normalize(glm::vec3(summed_triangle.x, 0.0f, 0.0f));

            if (camera_up_abs.y > camera_up_abs.z) {
                new_up = glm::normalize(glm::vec3(0.0f, camera_up.y, 0.0f));
            } else {
                new_up = glm::normalize(glm::vec3(0.0f, 0.0f, camera_up.z));
            }
        } else if (std::abs(summed_triangle.y) == (3 * CUBE_SIZE)) {
            surface_normal = glm::normalize(glm::vec3(0.0f, summed_triangle.y, 0.0f));

            if (camera_up_abs.x > camera_up_abs.z) {
                new_up = glm::normalize(glm::vec3(camera_up.x, 0.0f, 0.0f));
            } else {
                new_up = glm::normalize(glm::vec3(0.0f, 0.0f, camera_up.z));
            }
        } else {
            surface_normal = glm::normalize(glm::vec3(0.0f, 0.0f, summed_triangle.z));

            if (camera_up_abs.x > camera_up_abs.y) {
                new_up = glm::normalize(glm::vec3(camera_up.x, 0.0f, 0.0f));
            } else {
                new_up = glm::normalize(glm::vec3(0.0f, camera_up.y, 0.0f));
            }
        }

        //Rotate camera so that it's normal is perpendicular to clicked face.
        glm::quat camera_rotation_quat = RotationBetweenVectors(camera_forward, surface_normal);

        // Set up vector to closest cardinal direction to camera original up.
        glm::vec3 current_up = camera_rotation_quat * camera_up;
        glm::quat up_vector_quat = RotationBetweenVectors(current_up, new_up);
        
        // Generate new rotation
        for (std::shared_ptr<Camera> c : affected_cameras) {
            c->SLERPCameraRotation(up_vector_quat * camera_rotation_quat * glm::quat(camera->GetRotation()), c->STANDARD_SLERP_TIME);
        }

        camera->aligned_to_face = true; //Special case, we've set the camera forward to be parallel to a face normal.
    }
}