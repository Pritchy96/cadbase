#ifndef VIEWPORTINPUT_HPP
#define VIEWPORTINPUT_HPP

#include "camera.hpp"

#include <vector>

class ViewportInput { 
    public:
        explicit ViewportInput(std::vector<std::shared_ptr<Camera>> affected_cameras) : affected_cameras(affected_cameras) {};
        std::vector<std::shared_ptr<Camera>> affected_cameras;
        
    private:
};

#endif