#ifndef VIEWPORTGRID_HPP
#define VIEWPORTGRID_HPP

#include "renderable.hpp"
#include "geometry.hpp"

    class ViewportGrid : public Geometry {
        public:
            ViewportGrid(int xLines, int yLines, float xSpacing, float ySpacing, GLuint shader);

            void SetupGrid(int xlines, int yLines, float xSpacing, float ySpacing);
    };
#endif