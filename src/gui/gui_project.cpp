#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>

#include "cad-base/gui/gui_project.hpp" 
#include "cad-base/gui/rendered_textures/viewport.hpp"
#include "imgui_internal.h"

using std::shared_ptr;

GuiProject::GuiProject(std::string name, GLFWwindow* glfw_window, std::shared_ptr<SceneData> scene_data) : name(name), glfw_window(glfw_window), scene_data(scene_data) {
  
}

void GuiProject::Draw() {
    ImGui::Begin(name.c_str());

    

    ImGui::Text((scene_data->scene_title + ": ").c_str());
    ImGui::Separator();

    bool selected;
    auto geo_ptr = scene_data->MasterGeoBegin();
        while (geo_ptr != scene_data->MasterGeoEnd()) {
        selected = std::find(scene_data->SelectedGeoBegin(), scene_data->SelectedGeoEnd(), (*geo_ptr)) != scene_data->SelectedGeoEnd();

        ImGui::Bullet();
        if (ImGui::Selectable((*geo_ptr)->name.c_str(), selected))   {
            if (!ImGui::GetIO().KeyShift) {    // Clear selection when Shift is not held
                scene_data->ClearSelectedGeo();
            }

            scene_data->SelectedGeoPushBack(*geo_ptr);
        }

        geo_ptr++;
    }


    ImGui::Separator();
    ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    ImGui::End();
}