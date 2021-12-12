#ifndef VIEWPORT_GRID_HPP
#define VIEWPORT_GRID_HPP

#include "cad_gui/opengl/renderables/renderable.hpp"
#include "cad_data/feature.hpp"
#include <glm/fwd.hpp>

//TODO: restructure so this doesn't extend feature.
namespace cad_gui {
    class ViewportGrid : public cad_data::Feature {
        public:
            ViewportGrid(int xLines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour, GLuint shader);

            void SetupGrid(int xlines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour);
    };
}
#endif