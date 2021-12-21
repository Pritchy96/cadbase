#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "cad_gui/opengl/renderables/renderable.hpp"
#include <GL/glew.h>
#include <string>
#include <vector>

namespace cad_gui {

    enum DEFAULT_SHADER_INDEXES { BASIC_COLOUR, BASIC_TEXTURED };

    struct Shader {
        GLuint program = -1;
        std::string name;
        std::vector<std::shared_ptr<cad_gui::Renderable>> render_list;

        public:
            Shader(GLuint shader_program, std::string shader_name) : program(shader_program), name(shader_name) {};
            //TODO: destructor to kill the Gluint?
    };

    class Renderer {

        public:
            std::vector<Shader> shaders;

            Renderer() {
                LoadDefaultShaders();
            }
            
            void Update();
            
            void LoadDefaultShaders();
            void AddShader(const char* vert_shader_glsl, const char* frag_shader_glsl, char* shader_name);
            std::string LoadFileFromDisk(char* path);
    };
}  // namespace cad_gui

#endif