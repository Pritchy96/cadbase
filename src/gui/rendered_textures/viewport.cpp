#include "cad-base/gui/rendered_textures/viewport.hpp"	
#include "cad-base/geometry/viewport_grid.hpp"
#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"

#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

const vector<vec3> AXIS_LINES = {
    vec3(0.0f, 0.0f, 0.0f),	// x
	vec3(100.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, 0.0f),	// y	
	vec3(0.0f, 100.0f, 0.0f), 
	vec3(0.0f, 0.0f, 0.0f),	// z
	vec3(0.0f, 0.0f, 100.0f),
};

const vector<vec3> AXIS_COLOURS = {
    vec3(1.0f, 0.0f, 0.0f),	// x
	vec3(1.0f, 0.0f, 0.0f),	
	vec3(0.0f, 1.0f, 0.0f), // y
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f),	// z
	vec3(0.0f, 0.0f, 1.0f)
};

Viewport::Viewport(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height) 
    : GuiRenderTexture(window, background_col, window_width, window_height) {

    spdlog::info("Viewport Initialised");

    render_axis = make_shared<Geometry>(AXIS_LINES, AXIS_COLOURS, "Render Axis");
	viewport_geo_renderable_pairs.emplace_back(render_axis, make_unique<Renderable>(basic_shader, render_axis, GL_LINES));

    grid = make_shared<ViewportGrid>(50, 50, 20, 20, glm::vec3(0.3f, 0.3f, 0.3f), basic_shader);
	viewport_geo_renderable_pairs.emplace_back(grid, make_unique<Renderable>(basic_shader, grid, GL_LINES));
}

void Viewport::Draw() { 
    //Call base class Draw method.
    GuiRenderTexture::Draw();

    //Render viewport specific Geometry.
    auto geo_renderable = viewport_geo_renderable_pairs.begin();

    while(geo_renderable != viewport_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = viewport_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
            renderable->valid_aa_bounding_box_vao = false;
        }

        renderable->Draw(camera->GetProjectionMatrix(), camera->GetViewMatrix());

        ++geo_renderable;
    }
}