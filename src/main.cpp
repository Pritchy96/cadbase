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

#include "cad-base/gui/gui_main.hpp"
#include "cad-base/shader.hpp"
#include "cad-base/geometry/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/viewport.hpp"
#include "cad-base/geometry_list.hpp"
#include "cad-base/gui/gui_logger.hpp"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

const ImVec4 BACKGROUND_COLOUR = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);    //TODO: remove

auto old_time = std::chrono::steady_clock::now(), new_time = std::chrono::steady_clock::now();
double delta_t;	

unique_ptr<GuiMain> gui_main;
unique_ptr<GeometryList> master_geometry;

shared_ptr<GuiLogger> gui_logger_sink;

GLFWwindow* glfw_window;

// TODO: We may wish to rename 'Viewport' as IMGUI now has such a concept.
shared_ptr<vector<shared_ptr<Viewport>>> viewports;

const vector<vec3> TEST_DATA_LINES = {
	glm::vec3(00.0, 00.0, 00.0),
	glm::vec3(20.0, 00.0, 00.0),
	glm::vec3(00.0, 20.0, 00.0),

	glm::vec3(00.0, 00.0, 00.0),
	glm::vec3(00.0, 00.0, 20.0), 
	glm::vec3(20.0, 00.0, 00.0),

	glm::vec3(00.0, 00.0, 00.0),
	glm::vec3(00.0, 20.0, 00.0),
	glm::vec3(00.0, 00.0, 20.0)	
};

const vector<vec3> TEST_TRIANGLE_VERTS = {
    vec3(-10.0f, -10.0f, 0.0f),
    vec3(10.0f, -10.0f, 0.0f),
    vec3(0.0f,  10.0f, 0.0f)
};

const vector<vec3> TEST_TRIANGLE_COLS = {
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f,  0.0f, 1.0f)
};

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

    vector<vec3> test_geo;
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

	master_geometry->push_back(make_shared<Geometry>(test_geo, glm::vec3((rand() % 1000) - 500, (rand() % 1000) - 500, (rand() % 1000) - 500)));
    return true;
}

void SetupRenderWindows() {
    // TODO: temp test.
    for (int i= 0; i < 4; i++) { 
        viewports->push_back(make_shared<Viewport>(glfw_window, glm::vec3(gui_main->BACKGROUND_COLOUR.x, gui_main->BACKGROUND_COLOUR.y, gui_main->BACKGROUND_COLOUR.z), 1000, 1000));

        // Make our render windows - one for each viewport for now.
        std::string name = "Render Window " + std::to_string(i);
        gui_main->gui_render_windows.push_back(make_shared<GuiRenderWindow>(name, glfw_window, viewports->back()));
    }
}

void Update() {
    old_time = new_time;
    new_time = std::chrono::steady_clock::now();
    delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(new_time - old_time).count();

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the imgui_io.WantCaptureMouse, imgui_io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When imgui_io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When imgui_io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    glEnable(GL_DEPTH_TEST); // Enable depth-testing
    glEnable(GL_CULL_FACE);

    for (const auto& v : (*viewports)) {
        v->Update(delta_t);
    }

    auto geo_ptr = master_geometry->begin();
    while (geo_ptr != master_geometry->end()) {
        (*geo_ptr)->Update(delta_t);
        geo_ptr++;
    }

    gui_main->Update(delta_t);
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
	master_geometry = make_unique<GeometryList>(viewports);    
    gui_main = make_unique<GuiMain>(glfw_window, gui_logger_sink); 

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
