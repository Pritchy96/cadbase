#ifndef VIEWPORTGRID_HPP
#define VIEWPORTGRID_HPP

#include "cad-base/renderable.hpp"
#include "cad-base/geometry/geometry.hpp"
#include <glm/fwd.hpp>

    class ViewportGrid : public Geometry {
        public:
            ViewportGrid(int xLines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour, GLuint shader);

            void SetupGrid(int xlines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour);
    };
#endif