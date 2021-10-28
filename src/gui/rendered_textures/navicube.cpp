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