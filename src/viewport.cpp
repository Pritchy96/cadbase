#include "cad-base/viewport.hpp"	
#include "cad-base/viewport_grid.hpp"

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
	vec3(500.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, 0.0f),	// y	
	vec3(0.0f, 500.0f, 0.0f), 
	vec3(0.0f, 0.0f, 0.0f),	// z
	vec3(0.0f, 0.0f, 500.0f)
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

    std::cout << "Initialised Viewport" << std::endl;
	glfw_window = window;
	window_width_ = window_width;
	window_height_ = window_height;
	background_colour = background_col;

	camera = new Camera(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));

	// Pass an empty mat4 to the tcs matrix, this means it treats every mouse input as screen coords i.e between -1 and 1 for x and y.
	// So we need to run all mouse input through conversion to go from 0:width/height to -1:1
	// arcball_camera = new ArcBall(camera, (window_width_* width_ratio), (window_height_ * height_ratio), glm::vec3(0, 0, 0), 0.9f);
		
	// input_handlers.push_back(arcball_camera);

	// TODO: Move everything dependent on this (gl calls, shader loads etc) to an init() function so this context setting can be done 
	// in main.cpp
	glfwMakeContextCurrent(glfw_window);

	// TODO: file paths are currently relative to excution path, not main location.
	// TODO: load these once, keep in static file?
	basic_shader = shader::LoadShaders((char*)"./shaders/basic_camera.vertshader", (char*)"./shaders/basic_camera.fragshader");

    // Framebuffer config
    // TODO: can we make a single fbo and then when iterating through each viewport, bind the texture and draw to it.
    // As opposed to making many fbo's with the texture permanently bound here.
    glGenFramebuffers(1, &fbo); // Generate fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind fbo to config it

    // Create and bind a colour attatchment texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4000, 4000, 0,GL_RGB, GL_UNSIGNED_BYTE, nullptr); // Setup empty texture

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // GLuint depthrenderbuffer;
    // glGenRenderbuffers(1, &depthrenderbuffer);
    
    // Depth buffer
    // glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 500, 500);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Check that framebuffer is ok
    // TODO throw error!
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer not ok!" << std::endl;
    }

    // Unbind framebuffer (bind default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shared_ptr<Geometry> render_axis = make_shared<Geometry>(AXIS_LINES, AXIS_COLOURS);
	
	// TODO: better to have a GeoList in the viewport with just (this) in it's renderable list?
	// geo_renderable_pairs.emplace_back(render_axis, make_unique<Renderable>(basic_shader, render_axis, GL_LINES));

    shared_ptr<ViewportGrid> grid = make_shared<ViewportGrid>(80, 80, 40, 40, basic_shader);
	
	// TODO: better to have a GeoList in the viewport with just (this) in it's renderable list?
	geo_renderable_pairs.emplace_back(grid, make_unique<Renderable>(basic_shader, grid, GL_LINES));
}

void Viewport::SetupTransformShader(GLuint transformShader) {
	t_shader = transformShader;
}

void Viewport::Update(double deltaTime) {
	glPolygonMode(render_face, render_mode);
    // Render opengl window to texture
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, 4000, 4000);
    glClearColor(background_colour.r, background_colour.g, background_colour.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    auto geo_renderable = geo_renderable_pairs.begin();

    while(geo_renderable != geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = geo_renderable_pairs.erase(geo_renderable);
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
            renderable->valid_vao = false;
        }

        renderable->Draw(deltaTime, camera->GetProjectionMatrix(), camera->GetViewMatrix());
        ++geo_renderable;
    }

    // Bind default framebuffer again.
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// void Viewport::WindowSizeCallback(GLFWwindow* glfw_window, int width, int height) {
//  	this->window_height_ = height;
// 	this->window_width_ = width;
// }