#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <memory>
#include <string>
#include <algorithm>

#include "cad_data/part.hpp"
#include "cad_gui/imgui/imgui_windows/project_window.hpp" 
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"
#include "imgui_internal.h"

using std::shared_ptr;

namespace cad_gui {
    ProjectWindow::ProjectWindow(std::string name, GLFWwindow* glfw_window, std::shared_ptr<cad_data::SceneData> scene_data) : name(name), glfw_window(glfw_window), scene_data(scene_data) {
    
    }

    void ProjectWindow::Draw() {
        ImGui::Begin(name.c_str());
        ImGui::Text("%s", (scene_data->scene_title + ": ").c_str());
        ImGui::Separator();

        static ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        shared_ptr<cad_data::Part> part_clicked = nullptr;
        auto part_ptr = scene_data->PartListBegin();

        while (part_ptr != scene_data->PartListEnd()) {
            ImGuiTreeNodeFlags part_flags = tree_flags;
            if (std::find(scene_data->SelectedPartListBegin(), scene_data->SelectedPartListEnd(), (*part_ptr)) != scene_data->SelectedPartListEnd()) {
                part_flags |= ImGuiTreeNodeFlags_Selected;  //Show node visibly selected.
            }

            int index = part_ptr - scene_data->PartListBegin();
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)index, part_flags, "%s", (*part_ptr)->part_name.c_str());
            if (ImGui::IsItemClicked()) {
                part_clicked = (*part_ptr);
            }

            if (node_open)  {
                auto feature_ptr = (*part_ptr)->FeatureListBegin();

                while (feature_ptr != (*part_ptr)->FeatureListEnd()) {
                    ImGui::BulletText("%s\n", (*feature_ptr)->name.c_str());
                    feature_ptr++;
                }       
                ImGui::TreePop();
            }
            part_ptr++;
        }

        // Update selection state (outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (part_clicked != nullptr) {
            if (!ImGui::GetIO().KeyShift) {
                scene_data->ClearSelectedPartList();
            } 
            scene_data->SelectedPartPushBack(part_clicked);
        }

        ImGui::Separator();
        ImGui::Text("Application average: %.3f ms/frame\n(%.1f FPS)\n", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}