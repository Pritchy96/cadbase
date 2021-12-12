#ifndef VIEWPORT_GRID_HPP
#define VIEWPORT_GRID_HPP

#include "cad_gui/opengl/render_data_types/renderable/renderable.hpp"
#include "cad_gui/opengl/render_data_types/geometry/geometry.hpp"
#include <glm/fwd.hpp>

namespace CadGui {
    class ViewportGrid : public CadGui::Geometry {
        public:
            ViewportGrid(int xLines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour, GLuint shader);

            void SetupGrid(int xlines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour);
    };
}
#endif