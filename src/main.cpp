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

#include <nfd.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/mesh.h"
#include "cad-base/shader.hpp"
#include "cad-base/geometry/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/viewport.hpp"
#include "cad-base/geometry_list.hpp"

#include "cad-base/gui/gui_project.hpp"
#include "cad-base/gui/gui_render_window.hpp"


using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

auto old_time = std::chrono::steady_clock::now(), new_time = std::chrono::steady_clock::now();
double delta_t;	

const float INITIAL_SCALE_FACTOR = 2.0f;
float scale_factor = INITIAL_SCALE_FACTOR;

unique_ptr<ImGuiIO> imgui_io;

GLFWwindow* glfw_window;

unique_ptr<GeometryList> master_geometry;
shared_ptr<Geometry> loaded_geometry;

// TODO: Do we still need this to be a shared ptr to a vector? What was the reasoning for this?
// Geometry_list needs this list to add and remove geo from them.
// TODO: We may wish to rename 'Viewport' as IMGUI now has such a concept.
shared_ptr<vector<shared_ptr<Viewport>>> viewports;

//GUI
unique_ptr<GuiProject> gui_settings;
vector<shared_ptr<GuiRenderWindow>> gui_render_windows;
bool show_demo_window = false;

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

const ImVec4 BACKGROUND_COLOUR = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

bool ImportGeoTest( const std::string& pFile);  //TODO: temp prototype.

