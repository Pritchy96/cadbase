#include "cad_gui/opengl/renderer.hpp"

#include <string>
#include <spdlog/spdlog.h>
#include <iostream>
#include <ostream>
#include <fstream>

using std::string;

namespace cad_gui {
        void Renderer::LoadDefaultShaders() {
            auto vert_glsl = LoadFileFromDisk("./shaders/basic_colour.vertshader");
            auto frag_glsl = LoadFileFromDisk("./shaders/basic_colour.fragshader");
            AddShader(vert_glsl.c_str(), frag_glsl.c_str(), "Basic Colour");

            vert_glsl = LoadFileFromDisk("./shaders/basic_textured.vertshader");
            frag_glsl = LoadFileFromDisk("./shaders/basic_textured.fragshader");
            AddShader(vert_glsl.c_str(), frag_glsl.c_str(), "Basic Textured");
        }

        void Renderer::AddShader(const char* vert_shader_glsl, const char* frag_shader_glsl, char* shader_name) {
            spdlog::info("Compiling Shaders");

            GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
            GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

            GLint compile_result = GL_FALSE; int info_log_size;

            // Compile Vertex Shader
            spdlog::info("Compiling Vertex Shader: \"{0}\"", shader_name);
            char const* vert_code_pntr = vert_shader_glsl;
            glShaderSource(vert_shader_id, 1, &vert_code_pntr , nullptr);
            glCompileShader(vert_shader_id);

            // Check Vertex Shader
            glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &compile_result);
            glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
            std::vector<char> vertex_shader_error_message(info_log_size);
            glGetShaderInfoLog(vert_shader_id, info_log_size, nullptr, &vertex_shader_error_message[0]);
            
            if (!compile_result) {
                spdlog::error("Vertex Shader error: {0}", &vertex_shader_error_message[0]);
            }
            spdlog::info("Vertex Shader compiled and checked");

            // Compile Fragment Shader
            spdlog::info("Compiling Fragment Shader: \"{0}\"", shader_name);
            char const* fragment_source_pointer = frag_shader_glsl;
            glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , nullptr);
            glCompileShader(fragment_shader_id);

            // Check Fragment Shader
            glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compile_result);
            glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
            std::vector<char> fragment_shader_error_message(info_log_size);
            glGetShaderInfoLog(fragment_shader_id, info_log_size, nullptr, &fragment_shader_error_message[0]);

            if (!compile_result) {
                spdlog::error("Fragment Shader error: {0}", &fragment_shader_error_message[0]);
            }
            spdlog::info("Fragment Shader compiled and checked");

            // Link the program
            spdlog::info("Linking Shader Program");
            GLuint program_id = glCreateProgram();
            glAttachShader(program_id, vert_shader_id);
            glAttachShader(program_id, fragment_shader_id);

            glLinkProgram(program_id); 
            glUseProgram(program_id);

            // Check the program
            glGetProgramiv(program_id, GL_LINK_STATUS, &compile_result);
            glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_size);
            std::vector<char> program_error_message(std::max(info_log_size, int(1)) );
            glGetProgramInfoLog(program_id, info_log_size, nullptr, &program_error_message[0]);
            
            if (!compile_result) {
                spdlog::error("Shader Program error: {0}", &program_error_message[0]);
            }
            spdlog::info("Shader Program created and checked");

            glDeleteShader(vert_shader_id);
            glDeleteShader(fragment_shader_id);

            shaders.emplace_back(program_id, shader_name);
        }

        void Renderer::Update() {
            
        }

        string Renderer::LoadFileFromDisk(char* path) {
            string contents;
            std::ifstream file_stream(path, std::ios::in);
            if(file_stream.is_open()){
                string line;
                while(getline(file_stream, line)) {
                    contents += "\n" + line;
                }
                file_stream.close();
            }

            spdlog::info("File read from disk");
            return contents;
        }
    
}  // namespace cad_gui