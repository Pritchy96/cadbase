#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <glm/fwd.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <memory>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GL/glxmd.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "cad_data/feature.hpp"
#include "cad_gui/opengl/renderables/renderable.hpp"
#include "cad_gui/imgui/gui_data.hpp"
#include "cad_gui/opengl/camera.hpp"
#include "cad_gui/opengl/renderables/viewport_grid.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/gui_render_texture.hpp"

// #include "cad_data/scene_data.hpp"
namespace cad_data {
    class SceneData;   
}

namespace cad_gui {
    class Viewport: public cad_gui::GuiRenderTexture, public std::enable_shared_from_this<Viewport> {
        public:
            Viewport(GLFWwindow *window, glm::vec4 background_col, int viewport_width, int viewport_height, std::shared_ptr<cad_data::SceneData> scene_data);
            ~Viewport() = default;

            std::shared_ptr<Viewport> GetSharedPtr() {
                return shared_from_this();
            }

            void Draw() override;
            void HandleIO() override;
            void SelectRenderable(std::shared_ptr<Renderable> selected_renderable) override;
            void SelectNothing() override;

            //Viewport specific stuff that doesn't need i.e selecting etc.
            std::vector<std::pair<std::shared_ptr<cad_data::Feature>, std::shared_ptr<Renderable>>> feature_renderable_pairs;

            std::shared_ptr<cad_data::SceneData> scene_data;
            std::shared_ptr<cad_data::Feature> render_axis;
            std::shared_ptr<ViewportGrid> grid;
    };
}
#endif
