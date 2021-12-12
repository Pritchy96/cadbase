#include <algorithm>
#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"	
#include "cad_gui/opengl/renderables/viewport_grid.hpp"
#include "cad_gui/imgui/gui_data.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/gui_render_texture.hpp"
#include "cad_data/scene_data.hpp"

using std::vector;

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

namespace cad_gui {

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

    Viewport::Viewport(GLFWwindow *window, glm::vec4 background_col, int viewport_width, int viewport_height, shared_ptr<cad_data::SceneData> scene_data) 
        : GuiRenderTexture(window, background_col, viewport_width, viewport_height), scene_data(scene_data) {

        spdlog::info("Viewport Initialised");

        render_axis = make_shared<cad_data::Feature>(AXIS_LINES, AXIS_COLOURS, "Render Axis");
        feature_renderable_pairs.emplace_back(render_axis, make_unique<Renderable>(basic_shader, render_axis, GL_LINES));

        grid = make_shared<ViewportGrid>(50, 50, 20, 20, glm::vec3(0.3f, 0.3f, 0.3f), basic_shader);
        feature_renderable_pairs.emplace_back(grid, make_unique<Renderable>(basic_shader, grid, GL_LINES));
    }

    void Viewport::Draw() {
        //Call base class Draw method.
        GuiRenderTexture::Draw();

        //Render viewport specific Geometry.
        auto geo_renderable = feature_renderable_pairs.begin();

        while(geo_renderable != feature_renderable_pairs.end()) {
            // Geo is dead, nuke the map link
            if (geo_renderable->first->is_dead) {
                // iterator.erase gives the next item in the list.
                geo_renderable = feature_renderable_pairs.erase(geo_renderable);
                continue;
            }

            if (geo_renderable->second == nullptr) {
                // Renderable for geo doesn't exist, make one.
                // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
                geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
            }

            shared_ptr<cad_data::Feature> geometry = geo_renderable->first;
            shared_ptr<Renderable> renderable = geo_renderable->second;

            if (geometry->buffers_invalid) {
                renderable->valid_geometry_vao = false;
                renderable->valid_aa_bounding_box_vao = false;
            }

            renderable->Draw(camera->GetProjectionMatrix(), camera->GetViewMatrix());

            ++geo_renderable;
        }
    }

    void Viewport::HandleIO() {
        GuiRenderTexture::HandleIO();

        ImGuiIO& io = ImGui::GetIO();
        bool image_hovered = ImGui::IsItemHovered();

        //Zoom  
        if ((image_hovered || texture_has_focus) && io.MouseWheel != 0) {
            arcball->Zoom();
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {  //Pan
            if (image_hovered) {
                arcball->Pan();
            }
        } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            if (image_hovered) {
                glm::vec2 mouse_delta = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

                //TODO: use Persp matrix and distance from camera > selected object to make movement of object 1:1 with movement of object.
                glm::vec4 mouse_delta_world = camera->GetRotation() * glm::vec4(mouse_delta.x, 0.0f, -mouse_delta.y, 1.0f);
                mouse_delta_world /= mouse_delta_world.w;

                auto selected_ptr = scene_data->SelectedFeatBegin();
                while (selected_ptr != scene_data->SelectedFeatEnd()) {
        
                    (*selected_ptr)->MoveOrigin(mouse_delta_world);
                    selected_ptr++;

                }
            }
        }
    }

    void Viewport::SelectRenderable(shared_ptr<Renderable> clicked_renderable) {    

        bool object_already_selected = std::find(scene_data->SelectedFeatBegin(), scene_data->SelectedFeatEnd(), clicked_renderable->feature) != scene_data->SelectedFeatEnd();
        
        //Don't deselect all geo other than clicked object if it's already clicked 
        //This is annoying behaviour for the user if they accidentally click a selected object
        if (!ImGui::GetIO().KeyShift && !object_already_selected) {
            scene_data->ClearSelectedGeo();
        }

        //Not in list, add it.
        if (!object_already_selected) {
            scene_data->SelectedGeoPushBack(clicked_renderable->feature);
        }
    }

    void Viewport::SelectNothing() {    
        if (!ImGui::GetIO().KeyShift) {    // Clear selection when Shift is not held
            scene_data->ClearSelectedGeo();
        }
    }

}