#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <string>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui_internal.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cad-base/shader.hpp"
#include "cad-base/geometry.hpp"
#include "cad-base/renderable.hpp"
#include "cad-base/viewport.hpp"
#include "cad-base/geometry_list.hpp"

#include "cad-base/gui/gui_settings.hpp"
#include "cad-base/gui/gui_render_window.hpp"

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

using glm::vec3;

auto old_time = std::chrono::steady_clock::now(), new_time = std::chrono::steady_clock::now();
double delta_t;	

float scale_factor = 2.0f;

unique_ptr<GeometryList> master_geometry;
// TODO: Do we still need this to be a shared ptr to a vector? What was the reasoning for this?
// Geometry_list needs this list to add and remove geo from them.
// TODO: We may wish to rename 'Viewport' as IMGUI now has such a concept.
shared_ptr<vector<shared_ptr<Viewport>>> viewports;
vector<shared_ptr<GuiRenderWindow>> gui_render_windows;

unique_ptr<GuiSettings> gui_settings;

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

ImVec4 background_colour = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // NOLINT dumb.

void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{   
    // ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if(ImGui::GetIO().WantCaptureMouse)  return;

    std::cout << "Mouse button not captured by IMGUI" << std::endl;

    // double x,y;
    // glfwGetCursorPos(window, &x, &y);
}

void SetupGui() {
        // Pass through input.
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each other.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        // Setup window to match screen size/pos.
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
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
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time)
            {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
                //  window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
                //                                                             out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
                ImGuiID dock_id_settings, dock_id_grid, dock_id_grid_top_left, dock_id_grid_bottom_left, dock_id_grid_top_right, dock_id_grid_bottom_right;
                ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, &dock_id_settings, &dock_id_grid);
                ImGui::DockBuilderSplitNode(dock_id_grid, ImGuiDir_Left, 0.5f, &dock_id_grid_top_left, &dock_id_grid_top_right);
                ImGui::DockBuilderSplitNode(dock_id_grid_top_left, ImGuiDir_Up, 0.5f, &dock_id_grid_top_left, &dock_id_grid_bottom_left);
                ImGui::DockBuilderSplitNode(dock_id_grid_top_right, ImGuiDir_Up, 0.5f, &dock_id_grid_top_right, &dock_id_grid_bottom_right);

                // we now dock our windows into the docking node we made above
                ImGui::DockBuilderDockWindow("Settings", dock_id_settings);
                ImGui::DockBuilderDockWindow("Render Window 0", dock_id_grid_top_left);
                ImGui::DockBuilderDockWindow("Render Window 1", dock_id_grid_top_right);
                ImGui::DockBuilderDockWindow("Render Window 2", dock_id_grid_bottom_left);
                ImGui::DockBuilderDockWindow("Render Window 3", dock_id_grid_bottom_right);

                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();
}

int main(int argc, const char* argv[]) { // NOLINT: main function.
    std::cout << "Launching Program" << std::endl;

    std::srand(time(NULL));

    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS compat
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    const GLFWvidmode* glfwvidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* glfw_window = glfwCreateWindow(glfwvidmode->width, glfwvidmode->height, "CAD-BASE", nullptr, nullptr);
    if (glfw_window == nullptr) {
        return 1;
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
		return -1;
	}

    viewports = make_shared<vector<shared_ptr<Viewport>>>();
	master_geometry = make_unique<GeometryList>(viewports);

    gui_settings = make_unique<GuiSettings>("Settings", viewports);

    // TODO: temp test.
    for (int i= 0; i < 4; i++) { 
        viewports->push_back(make_shared<Viewport>(glfw_window, glm::vec3(background_colour.x, background_colour.y, background_colour.z), 4000, 4000));
        // Randomly position camera to show different viewports.
        viewports->back()->camera->position = glm::vec3(std::rand()/((RAND_MAX + 1u)/100), std::rand()/((RAND_MAX + 1u)/100), 1 + std::rand()/((RAND_MAX + 1u)/100));
        
        // Make our render windows.
        std::string name = "Render Window " + std::to_string(i);
        gui_render_windows.push_back(make_shared<GuiRenderWindow>(name, viewports->back()));
    }

	master_geometry->push_back(make_shared<Geometry>(TEST_TRIANGLE_VERTS, TEST_TRIANGLE_COLS));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // TODO: Currently broken on Linux, needs further investigation
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(scale_factor);
    // ImGui::SetCurrentFont(ImFont *font)
    ImGui::GetStyle().WindowMinSize = ImVec2(400, 400);


    ImFontConfig cfg;
    cfg.SizePixels = 13 * scale_factor; // NOLINT 13 is the intended size for this font.
    ImGui::GetIO().Fonts->AddFontDefault(&cfg);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(glfw_window))
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

        // We don't want this when drawing UI elements
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Ready Docking etc for window drawing.
        SetupGui();

        // Render Windows
        for (const auto& r : (gui_render_windows)) {
        	r->Draw(delta_t);
		}

        // Settings Window
        gui_settings->Draw(delta_t);

        if (gui_settings->show_demo_window) {
            ImGui::ShowDemoWindow(&(gui_settings->show_demo_window));
        }

        // Rendering
        ImGui::Render();
        
        int display_w;
        int display_h;
        glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(background_colour.x * background_colour.w, background_colour.y * background_colour.w, background_colour.z * background_colour.w, background_colour.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(glfw_window);

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(glfw_window);
    glfwTerminate();

    std::cout << "Terminating Program" << std::endl;    
    return 0;
}
