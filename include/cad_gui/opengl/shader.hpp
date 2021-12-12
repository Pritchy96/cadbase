
#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>

namespace cad_gui {
    namespace Shader {
        GLuint LoadShaders(char* vertex_file_path, char* fragment_file_path);
        GLuint LoadTransformShader(char * path);
        std::string LoadFileFromDisk(char* path);
    };
}

#endif