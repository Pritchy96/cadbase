#ifndef GUI_SETTINGS_HPP
#define GUI_SETTINGS_HPP

#include <memory>
#include <string>

#include "cad-base/viewport.hpp"

struct GuiSettings {
        public:
            explicit GuiSettings(std::string name, std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewports);
            // ~GuiSettings();

            std::string name;
            bool is_alive = true;

            void Draw(double deltaTime);
        private:   
            std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewports_;
        };

#endif