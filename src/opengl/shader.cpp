#include <spdlog/spdlog.h>
#include <iostream>
#include <ostream>

#include "cad_gui/opengl/shader.hpp"

using std::string;
using std::vector;

namespace CadGui {
	GLuint Shader::LoadShaders(char* vertex_file_path, char* fragment_file_path) {
		spdlog::info("Compiling Shaders");

		GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

		string vert_shader_code = LoadFileFromDisk(vertex_file_path);
		string frag_shader_code = LoadFileFromDisk(fragment_file_path);
		spdlog::info("Shaders read from file");

		GLint compile_result = GL_FALSE; int info_log_size;

		// Compile Vertex Shader
		spdlog::info("Compiling Shader: \"{0}\"", vertex_file_path);
		char const * vert_code_pntr = vert_shader_code.c_str();
		glShaderSource(vert_shader_id, 1, &vert_code_pntr , nullptr);
		glCompileShader(vert_shader_id);

		// Check Vertex Shader
		glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &compile_result);
		glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
		vector<char> vertex_shader_error_message(info_log_size);
		glGetShaderInfoLog(vert_shader_id, info_log_size, nullptr, &vertex_shader_error_message[0]);
		
		if (!compile_result) {
			spdlog::error("Vertex Shader error: {0}", &vertex_shader_error_message[0]);
		}
		spdlog::info("Vertex Shader compiled and checked");


		// Compile Fragment Shader
		spdlog::info("Compiling Shader: \"{0}\"", fragment_file_path);
		char const *fragment_source_pointer = frag_shader_code.c_str();
		glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , nullptr);
		glCompileShader(fragment_shader_id);

		// Check Fragment Shader
		glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compile_result);
		glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
		vector<char> fragment_shader_error_message(info_log_size);
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
		vector<char> program_error_message(std::max(info_log_size, int(1)) );
		glGetProgramInfoLog(program_id, info_log_size, nullptr, &program_error_message[0]);
		
		if (!compile_result) {
			spdlog::error("Shader Program error: {0}", &program_error_message[0]);
		}
		spdlog::info("Shader Program created and checked");

		glDeleteShader(vert_shader_id);
		glDeleteShader(fragment_shader_id);

		return program_id;
	}

	string Shader::LoadFileFromDisk(char* path) {
		string contents;
		std::ifstream file_stream(path, std::ios::in);
		if(file_stream.is_open()){
			string line;
			while(getline(file_stream, line)) {
				contents += "\n" + line;
			}
			file_stream.close();
		}
		return contents;
	}

	GLuint Shader::LoadTransformShader(char* path){
		spdlog::info("Compiling Transform Shaders");

		GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);

		string vert_shader_code = LoadFileFromDisk(path);
		spdlog::info("Transform Shaders read from file");

		GLint compile_result = GL_FALSE; int info_log_size;

		// Compile Shader
		spdlog::info("Compiling Shader: \"{0}\"", path);
		char const * vert_code_pntr = vert_shader_code.c_str();
		glShaderSource(vert_shader_id, 1, &vert_code_pntr , nullptr);
		glCompileShader(vert_shader_id);

		// Check Shader
		glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &compile_result);
		glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
		vector<char> vertex_shader_error_message(info_log_size);
		glGetShaderInfoLog(vert_shader_id, info_log_size, nullptr, &vertex_shader_error_message[0]);
		
		if (!compile_result) {
			spdlog::error("Transform Shader Error: {0}", &vertex_shader_error_message[0]);
		}
		spdlog::info("Transform Shader compiled and checked");

		// Link the program
		spdlog::info("Linking Transform Shader program");
		GLuint program_id = glCreateProgram();
		glAttachShader(program_id, vert_shader_id);

		const GLchar* varyings[4];
		varyings[0] = "out_pos";
		varyings[1] = "out_vel";
		varyings[2] = "out_age";

		glTransformFeedbackVaryings(program_id, 3, varyings, GL_SEPARATE_ATTRIBS);

		glLinkProgram(program_id); 
		glUseProgram(program_id);

		// Check the program
		glGetProgramiv(program_id, GL_LINK_STATUS, &compile_result);
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_size);
		vector<char> program_error_message(std::max(info_log_size, int(1)) );
		glGetProgramInfoLog(program_id, info_log_size, nullptr, &program_error_message[0]);
		
		if (!compile_result) {
			spdlog::error("Transform Shader Program error: {0}", &program_error_message[0]);
		}
			spdlog::info("Transform Shader Program created and checked");

		glDeleteShader(vert_shader_id);

		return program_id;
	}
}