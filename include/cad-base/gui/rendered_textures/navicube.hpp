#ifndef NAVICUBE_HPP
#define NAVICUBE_HPP

#include "imgui.h"
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

class NaviCube : public GuiRenderTexture, public ViewportInput {
    public:
        NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height, std::vector<std::shared_ptr<Camera>> affected_cameras);
        void SelectRenderable(std::shared_ptr<Renderable> selected_renderable) override;
        void DeselectRenderable() override;
        void LoadTextures();
        glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

        GLuint arrow_plus_rotate, arrow_neg_rotate;

    private:
        constexpr const static float CUBE_SIZE = 100;

        std::shared_ptr<Renderable> selected_face_;

        GLuint *face_textures_ = new GLuint[NUM_FACES];

        const std::vector<std::string> FACE_TEXTURE_PATHS = {
            "/home/tom/git/cad-base/resources/navicube/right.tga",
            "/home/tom/git/cad-base/resources/navicube/left.tga",
            "/home/tom/git/cad-base/resources/navicube/front.tga",
            "/home/tom/git/cad-base/resources/navicube/back.tga",
            "/home/tom/git/cad-base/resources/navicube/top.tga",
            "/home/tom/git/cad-base/resources/navicube/bottom.tga"
        };

        const std::string ARROW_PLUS_ROTATE_TEXTURE_PATH = "/home/tom/git/cad-base/resources/navicube/arrow_plus_rotate.tga";
        const std::string ARROW_NEG_ROTATE_TEXTURE_PATH = "/home/tom/git/cad-base/resources/navicube/arrow_neg_rotate.tga";

        static const int NUM_FACES = 6;

        //1 & 2, 3 & 4, 5 & 6 are opposite pairs.
        const std::vector<glm::vec3> FACE_VERTS_1 = {
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
        }, 
        FACE_VERTS_2 = {
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        },
        FACE_VERTS_3 = {
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE)  
        }, 
        FACE_VERTS_4 = {
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        },
        FACE_VERTS_5 = {
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE), 
            glm::vec3( CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE)  
        }, 
        FACE_VERTS_6 = {
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3( CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE),  
            glm::vec3(-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE), 
            glm::vec3(-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE)  
        };

        const std::vector<glm::vec2> SQUARE_UVS = {
            glm::vec2(1.0f, 1.0f),   // top right
            glm::vec2(1.0f, 0.0f),   // bottom right
            glm::vec2(0.0f, 0.0f),   // bottom left
            glm::vec2(1.0f, 1.0f),   // top left
            glm::vec2(0.0f, 0.0f),   // top right
            glm::vec2(0.0f, 1.0f)   // bottom left
        };
};

#endif
