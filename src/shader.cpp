#include "cad-base/shader.hpp"
#include <iostream>
#include <ostream>

using std::string;
using std::vector;

GLuint shader::LoadShaders(char* vertex_file_path, char* fragment_file_path) {
	puts("Compiling Shaders");

	GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	string vert_shader_code = LoadFileFromDisk(vertex_file_path);
	string frag_shader_code = LoadFileFromDisk(fragment_file_path);
	puts("Shaders Read from File");

	GLint result = GL_FALSE; int info_log_size;

	// Compile Vertex Shader
	printf("Compiling shader: %s\n", vertex_file_path);
	char const * vert_code_pntr = vert_shader_code.c_str();
	glShaderSource(vert_shader_id, 1, &vert_code_pntr , nullptr);
	glCompileShader(vert_shader_id);

	// Check Vertex Shader
	glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
	vector<char> vertex_shader_error_message(info_log_size);
	glGetShaderInfoLog(vert_shader_id, info_log_size, nullptr, &vertex_shader_error_message[0]);
	
	if (&vertex_shader_error_message[0]) {
		fprintf(stdout, "%s\n", &vertex_shader_error_message[0]);
	}
	puts("Vertex Shader Compiled and Checked");


	// Compile Fragment Shader
	printf("Compiling shader: %s\n", fragment_file_path);
	char const *fragment_source_pointer = frag_shader_code.c_str();
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer , nullptr);
	glCompileShader(fragment_shader_id);

	// Check Fragment Shader
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
	vector<char> fragment_shader_error_message(info_log_size);
	glGetShaderInfoLog(fragment_shader_id, info_log_size, nullptr, &fragment_shader_error_message[0]);

	if (&fragment_shader_error_message[0]) {
		fprintf(stdout, "%s\n", &fragment_shader_error_message[0]);
	}
	puts("Fragment Shader Compiled and Checked");

	// Link the program
	puts("Linking program");
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vert_shader_id);
	glAttachShader(program_id, fragment_shader_id);

	glLinkProgram(program_id); 
	glUseProgram(program_id);

	// Check the program
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_size);
	vector<char> program_error_message(std::max(info_log_size, int(1)) );
	glGetProgramInfoLog(program_id, info_log_size, nullptr, &program_error_message[0]);
	
	if (&program_error_message[0]) {
		fprintf(stdout, "%s\n", &program_error_message[0]);
	}

	glDeleteShader(vert_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

string shader::LoadFileFromDisk(char* path) {
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

GLuint shader::LoadTransformShader(char* path){
	puts("Compiling Shaders");

	GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);

	string vert_shader_code = LoadFileFromDisk(path);
	puts("Shader Read from File");

	GLint result = GL_FALSE; int info_log_size;

	// Compile Vertex Shader
	printf("Compiling shader: %s\n", path);
	char const * vert_code_pntr = vert_shader_code.c_str();
	glShaderSource(vert_shader_id, 1, &vert_code_pntr , nullptr);
	glCompileShader(vert_shader_id);

	// Check Vertex Shader
	glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &info_log_size);
	vector<char> vertex_shader_error_message(info_log_size);
	glGetShaderInfoLog(vert_shader_id, info_log_size, nullptr, &vertex_shader_error_message[0]);
	
	if (&vertex_shader_error_message[0]) {
		fprintf(stdout, "%s\n", &vertex_shader_error_message[0]);
	}
	puts("Vertex Shader Compiled and Checked");

	// Link the program
	puts("Linking program");
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
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_size);
	vector<char> program_error_message(std::max(info_log_size, int(1)) );
	glGetProgramInfoLog(program_id, info_log_size, nullptr, &program_error_message[0]);
	
	if (&program_error_message[0]) {
		fprintf(stdout, "%s\n", &program_error_message[0]);
	}

	glDeleteShader(vert_shader_id);

	return program_id;
}
