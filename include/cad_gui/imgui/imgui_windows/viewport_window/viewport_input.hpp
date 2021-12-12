#ifndef VIEWPORT_INPUT_HPP
#define VIEWPORT_INPUT_HPP

#include "cad_gui/opengl/camera.hpp"

#include <vector>

namespace CadGui {
    class ViewportInput { 
        public:
            explicit ViewportInput(std::vector<std::shared_ptr<Camera>> affected_cameras) : affected_cameras(affected_cameras) {};
            std::vector<std::shared_ptr<Camera>> affected_cameras;
            
        private:
    };
}

#endif