#ifndef LEVEL_GEN_HPP
#define LEVEL_GEN_HPP

#include "OpenSimplexNoise.h"
#include <cad-base/level_geo.hpp>
#include <cstdlib>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using namespace std;

OpenSimplexNoise::Noise simplex_noise(std::rand());
OpenSimplexNoise::Noise simplex_noise_2(std::rand());

class LevelGen {
    public:
        LevelGen(shared_ptr<LevelGeo> level_geo) : level_geo(level_geo) {};


        static glm::vec3 GetGeoVector(int x, int y) {
            glm::vec3 geo_vector;
            float z_scale = 30.0f, resolution = 0.06f, resolution_2 = 0.025f;


            float z = (simplex_noise.eval(x * resolution, y * resolution) * 4.0f) 
                + (simplex_noise_2.eval(x * resolution_2, y * resolution_2) * .6f) * z_scale;

            geo_vector = glm::vec3(x, y, z);

            return geo_vector; 
        }


        bool GenerateGeometry() {
            vector<glm::vec3> test_geo;

            int x_size = 400, y_size = 400;

                for (int x = 0; x < x_size; x++) {
                    for (int y = 0; y < y_size; y++) {
                        
                        auto p1 = GetGeoVector(x, y);
                        auto p2 = GetGeoVector(x + 1, y);
                        auto p3 = GetGeoVector(x + 1, y + 1);
                        auto p4 = GetGeoVector(x, y + 1);
                        
                        test_geo.push_back(p1);
                        test_geo.push_back(p2);
                        test_geo.push_back(p3);
                        test_geo.push_back(p1);
                        test_geo.push_back(p3);
                        test_geo.push_back(p4);
                    }
                }

            level_geo->MasterGeoPushBack(make_shared<Geometry>(test_geo, "Test Geo " + std::to_string(rand()), glm::vec3(0.0f)));
            return true;
        }
    private:
        shared_ptr<LevelGeo> level_geo;


};

#endif