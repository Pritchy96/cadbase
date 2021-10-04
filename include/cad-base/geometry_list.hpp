#ifndef GEOMETRYLIST_HPP
#define GEOMETRYLIST_HPP

#include <iterator>
#include <vector>
#include <memory>
    
#include "cad-base/viewport.hpp"
#include "cad-base/viewport.hpp"
#include "cad-base/geometry.hpp"

    class GeometryList {
        public:
            explicit GeometryList(std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewport_list) : viewports(viewport_list) {};
            ~GeometryList() = default;
            std::vector<std::shared_ptr<Geometry>> geometry_list;

            void push_back(std::shared_ptr<Geometry> geometry) { //NOLINT: lower case method name to match std::vector method.
                geometry_list.push_back(geometry);
            
                for (const std::shared_ptr<Viewport>& v : *viewports) {
                        v->geo_renderable_pairs.emplace_back(geometry, nullptr);
                }
            }
            
            void erase(std::vector<std::shared_ptr<Geometry>>::iterator position) { //NOLINT: lower case method name to match std::vector method.
                (*position)->is_dead = true;
                geometry_list.erase(position);
            }

            void erase(int index) { //NOLINT: lower case method name to match std::vector method.
                auto iter = geometry_list.begin() + index;
                (*iter)->is_dead = true;
                geometry_list.erase(iter);
            }

            std::vector<std::shared_ptr<Geometry>>::iterator begin() { return geometry_list.begin(); }    //NOLINT: lower case method name to match std::vector method.
            std::vector<std::shared_ptr<Geometry>>::iterator end() { return geometry_list.end(); }    //NOLINT: lower case method name to match std::vector method.

        private:    
            std::shared_ptr<std::vector<std::shared_ptr<Viewport>>> viewports;
    };

#endif
