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

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/mesh.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cad_gui/opengl/shader.hpp"
#include "cad_gui/opengl/render_data_types/geometry/geometry.hpp"
#include "cad_gui/opengl/render_data_types/renderable/renderable.hpp"
#include "cad_gui/scene_data.hpp"
#include "cad_gui/imgui/imgui_windows/log_window.hpp"
#include "cad_gui/imgui/app_style.hpp"
#include "cad_gui/imgui/imgui_windows/main_window.hpp"
#include "cad_gui/imgui/imgui_windows/viewport_window/viewport.hpp"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

const ImVec4 BACKGROUND_COLOUR = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);    //TODO: remove

unique_ptr<CadGui::MainWindow> main_window;
shared_ptr<CadGui::SceneData> scene_data;
shared_ptr<CadGui::LogWindow> log_window_sink;

CadGui::AppStyle app_style;

GLFWwindow* glfw_window;

// TODO: We may wish to rename 'CadGui::Viewport' as IMGUI now has such a concept.
shared_ptr<vector<shared_ptr<CadGui::Viewport>>> viewports;

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

bool ImportGeoTest(const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    //check if file exists
    std::ifstream fin(pFile.c_str());
    if(!fin.fail()) {
        fin.close();
    }
    else{
        printf("Couldn't open file: %s\n", pFile.c_str());
        printf("%s\n", importer.GetErrorString());
        return false;
    }

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( pFile,
        aiProcess_CalcTangentSpace       |
        aiProcess_Triangulate            |
        aiProcess_JoinIdenticalVertices);

    // If the import failed, report it
    if (nullptr == scene) {
        spdlog::error("Failed to Import Mesh: {0}", importer.GetErrorString());
        return false;
    }

    vector<glm::vec3> test_geo;
    float import_scale_factor = 1.0f;

    for (int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh =  scene->mMeshes[m];

        for (int f = 0; f < mesh->mNumFaces; f++) {
            for (int i = 0; i < mesh->mFaces->mNumIndices; i++) {
                int face_index = mesh->mFaces[f].mIndices[i];

                test_geo.push_back(glm::vec3(mesh->mVertices[face_index].x, mesh->mVertices[face_index].y, mesh->mVertices[face_index].z) * import_scale_factor);
            }
        }
    }

	scene_data->MasterGeoPushBack(make_shared<CadGui::Geometry>(test_geo, "Test Geo " + std::to_string(rand()), glm::vec3((rand() % 1000) - 500, (rand() % 1000) - 500, (rand() % 1000) - 500)));
    return true;
}

void SetupRenderWindows() {
    // TODO: temp test.
    for (int i= 0; i < 4; i++) { 
        viewports->push_back(make_shared<CadGui::Viewport>(glfw_window, 
            glm::vec4(app_style.BACKGROUND_COLOUR_MEDIUM.x, app_style.BACKGROUND_COLOUR_MEDIUM.y, 
                app_style.BACKGROUND_COLOUR_MEDIUM.z, app_style.BACKGROUND_COLOUR_MEDIUM.w),
            1000, 1000, scene_data));

        // Make our render windows - one for each cadgui::viewport for now.
        std::string name = "Render Window " + std::to_string(i);
        main_window->gui_render_windows.push_back(make_shared<CadGui::ViewportWindow>(name, glfw_window, viewports->back()));
    }
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

    auto geo_ptr = scene_data->MasterGeoBegin();
    while (geo_ptr != scene_data->MasterGeoEnd()) {
        (*geo_ptr)->Update();
        geo_ptr++;
    }

    main_window->Update();
}

void SetupLogger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    log_window_sink = make_shared<CadGui::LogWindow>();
    spdlog::sinks_init_list sink_list = {log_window_sink, console_sink};

    auto logger = make_shared<spdlog::logger>("logger", sink_list);
    spdlog::set_default_logger(logger); //Means that when we do spdlog::info/warn etc it goes to this logger.
}

int main(int argc, const char* argv[]) { // NOLINT: main function.
    SetupLogger();

    std::srand(time(nullptr));

    if (!SetupGLFW()) {
        return -1;
    }

    viewports = make_shared<vector<shared_ptr<CadGui::Viewport>>>();
	scene_data = make_shared<CadGui::SceneData>(viewports);    
    main_window = make_unique<CadGui::MainWindow>(glfw_window, log_window_sink, scene_data); 

    SetupRenderWindows();

    //TODO: Temp tests.
    for (int i = 0; i < 10; i++) {
        ImportGeoTest("/home/tom/git/cad-base/thirdparty/assimp/test/models/OBJ/spider.obj");
    }

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
