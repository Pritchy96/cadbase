#ifndef SCENEDATA_HPP
#define SCENEDATA_HPP

#include <iterator>
#include <vector>
#include <memory>

#include "cad-base/geometry/geometry.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"

class LevelGeo {
    public:
        explicit LevelGeo(std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewport_list) : viewports_(viewport_list) {};

~LevelGeo() = default;

        void MasterGeoPushBack(std::shared_ptr<Geometry> geometry) {
            master_geometry_list_.push_back(geometry);

            for (const std::shared_ptr<Viewport>& v : *viewports_) {
                    v->geo_renderable_pairs.emplace_back(geometry, nullptr);
            }
        }

        void MasterGeoErase(std::vector<std::shared_ptr<Geometry>>::iterator position) {
            (*position)->is_dead = true;
            master_geometry_list_.erase(position);
        }

        void MasterGeoErase(int index) {
            auto iter = master_geometry_list_.begin() + index;
            (*iter)->is_dead = true;
            master_geometry_list_.erase(iter);
        }

        std::vector<std::shared_ptr<Geometry>>::iterator MasterGeoBegin() { return master_geometry_list_.begin(); }
        std::vector<std::shared_ptr<Geometry>>::iterator MasterGeoEnd() { return master_geometry_list_.end(); }

        void SelectedGeoPushBack(std::shared_ptr<Geometry> geometry) {
            selected_geometry_list_.push_back(geometry);
            geometry->draw_aa_bounding_box = true;

        }

        void SelectedGeoErase(std::vector<std::shared_ptr<Geometry>>::iterator position) {
            (*position)->draw_aa_bounding_box = false;;
            selected_geometry_list_.erase(position);
        }

        void SelectedGeoErase(int index) {
            auto iter = selected_geometry_list_.begin() + index;
            (*iter)->draw_aa_bounding_box = false;
            selected_geometry_list_.erase(iter);
        }

        void ClearSelectedGeo() {
            for (std::shared_ptr<Geometry> geo : selected_geometry_list_) {
                geo->draw_aa_bounding_box = false;
            }
            selected_geometry_list_.clear();
        }

        std::vector<std::shared_ptr<Geometry>>::iterator SelectedGeoBegin() { return selected_geometry_list_.begin(); }
        std::vector<std::shared_ptr<Geometry>>::iterator SelectedGeoEnd() { return selected_geometry_list_.end(); }


    private:
        std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewports_;
        std::vector<std::shared_ptr<Geometry>> master_geometry_list_;
        std::vector<std::shared_ptr<Geometry>> selected_geometry_list_;
};

#endif
