#include "cad-base/viewport.hpp"	
#include "cad-base/geometry/viewport_grid.hpp"

#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

const vector<vec3> AXIS_LINES = {
    vec3(0.0f, 0.0f, 0.0f),	// x
	vec3(100.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, 0.0f),	// y	
	vec3(0.0f, 100.0f, 0.0f), 
	vec3(0.0f, 0.0f, 0.0f),	// z
	vec3(0.0f, 0.0f, 100.0f),
};

const vector<vec3> AXIS_COLOURS = {
    vec3(1.0f, 0.0f, 0.0f),	// x
	vec3(1.0f, 0.0f, 0.0f),	
	vec3(0.0f, 1.0f, 0.0f), // y
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, 0.0f, 1.0f),	// z
	vec3(0.0f, 0.0f, 1.0f)
};

Viewport::Viewport(GLFWwindow *window, glm::vec3 background_col, int window_width, int window_height) {

    puts("Initialised Viewport");
	glfw_window = window;
	window_width_ = window_width;
	window_height_ = window_height;
	background_colour = background_col;

	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 10.0f);

	// TODO: Move everything dependent on this (gl calls, shader loads etc) to an init() function so this context setting can be done 
	// in main.cpp
	glfwMakeContextCurrent(glfw_window);

	// TODO: file paths are currently relative to excution path, not main location.
	// TODO: load these once, keep in static file?
	basic_shader = shader::LoadShaders((char*)"./shaders/basic_camera.vertshader", (char*)"./shaders/basic_camera.fragshader");

    SetupFBO();

    render_axis = make_shared<Geometry>(AXIS_LINES, AXIS_COLOURS);
	viewport_geo_renderable_pairs.emplace_back(render_axis, make_unique<Renderable>(basic_shader, render_axis, GL_LINES));

    grid = make_shared<ViewportGrid>(50, 50, 20, 20, glm::vec3(0.3f, 0.3f, 0.3f), basic_shader);
	viewport_geo_renderable_pairs.emplace_back(grid, make_unique<Renderable>(basic_shader, grid, GL_LINES));
}

void Viewport::SetupTransformShader(GLuint transformShader) {
	t_shader = transformShader;
}

void Viewport::Update(double deltaTime) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(background_colour.r, background_colour.g, background_colour.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(render_face, render_mode);

    //Render master Geo.
    auto geo_renderable = master_geo_renderable_pairs.begin();

    while(geo_renderable != master_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = master_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
        }

        renderable->Draw(deltaTime, camera->GetProjectionMatrix(), camera->GetViewMatrix());
        ++geo_renderable;
    }

    //Render viewport specific stuff.
    geo_renderable = viewport_geo_renderable_pairs.begin();

    while(geo_renderable != viewport_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = viewport_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
        }

        renderable->Draw(deltaTime, camera->GetProjectionMatrix(), camera->GetViewMatrix());
        ++geo_renderable;
    }

    // Bind default framebuffer again.
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewport::SetupFBO() {
    //Colour texture
	glGenTextures(1, &colour_texture);
	glBindTexture(GL_TEXTURE_2D, colour_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA,GL_UNSIGNED_BYTE , 0);
	glBindTexture(GL_TEXTURE_2D, 0);

    // Depth texture - Slower than depth buffer, but can sample it later in shader
    // TODO: If we don't end up using this texture and neeed a fps boost, we can change this to a buffer
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //Attach textures to FBO context.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    // Check that framebuffer is ok
    CheckFramebufferStatus(fbo);

    // Unbind framebuffer (bind default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Viewport::CheckFramebufferStatus(GLuint fbo) {
    // check FBO status
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))  {
        case GL_FRAMEBUFFER_COMPLETE:
            std::cout << "Framebuffer complete." << std::endl;
            return true;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
            return false;
        /*
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
            std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
            return false;
        */
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "[ERROR] Framebuffer incomplete: Multisample." << std::endl;
            return false;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
            return false;

        default:
            std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
            return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// void Viewport::WindowSizeCallback(GLFWwindow* glfw_window, int width, int height) {
//  	this->window_height_ = height;
// 	this->window_width_ = width;
// }