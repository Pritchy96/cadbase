#ifndef NAVICUBE_HPP
#define NAVICUBE_HPP

#include <glm/fwd.hpp>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <memory>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GL/glxmd.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "cad-base/gui/rendered_textures/gui_render_texture.hpp"
#include "cad-base/shader.hpp"
#include "cad-base/geometry/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/camera.hpp"

class NaviCube : public GuiRenderTexture {
    public:
        NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height);
        void SelectRenderable(std::shared_ptr<Renderable> selected_renderable) override;
        void DeselectRenderable() override;
        glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

    private:
        constexpr const static float CUBE_SIZE = 100;

        std::shared_ptr<Renderable> selected_face_;

        //1 & 2, 3 & 4, 5 & 6 are opposite pairs.
        std::vector<glm::vec3> face_1_ = {
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
        }, 
        face_2_ = {
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        },
        face_3_ = {
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
        }, 
        face_4_ = {
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        },
        face_5_ = {
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        }, 
        face_6_ = {
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE)  
        };
};

#endif
