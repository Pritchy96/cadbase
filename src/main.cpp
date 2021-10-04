#include <cstdio>
#include <chrono>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cad-base/shader.hpp"
#include "cad-base/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/viewport.hpp"
#include "cad-base/geometry_list.hpp"
#include "imgui_internal.h"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

auto old_time = std::chrono::steady_clock::now(), new_time = std::chrono::steady_clock::now();
long delta_t;	

float scale_factor = 2.0f;

unique_ptr<GeometryList> master_geometry;
shared_ptr<vector<shared_ptr<Viewport>>> viewports;

const vector<vec3> TEST_TRIANGLE = {
    vec3(-1.0f, -1.0f, 0.0f),
    vec3(1.0f, -1.0f, 0.0f),
    vec3(0.0f,  1.0f, 0.0f)
};

// Our state
bool show_alt_renderer = false;
ImVec4 background_colour = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); //NOLINT dumb.

int main(int argc, const char* argv[]) { //NOLINT: main function.
    std::cout << "Launching Program" << std::endl;

    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MacOS compat
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 
    // Create window with graphics context
    const GLFWvidmode* glfwvidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(glfwvidmode->width, glfwvidmode->height, "CAD-BASE", nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

	// Initialize GLEW
	glewExperimental = 1u; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

    viewports = make_shared<vector<shared_ptr<Viewport>>>();
	master_geometry = make_unique<GeometryList>(viewports);

    //TODO: temp test.
    viewports->push_back(make_shared<Viewport>(window, glm::vec3(background_colour.x, background_colour.y, background_colour.z), 4000, 4000));
    viewports->push_back(make_shared<Viewport>(window, glm::vec3(background_colour.x, background_colour.y, background_colour.z), 4000, 4000));
    
    //change some settings to differentiate the Viewports
    viewports->at(1)->background_colour = glm::vec3(0.0f, 1.0f, 0);
    // viewports->at(1)->camera->SetProjection(true);
    viewports->at(1)->camera->position = glm::vec3(2.0f, 10.0f, 10.0f);


	master_geometry->push_back(make_shared<Geometry>(TEST_TRIANGLE));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(scale_factor);
    // ImGui::SetCurrentFont(ImFont *font)

    ImFontConfig cfg;
    cfg.SizePixels = 13 * scale_factor; //NOLINT 13 is the intended size for this font.
    ImGui::GetIO().Fonts->AddFontDefault(&cfg);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        old_time = new_time;
    	new_time = std::chrono::steady_clock::now();
		delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(new_time - old_time).count();

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        for (const auto& v : (*viewports)) {
        	v->Update(delta_t);
		}

        auto geo_ptr = master_geometry->begin();
		while (geo_ptr != master_geometry->end()) {
			(*geo_ptr)->Update(delta_t);
			geo_ptr++;
		}

        //We don't want this when drawing UI elements
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        //1. Render Window
        {
            ImGui::Begin("GameWindow");
            // Using a Child allow to fill all the space of the window.
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            GLuint render_texture;

            if (!show_alt_renderer) {
                render_texture = viewports->at(0)->texture;
            } else {
                render_texture = viewports->at(1)->texture;
            }

            ImGui::Image((ImTextureID)render_texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
            ImGui::End();
        }
        
        // 2. Settings Window
        {
            ImGui::Begin("Settings!");
            ImGui::Checkbox("Alt Renderer", &show_alt_renderer);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(background_colour.x * background_colour.w, background_colour.y * background_colour.w, background_colour.z * background_colour.w, background_colour.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Terminating Program" << std::endl;    
    return 0;
}
