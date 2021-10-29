#ifndef GUI_DATA_HPP
#define GUI_DATA_HPP

#include <memory>

#include "cad-base/renderable.hpp"

struct GuiData {
    std::shared_ptr<Renderable> selected_renderable;
};

#endif