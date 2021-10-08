#ifndef GUI_BASE_HPP
#define GUI_BASE_HPP

#include <GLFW/glfw3.h>
#include <string>

class GuiBase {
        public:
            explicit GuiBase(std::string name, GLFWwindow* glfw_window);
            // ~GuiBase();

            GLFWwindow* glfw_window;
            std::string name;
            bool is_alive = true;
            virtual void Draw(double deltaTime) = 0;
        private:   
        
        };

#endif