void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {    //NOLINT: unused params in callback.
    // ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if(ImGui::GetIO().WantCaptureMouse)  return;

    puts("Mouse button not captured by IMGUI");

    // double x,y;
    // glfwGetCursorPos(window, &x, &y);
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

    // Create window with graphics context
    const GLFWvidmode* glfwvidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfw_window = glfwCreateWindow(glfwvidmode->width, glfwvidmode->height, "CAD-BASE", nullptr, nullptr);
    if (glfw_window == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(glfw_window);

    // glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, GL_TRUE);
    // glfwSetKeyCallback(glfw_window, input_router->KeyCallback);
	glfwSetMouseButtonCallback(glfw_window, GlfwMouseButtonCallback);
	// glfwSetScrollCallback(glfw_window, input_router->ScrollCallback);
	// glfwSetCursorPosCallback(glfw_window, input_router->CursorCallback);
	// glfwSetWindowSizeCallback(glfw_window, input_router->WindowSizeCallback);

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

bool SetupImgui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO(); (void)imgui_io;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // TODO: Currently broken on Linux, needs further investigation
    // imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(scale_factor);
    // ImGui::SetCurrentFont(ImFont *font)
    ImGui::GetStyle().WindowMinSize = ImVec2(400, 400);

    ImFontConfig cfg;
    cfg.SizePixels = 13 * scale_factor; // NOLINT: 13 is the intended size for this font.
    ImGui::GetIO().Fonts->AddFontDefault(&cfg);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void SetupGuiMainMenu() {   //NOLINT: Nesting is easy to understand.
    if(ImGui::BeginMainMenuBar())   {
        if (ImGui::BeginMenu("File"))   {

            //Disable these for now.
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

            if(ImGui::MenuItem("New file"))  {
                //Do something
            }
            if(ImGui::MenuItem("Save file"))  {
                //Do something
            }
            if(ImGui::MenuItem("Load file"))  {

            }

            ImGui::PopItemFlag();
            ImGui::PopStyleVar();


            if(ImGui::MenuItem("Import Mesh"))  {
                nfdchar_t *out_path = NULL;
                //TODO this doesn't work when ran in vscode debugger? 
                //Says "zenity not installed"
                nfdresult_t result = NFD_OpenDialog(NULL, NULL, &out_path);
                    
                if ( result == NFD_OKAY ) {
                    puts("Success!");
                    ImportGeoTest(out_path);
                    free(out_path);
                }
                else if ( result == NFD_CANCEL ) {
                    puts("User pressed cancel.");
                }
                else {
                    printf("Error: %s\n", NFD_GetError() );
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Preferences"))   {
            if(ImGui::MenuItem("Thing1"))  {
                //Do something
            }
            if(ImGui::MenuItem("Thing2"))  {
                //Do something
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))   {
            if(ImGui::MenuItem("Show Dear ImGUI Demo"))  {
                show_demo_window = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))   {
                if(ImGui::MenuItem("About"))  {
                    //Do something
                }
                if(ImGui::MenuItem("Github"))  {
                    //Do something
                }
                ImGui::EndMenu();
            }
    }
        
    ImGui::EndMainMenuBar();
}

void SetupGuiTheme() {
    //Set some global style settings
    //Todo: load this from a struct?
    ImGui::GetStyle().WindowPadding = ImVec2(5.0f, 5.0f);
    ImGui::GetStyle().FrameBorderSize = 0;
    ImGui::GetStyle().PopupBorderSize = 0;

    const ImVec4 accent_colour_primary   = ImVec4(0.160f, 0.204f, 0.204f, 1.000f); //Standard highlight colour, used for active tabs etc
    const ImVec4 accent_colour_secondary = ImVec4(0.25f, 0.38f, 0.36f, 1.00f); //Less important highlight colour, used for inactive tabs etc
    const ImVec4 accent_colour_highlight = ImVec4(0.11f, 0.58f, 0.51f, 1.00f); //Brighter, used for hovering etc
    const ImVec4 accent_colour_undefined = ImVec4(1.00f, 0.43f, 0.35f, 1.00f); //A highly contrasting colour used to show elements not themed yet.

    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.44f, 0.44f, 0.44f, 0.60f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.57f, 0.57f, 0.57f, 0.70f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.76f, 0.76f, 0.76f, 0.80f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.141f, 0.141f, 0.141f, 1.000f);

    colors[ImGuiCol_PopupBg]                = accent_colour_primary;
    colors[ImGuiCol_CheckMark]              = accent_colour_primary;
    colors[ImGuiCol_SliderGrab]             = accent_colour_primary;
    colors[ImGuiCol_SliderGrabActive]       = accent_colour_secondary;
    colors[ImGuiCol_Button]                 = accent_colour_primary;
    colors[ImGuiCol_ButtonHovered]          = accent_colour_highlight;
    colors[ImGuiCol_ButtonActive]           = accent_colour_secondary;
    colors[ImGuiCol_Header]                 = accent_colour_primary;
    colors[ImGuiCol_HeaderHovered]          = accent_colour_highlight;
    colors[ImGuiCol_HeaderActive]           = accent_colour_secondary;
    colors[ImGuiCol_SeparatorHovered]       = accent_colour_highlight;
    colors[ImGuiCol_SeparatorActive]        = accent_colour_secondary;
    colors[ImGuiCol_ResizeGrip]             = accent_colour_primary;
    colors[ImGuiCol_ResizeGripHovered]      = accent_colour_highlight;
    colors[ImGuiCol_ResizeGripActive]       = accent_colour_secondary;
    colors[ImGuiCol_Tab]                    = accent_colour_primary;
    colors[ImGuiCol_TabHovered]             = accent_colour_highlight;
    colors[ImGuiCol_TabActive]              = accent_colour_secondary;
    colors[ImGuiCol_TabUnfocused]           = accent_colour_primary;
    colors[ImGuiCol_TabUnfocusedActive]     = accent_colour_secondary;
    colors[ImGuiCol_DockingPreview]         = accent_colour_primary;
    colors[ImGuiCol_DockingEmptyBg]         = accent_colour_undefined;
    colors[ImGuiCol_PlotLines]              = accent_colour_undefined;
    colors[ImGuiCol_PlotLinesHovered]       = accent_colour_highlight;
    colors[ImGuiCol_PlotHistogram]          = accent_colour_undefined;
    colors[ImGuiCol_PlotHistogramHovered]   = accent_colour_highlight;
    colors[ImGuiCol_TableHeaderBg]          = accent_colour_undefined;
    colors[ImGuiCol_TableBorderStrong]      = accent_colour_undefined;
    colors[ImGuiCol_TableBorderLight]       = accent_colour_undefined;
    colors[ImGuiCol_TableRowBg]             = accent_colour_undefined;
    colors[ImGuiCol_TableRowBgAlt]          = accent_colour_undefined;
    colors[ImGuiCol_TextSelectedBg]         = accent_colour_undefined;
    colors[ImGuiCol_DragDropTarget]         = accent_colour_undefined;
    colors[ImGuiCol_NavHighlight]           = accent_colour_undefined;
    colors[ImGuiCol_NavWindowingHighlight]  = accent_colour_secondary;
    colors[ImGuiCol_NavWindowingDimBg]      = accent_colour_undefined;
    colors[ImGuiCol_ModalWindowDimBg]       = accent_colour_undefined;
}

void SetupGui() {
    // Pass through input.
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each other.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    // Setup window to match screen size/pos.
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    SetupGuiMainMenu();
    
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    //Set some styles just for the root dockspace.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    // Setup Dockspace
    // TODO: Have this as an editable default (don't reset every boot unless user clicks a 'reset UI' button)
    imgui_io = make_unique<ImGuiIO>(ImGui::GetIO());
    if (imgui_io->ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static auto first_time = true;
        if (first_time) {
            first_time = false;

            SetupGuiTheme();

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            ImGuiID dock_id_left_pane, dock_id_grid, dock_id_grid_top_left, dock_id_grid_bottom_left, dock_id_grid_top_right, dock_id_grid_bottom_right;
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, &dock_id_left_pane, &dock_id_grid);

            //Construct the grid.
            ImGui::DockBuilderSplitNode(dock_id_grid, ImGuiDir_Up, 0.5f, &dock_id_grid_top_left, &dock_id_grid_bottom_left);
            ImGui::DockBuilderSplitNode(dock_id_grid_top_left, ImGuiDir_Left, 0.5f, &dock_id_grid_top_left, &dock_id_grid_top_right);
            ImGui::DockBuilderSplitNode(dock_id_grid_bottom_left, ImGuiDir_Left, 0.5f, &dock_id_grid_bottom_left, &dock_id_grid_bottom_right);

            //Auto hide viewport tab bars.
            ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_grid_top_left);
            node->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar; 
            node = ImGui::DockBuilderGetNode(dock_id_grid_top_right);
            node->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;
            node = ImGui::DockBuilderGetNode(dock_id_grid_bottom_left);
            node->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;
            node = ImGui::DockBuilderGetNode(dock_id_grid_bottom_right);
            node->LocalFlags |= ImGuiDockNodeFlags_AutoHideTabBar;

            // we now dock our windows into the docking node we made above
            ImGui::DockBuilderDockWindow("Project", dock_id_left_pane);
            ImGui::DockBuilderDockWindow("Dear Imgui Demo", dock_id_left_pane);
            ImGui::DockBuilderDockWindow("Render Window 0", dock_id_grid_top_left);
            ImGui::DockBuilderDockWindow("Render Window 1", dock_id_grid_top_right);
            ImGui::DockBuilderDockWindow("Render Window 2", dock_id_grid_bottom_left);
            ImGui::DockBuilderDockWindow("Render Window 3", dock_id_grid_bottom_right);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    ImGui::End();
}

bool ImportGeoTest( const std::string& pFile) {
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
        puts(importer.GetErrorString());
        return false;
    }

    vector<vec3> test_geo;
    float import_scale_factor = 0.05f;

    for (int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh =  scene->mMeshes[m];

        for (int f = 0; f < mesh->mNumFaces; f++) {
            for (int i = 0; i < mesh->mFaces->mNumIndices; i++) {
                int faceIndex = mesh->mFaces[f].mIndices[i];

                test_geo.push_back(glm::vec3(mesh->mVertices[faceIndex].x, mesh->mVertices[faceIndex].y, mesh->mVertices[faceIndex].z) * import_scale_factor);
            }
        }
    }

    //Unload old geo.
    if (loaded_geometry != nullptr) {
        loaded_geometry->is_dead = true;
        loaded_geometry = nullptr;
    }

    loaded_geometry = make_shared<Geometry>(test_geo);
	master_geometry->push_back(loaded_geometry);

    return true;
}

void SetupTestGeo() {
    // TODO: temp test.
    for (int i= 0; i < 1; i++) { 
        viewports->push_back(make_shared<Viewport>(glfw_window, glm::vec3(BACKGROUND_COLOUR.x, BACKGROUND_COLOUR.y, BACKGROUND_COLOUR.z), 1000, 1000));

        // Make our render windows.
        std::string name = "Render Window " + std::to_string(i);
        gui_render_windows.push_back(make_shared<GuiRenderWindow>(name, glfw_window, viewports->back()));
    }

	// master_geometry->push_back(make_shared<Geometry>(TEST_TRIANGLE_VERTS, TEST_TRIANGLE_COLS));
	// master_geometry->push_back(make_shared<Geometry>(test_data_lines, test_data_lines));

    ImportGeoTest("/home/tom/git/cad-base/thirdparty/assimp/test/models/Q3D/E-AT-AT.q3o");
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
    glDepthFunc(GL_LESS); // Depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);


    for (const auto& v : (*viewports)) {
        v->Update(delta_t);
    }

    auto geo_ptr = master_geometry->begin();
    while (geo_ptr != master_geometry->end()) {
        (*geo_ptr)->Update(delta_t);
        geo_ptr++;
    }

    // We don't want this when drawing UI elements
    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    SetupGui();
    
    // Render Windows
    for (const auto& r : (gui_render_windows)) {
        r->Draw(delta_t);
    }

    // Settings Window
    gui_settings->Draw(delta_t);

    if(show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    // Rendering
    ImGui::Render();
    
    int display_w;
    int display_h;
    glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(BACKGROUND_COLOUR.x * BACKGROUND_COLOUR.w, BACKGROUND_COLOUR.y * BACKGROUND_COLOUR.w, BACKGROUND_COLOUR.z * BACKGROUND_COLOUR.w, BACKGROUND_COLOUR.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(glfw_window);

    int xpos, ypos;
    glfwGetWindowPos(glfw_window, &xpos, &ypos);

    // Update and Render additional Platform Windows
    if (imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

int main(int argc, const char* argv[]) { // NOLINT: main function.
    puts("Launching Program");

    std::srand(time(NULL));

    if (!SetupGLFW()) {
        return -1;
    }

    viewports = make_shared<vector<shared_ptr<Viewport>>>();
	master_geometry = make_unique<GeometryList>(viewports);

    gui_settings = make_unique<GuiProject>("Project", glfw_window, viewports);

    SetupTestGeo();

    if (!SetupImgui()) {
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(glfw_window))
    {
        Update();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(glfw_window);
    glfwTerminate();

    puts("Terminating Program");    
    return 0;
}
