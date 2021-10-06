#ifndef VIEWPORTGRID_HPP
#define VIEWPORTGRID_HPP

#include "renderable.hpp"
#include "geometry.hpp"
#include <glm/fwd.hpp>

    class ViewportGrid : public Geometry {
        public:
            ViewportGrid(int xLines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour, GLuint shader);

            void SetupGrid(int xlines, int yLines, float xSpacing, float ySpacing, glm::vec3 gridColour);
    };
#endif