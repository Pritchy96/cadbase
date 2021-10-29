#include "cad-base/gui/rendered_textures/navicube.hpp"
#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"

#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

NaviCube::NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height) 
    : GuiRenderTexture(window, background_col, window_width, window_height) {

    camera->SetProjectionStyle(true);
    camera->can_pan = false;
    camera->can_zoom = false;

    shared_ptr<Geometry> geo = make_shared<Geometry>(face_1_, "Face 1");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_2_, "Face 2");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));
    
    geo = make_shared<Geometry>(face_3_, "Face 3");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_4_, "Face 4");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_5_, "Face 5");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_6_, "Face 6");
	geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));
}

void NaviCube::DeselectRenderable() {
    // Handle de-selecting previous selection before selecting new one.
    if (selected_face_ != nullptr) {
        selected_face_->geometry->draw_aa_bounding_box = false;
        selected_face_ = nullptr;
    }
}


void NaviCube::SelectRenderable(std::shared_ptr<Renderable> selected_renderable) {    
    selected_face_ = selected_renderable;
    spdlog::info("Selected NaviCube Face: {0}", selected_renderable->geometry->name);
    selected_face_->geometry->draw_aa_bounding_box = true;
}