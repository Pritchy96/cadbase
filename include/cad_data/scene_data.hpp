#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <iterator>
#include <vector>
#include <memory>

#include "cad_data/part.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"

namespace cad_data {
    class SceneData {
        public:
            explicit SceneData(std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewport_list, std::string scene_title = "Untitled Scene") : scene_title(scene_title), viewports_(viewport_list) {};
            ~SceneData() = default;

            std::string scene_title;

            void PartListPushBack(std::shared_ptr<cad_data::Part> part) {
                part_list_.push_back(part);
            }

            void PartListErase(std::vector<std::shared_ptr<cad_data::Part>>::iterator position) {
                //TODO: Handle killing part subgeo
                // (*position)->is_dead = true;
                part_list_.erase(position);
            }
    
            void PartListErase(int index) {
                auto iter = part_list_.begin() + index;
                //TODO: Handle killing part subgeo
                //(*iter)->is_dead = true;
                part_list_.erase(iter);
            }

            std::vector<std::shared_ptr<cad_data::Part>>::iterator PartListBegin() { return part_list_.begin(); }
            std::vector<std::shared_ptr<cad_data::Part>>::iterator PartListEnd() { return part_list_.end(); }


            void SelectedPartPushBack(std::shared_ptr<cad_data::Part> part) {
                selected_part_list_.push_back(part);
                //TODO: construct a bounding box - whenever a feature is added or modified, iterate through the feature
                // bounding boxes and get max/min points to get the part AABB
                // part->draw_aa_bounding_box = true;   

            }

            void SelectedPartErase(std::vector<std::shared_ptr<cad_data::Part>>::iterator position) {
                // (*position)->draw_aa_bounding_box = false;;
                selected_part_list_.erase(position);
            }

            void SelectedPartErase(int index) {
                auto iter = selected_part_list_.begin() + index;
                // (*iter)->draw_aa_bounding_box = false;
                selected_part_list_.erase(iter);
            }

            void ClearSelectedPartList() {
                // for (std::shared_ptr<cad_data::Part> part : selected_part_list_) {
                //     part->draw_aa_bounding_box = false;
                // }
                selected_part_list_.clear();
            }

            std::vector<std::shared_ptr<cad_data::Part>>::iterator SelectedPartListBegin() { return selected_part_list_.begin(); }
            std::vector<std::shared_ptr<cad_data::Part>>::iterator SelectedPartListEnd() { return selected_part_list_.end(); }

        private:
            std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewports_;
            std::vector<std::shared_ptr<cad_data::Part>> part_list_;
            std::vector<std::shared_ptr<cad_data::Part>> selected_part_list_;
    };
}
#endif
