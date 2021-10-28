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

    #include "cad-base/shader.hpp"
    #include "cad-base/geometry/geometry.hpp"
    #include "cad-base/renderable.hpp"
    #include "cad-base/camera.hpp"
    #include "cad-base/geometry/viewport_grid.hpp"
    #include "cad-base/gui/rendered_textures/gui_render_texture.hpp"

    class Viewport: public GuiRenderTexture, public std::enable_shared_from_this<Viewport> {
        public:
            Viewport(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height);
            ~Viewport() = default;

            std::shared_ptr<Viewport> GetSharedPtr() {
                return shared_from_this();
            }

            void Draw() override;

            // void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            // void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
            // void CursorCallback( GLFWwindow* window, double x, double y );
            // void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
            // void WindowSizeCallback(GLFWwindow* glfw_window, int width, int height);

            //Viewport specific stuff that doesn't need i.e selecting etc.
            std::vector<std::pair<std::shared_ptr<Geometry>, std::shared_ptr<Renderable>>> viewport_geo_renderable_pairs;

            std::shared_ptr<Geometry> render_axis;
            std::shared_ptr<ViewportGrid> grid;
    };

#endif
