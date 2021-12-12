#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <iterator>
#include <vector>
#include <memory>

#include "cad_data/feature.hpp"
#include "cad_data/feature.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"

namespace cad_data {
    class SceneData {
        public:
            explicit SceneData(std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewport_list, std::string scene_title = "Untitled Scene") : scene_title(scene_title), viewports_(viewport_list) {};
            ~SceneData() = default;

            std::string scene_title;

            void MasterGeoPushBack(std::shared_ptr<cad_data::Feature> feature) {
                feature_timeline_.push_back(feature);

                for (const std::shared_ptr<cad_gui::Viewport>& v : *viewports_) {
                        v->feature_renderable_pairs.emplace_back(feature, nullptr);
                }
            }

            void MasterGeoErase(std::vector<std::shared_ptr<cad_data::Feature>>::iterator position) {
                (*position)->is_dead = true;
                feature_timeline_.erase(position);
            }

            void MasterGeoErase(int index) {
                auto iter = feature_timeline_.begin() + index;
                (*iter)->is_dead = true;
                feature_timeline_.erase(iter);
            }

            std::vector<std::shared_ptr<cad_data::Feature>>::iterator MasterGeoBegin() { return feature_timeline_.begin(); }
            std::vector<std::shared_ptr<cad_data::Feature>>::iterator MasterGeoEnd() { return feature_timeline_.end(); }


            void SelectedGeoPushBack(std::shared_ptr<cad_data::Feature> geometry) {
                selected_feature_list_.push_back(geometry);
                geometry->draw_aa_bounding_box = true;

            }

            void SelectedGeoErase(std::vector<std::shared_ptr<cad_data::Feature>>::iterator position) {
                (*position)->draw_aa_bounding_box = false;;
                selected_feature_list_.erase(position);
            }

            void SelectedGeoErase(int index) {
                auto iter = selected_feature_list_.begin() + index;
                (*iter)->draw_aa_bounding_box = false;
                selected_feature_list_.erase(iter);
            }

            void ClearSelectedGeo() {
                for (std::shared_ptr<cad_data::Feature> geo : selected_feature_list_) {
                    geo->draw_aa_bounding_box = false;
                }
                selected_feature_list_.clear();
            }

            std::vector<std::shared_ptr<cad_data::Feature>>::iterator SelectedFeatBegin() { return selected_feature_list_.begin(); }
            std::vector<std::shared_ptr<cad_data::Feature>>::iterator SelectedFeatEnd() { return selected_feature_list_.end(); }

        private:
            std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewports_;
            std::vector<std::shared_ptr<cad_data::Feature>> feature_timeline_;
            std::vector<std::shared_ptr<cad_data::Feature>> selected_feature_list_;
    };
}
#endif
