#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

namespace cad_gui {

    struct Shader {
        GLuint program = -1;
        std::string name;

        public:
            Shader(GLuint shader_program, std::string shader_name) : program(shader_program), name(shader_name) {};
            //TODO: destructor to kill the Gluint?
    };

    class Renderer {

        public:
            std::vector<Shader> shaders;
            enum DEFAULT_SHADER_INDEXES { BASIC, BASIC_TEXTURED, BASIC_CAMERA };
            
            void LoadDefaultShaders();
            void AddShader(const char* vert_shader_glsl, const char* frag_shader_glsl, char* shader_name);
            std::string LoadFileFromDisk(char* path);
    };
}  // namespace cad_gui

#endif