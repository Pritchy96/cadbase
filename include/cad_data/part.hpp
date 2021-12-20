#ifndef PART_HPP
#define PART_HPP

#include <iterator>
#include <vector>
#include <memory>

#include "cad_data/feature.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"

namespace cad_data {
    class Part {
        public:
            explicit Part(std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewport_list, std::string part_name  = "Untitled Part", glm::vec3 origin = glm::vec3(0.0f), std::vector<std::shared_ptr<cad_data::Feature>> features = {}) : part_name(part_name), viewports_(viewport_list) {
                for (std::shared_ptr<Feature> f : features) {
                    FeatureListPushBack(f);
                }
            };
            
            ~Part() = default;
    
            std::string part_name;

            void FeatureListPushBack(std::shared_ptr<cad_data::Feature> feature) {
                feature_timeline_.push_back(feature);

                for (const std::shared_ptr<cad_gui::Viewport>& v : *viewports_) {
                        v->feature_renderable_pairs.emplace_back(feature, nullptr);
                }
            }

            void FeatureListErase(std::vector<std::shared_ptr<cad_data::Feature>>::iterator position) {
                (*position)->is_dead = true;
                feature_timeline_.erase(position);
            }
    
            void FeatureListErase(int index) {
                auto iter = feature_timeline_.begin() + index;
                (*iter)->is_dead = true;
                feature_timeline_.erase(iter);
            }

            std::vector<std::shared_ptr<cad_data::Feature>>::iterator FeatureListBegin() { return feature_timeline_.begin(); }
            std::vector<std::shared_ptr<cad_data::Feature>>::iterator FeatureListEnd() { return feature_timeline_.end(); }

            void SelectedFeaturePushBack(std::shared_ptr<cad_data::Feature> feature) {
                selected_feature_list_.push_back(feature);
                feature->draw_aa_bounding_box = true;
            }

            void SelectedFeatureErase(std::vector<std::shared_ptr<cad_data::Feature>>::iterator position) {
                (*position)->draw_aa_bounding_box = false;;
                selected_feature_list_.erase(position);
            }

            void SelectedFeatureErase(int index) {
                auto iter = selected_feature_list_.begin() + index;
                (*iter)->draw_aa_bounding_box = false;
                selected_feature_list_.erase(iter);
            }

            void ClearSelectedFeature() {
                for (std::shared_ptr<cad_data::Feature> feature : selected_feature_list_) {
                    feature->draw_aa_bounding_box = false;
                }
                selected_feature_list_.clear();
            }

            std::vector<std::shared_ptr<cad_data::Feature>>::iterator SelectedFeatBegin() { return selected_feature_list_.begin(); }
            std::vector<std::shared_ptr<cad_data::Feature>>::iterator SelectedFeatEnd() { return selected_feature_list_.end(); }


            glm::vec3 GetOrigin() { return origin_; }

            void SetOrigin(glm::vec3 new_origin) {
                origin_ = new_origin;

                for (std::shared_ptr<cad_data::Feature> feature : feature_timeline_) {
                    feature->buffers_invalid = true;
                }
            }

            void MoveOrigin(glm::vec3 delta) {
                origin_ += delta;
                
                for (std::shared_ptr<cad_data::Feature> feature : feature_timeline_) {
                    feature->buffers_invalid = true;
                }
            }


        private:
            std::shared_ptr<std::vector<std::shared_ptr<cad_gui::Viewport>>> viewports_;
            std::vector<std::shared_ptr<cad_data::Feature>> feature_timeline_;
            std::vector<std::shared_ptr<cad_data::Feature>> selected_feature_list_;
            glm::vec3 origin_ = glm::vec3(0.0f); //Offsets the feature.

    };
}
#endif
