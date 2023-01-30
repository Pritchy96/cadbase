#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <string>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui_internal.h>

// #include <assimp/Importer.hpp>      // C++ importer interface
// #include <assimp/scene.h>           // Output data structure
// #include <assimp/postprocess.h>     // Post processing flags
// #include <assimp/mesh.h>


#include <opensimplexnoise/OpenSimplexNoise/OpenSimplexNoise.h>



#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cad-base/shader.hpp"
#include "cad-base/geometry/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/level_geo.hpp"
#include "cad-base/gui/gui_logger.hpp"
#include "cad-base/gui/app_style.hpp"
#include "cad-base/gui/gui_main.hpp"
#include "cad-base/gui/rendered_textures/viewport.hpp"
#include "cad-base/level_gen.hpp"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

unique_ptr<GuiMain> gui_main;
shared_ptr<LevelGeo> level_geo;
shared_ptr<LevelGen> level_gen;
shared_ptr<GuiLogger> gui_logger_sink;

AppStyle app_style;

GLFWwindow* glfw_window;


// TODO: We may wish to rename 'Viewport' as IMGUI now has such a concept.
shared_ptr<vector<shared_ptr<Viewport>>> viewports;

bool ImportGeoTest( const std::string& pFile);  //TODO: temp prototype.

void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {    //NOLINT: unused params in callback.
    if(ImGui::GetIO().WantCaptureMouse) {  
        return;
    }

    spdlog::warn("Mouse button not captured by IMGUI");
}

bool SetupGLFW() {
    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return false;
    }
        
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS compat
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glDepthFunc(GL_LESS); // Depth-testing interprets a smaller value as "closer"

    // Create window with graphics context
    const GLFWvidmode* glfwvidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfw_window = glfwCreateWindow(glfwvidmode->width, glfwvidmode->height, "CAD-BASE", nullptr, nullptr);
    if (glfw_window == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(glfw_window);

	glfwSetMouseButtonCallback(glfw_window, GlfwMouseButtonCallback);

    glfwSwapInterval(1); // Enable vsync

	// Initialize GLEW
	glewExperimental = 1u; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return false;
	}

    return true;
}





void SetupRenderWindows() {
        viewports->push_back(make_shared<Viewport>(glfw_window, 
            glm::vec4(app_style.BACKGROUND_COLOUR_MEDIUM.x, app_style.BACKGROUND_COLOUR_MEDIUM.y, 
                app_style.BACKGROUND_COLOUR_MEDIUM.z, app_style.BACKGROUND_COLOUR_MEDIUM.w),
            100, 100, level_geo));

        // Make our render windows - one for each viewport for now.
        std::string name = "Render Window";
        gui_main->gui_render_windows.push_back(make_shared<GuiRenderWindow>(name, glfw_window, viewports->back()));
}

void Update() {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the imgui_io.WantCaptureMouse, imgui_io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When imgui_io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When imgui_io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    glEnable(GL_DEPTH_TEST); // Enable depth-testing
    glEnable(GL_CULL_FACE);

    for (const auto& v : (*viewports)) {
        v->Update();
    }

    auto geo_ptr = level_geo->MasterGeoBegin();
    while (geo_ptr != level_geo->MasterGeoEnd()) {
        (*geo_ptr)->Update();
        geo_ptr++;
    }

    gui_main->Update();
}

void SetupLogger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    gui_logger_sink = make_shared<GuiLogger>();
    spdlog::sinks_init_list sink_list = {gui_logger_sink, console_sink};

    auto logger = make_shared<spdlog::logger>("logger", sink_list);
    spdlog::set_default_logger(logger); //Means that when we do spdlog::info/warn etc it goes to this logger.
}

int main(int argc, const char* argv[]) { // NOLINT: main function.
    SetupLogger();

    std::srand(time(nullptr));

    if (!SetupGLFW()) {
        return -1;
    }

    viewports = make_shared<vector<shared_ptr<Viewport>>>();
	level_geo = make_shared<LevelGeo>(viewports);    
	level_gen = make_shared<LevelGen>(level_geo);    
    gui_main = make_unique<GuiMain>(glfw_window, gui_logger_sink, level_geo); 

    SetupRenderWindows();

    level_gen->GenerateGeometry();

    // Main loop
    while (!glfwWindowShouldClose(glfw_window)) {
        Update();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(glfw_window);
    glfwTerminate();

    spdlog::info("Terminating Program");    
    return 0;
}
