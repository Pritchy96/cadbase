#include "cad-base/navicube.hpp"	

#include <spdlog/spdlog.h>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

NaviCube::NaviCube(GLFWwindow *window, glm::vec4 background_col, int window_width, int window_height) {
    spdlog::info("Viewport Initialised");
	glfw_window_ = window;
	window_size = make_shared<glm::vec2>(window_width, window_height);
	background_colour = background_col;

	camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), 200.0f, window_size);
    camera->SetProjectionStyle(true);

	// TODO: Move everything dependent on this (gl calls, shader loads etc) to an init() function so this context setting can be done 
	// in main.cpp
	glfwMakeContextCurrent(glfw_window_);

	// TODO: file paths are currently relative to excution path, not main location.
	// TODO: load these once, keep in static file?
	basic_shader = shader::LoadShaders((char*)"./shaders/basic_camera.vertshader", (char*)"./shaders/basic_camera.fragshader");

    shared_ptr<Geometry> geo = make_shared<Geometry>(face_1_, "Face 1");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_2_, "Face 2");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));
    
    geo = make_shared<Geometry>(face_3_, "Face 3");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_4_, "Face 4");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_5_, "Face 5");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    geo = make_shared<Geometry>(face_6_, "Face 6");
	navicube_geo_renderable_pairs.emplace_back(geo, make_unique<Renderable>(basic_shader, geo, GL_TRIANGLES));

    SetupFBO();
}

void NaviCube::Update(double deltaTime) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glEnable(GL_DEPTH_TEST); // Enable depth-testing
    // glEnable(GL_CULL_FACE);

    glViewport(0, 0, window_size->x, window_size->y);
    glClearColor(background_colour.r, background_colour.g, background_colour.b, background_colour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(render_face, render_mode);

    //Render master Geo.
    auto geo_renderable = navicube_geo_renderable_pairs.begin();

    while(geo_renderable != navicube_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = navicube_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        renderable->Draw(deltaTime, camera->GetProjectionMatrix(), camera->GetViewMatrix());

        //Do this after drawing so geo updated this frame is loaded into the VAO 
        //(Geo is updated after renderables are drawn above)
        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
            renderable->valid_aa_bounding_box_vao = false;
        }
        ++geo_renderable;
    }

    //Render debug Geo.
    geo_renderable = debug_geo_renderable_pairs.begin();

    while(geo_renderable != debug_geo_renderable_pairs.end()) {
        // Geo is dead, nuke the map link
        if (geo_renderable->first->is_dead) {
            // iterator.erase gives the next item in the list.
            geo_renderable = debug_geo_renderable_pairs.erase(geo_renderable);
            continue;
        }

        if (geo_renderable->second == nullptr) {
            // Renderable for geo doesn't exist, make one.
            // TODO: Some logic to choose a render type? (currently default to GL_TRIANGLES)
            geo_renderable->second = make_unique<Renderable>(basic_shader, geo_renderable->first, GL_TRIANGLES);
        }

        shared_ptr<Geometry> geometry = geo_renderable->first;
        shared_ptr<Renderable> renderable = geo_renderable->second;

        renderable->Draw(deltaTime, camera->GetProjectionMatrix(), camera->GetViewMatrix());

        //Do this after drawing so geo updated this frame is loaded into the VAO 
        //(Geo is updated after renderables are drawn above)
        if (geometry->buffers_invalid) {
            renderable->valid_geometry_vao = false;
            renderable->valid_aa_bounding_box_vao = false;
        }
        ++geo_renderable;
    }


    // Bind default framebuffer again.
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST); // Disable depth testing for rest of UI.
    glDisable(GL_CULL_FACE);
}

void NaviCube::SetupFBO() {
    //Colour texture
	glGenTextures(1, &colour_texture);
	glBindTexture(GL_TEXTURE_2D, colour_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window_size->x, window_size->y, 0, GL_RGBA,GL_UNSIGNED_BYTE , 0);
	glBindTexture(GL_TEXTURE_2D, 0);

    // Depth texture - Slower than depth buffer, but can sample it later in shader
    // TODO: If we don't end up using this texture and neeed a fps boost, we can change this to a buffer
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_size->x, window_size->y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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

bool NaviCube::CheckFramebufferStatus(GLuint fbo) {
    // check FBO status
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))  {
        case GL_FRAMEBUFFER_COMPLETE:
            spdlog::info("Framebuffer construction complete.");
            return true;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            spdlog::error("Framebuffer incomplete: Attachment is NOT complete.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            spdlog::error("Framebuffer incomplete: No image is attached to FBO.");
            return false;
        /*
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            spdlog::error("Framebuffer incomplete: Attached images have different dimensions.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
            spdlog::error("Framebuffer incomplete: Color attached images have different internal formats.");
            return false;
        */
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            spdlog::error("Framebuffer incomplete: Draw buffer.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            spdlog::error("Framebuffer incomplete: Read buffer.");
            return false;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            spdlog::error("Framebuffer incomplete: Multisample.");
            return false;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            spdlog::error("Framebuffer incomplete: Unsupported by FBO implementation.");
            return false;

        default:
            spdlog::error("Framebuffer incomplete: Unknown error.");
            return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}