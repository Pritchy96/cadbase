#include "cad-base/gui/gui_base.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>


GuiBase::GuiBase(std::string name, GLFWwindow* glfw_window) : name(name), glfw_window(glfw_window){
    
}
