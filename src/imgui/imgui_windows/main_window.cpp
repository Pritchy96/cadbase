#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <nfd.h>

#include "cad_gui/imgui/imgui_windows/main_window.hpp"
#include "cad_gui/imgui/imgui_windows/log_window.hpp"
#include "cad_gui/imgui/app_style.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using std::make_unique;

namespace CadGui {
    MainWindow::MainWindow(GLFWwindow* glfw_window, std::shared_ptr<LogWindow> gui_logger_sink, std::shared_ptr<SceneData> scene_data) : glfw_window(glfw_window), gui_logger_sink_(gui_logger_sink) {
        if (!SetupImgui()) {
            //Todo: error handling.
        }
        
        main_imgui_viewport_ = ImGui::GetMainViewport();

        gui_project_ = make_unique<ProjectWindow>("Project", glfw_window, scene_data);
    }

    bool MainWindow::SetupImgui() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_io_ = &ImGui::GetIO();
        imgui_io_->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // imgui_io_.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // TODO: Currently broken on Linux, needs further investigation
        // imgui_io_.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // imgui_io_.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGui::GetStyle().ScaleAllSizes(gui_scale_factor);  //Do this before SetupGuiTheme() so everything but that modified by SetupGuiTheme() gets scaled.
        SetupGuiTheme();

        // ImGui::SetCurrentFont(ImFont *font)
        ImGui::GetStyle().WindowMinSize = ImVec2(400, 400);

        ImFontConfig cfg;
        cfg.SizePixels = 13 * gui_scale_factor; // NOLINT: 13 is the intended size for this font.
        ImGui::GetIO().Fonts->AddFontDefault(&cfg);

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
        const char* glsl_version = "#version 150";
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    void MainWindow::RenderGuiMainMenu() {   //NOLINT: Nesting is easy to understand.
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
                        //TODO: ImportGeoTest(out_path);
                        free(out_path);
                    }
                    else if ( result == NFD_CANCEL ) {
                        spdlog::info("NFD: User pressed cancel");
                    }
                    else {
                        spdlog::error("NFD: Error with library: {0}", NFD_GetError());
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
                    show_demo_window_ = true;
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

    void MainWindow::SetupGuiTheme() {
        //Set some global style settings
        //Todo: load this from a struct?
        ImGui::GetStyle().WindowPadding = ImVec2(5.0f, 5.0f);
        ImGui::GetStyle().FrameBorderSize = 0;
        ImGui::GetStyle().PopupBorderSize = 1;
        ImGui::GetStyle().WindowBorderSize = 0;

        ImGui::GetStyle().ChildRounding = 8.0f;
        ImGui::GetStyle().FrameRounding = 8.0f;
        ImGui::GetStyle().GrabRounding = 8.0f;
        ImGui::GetStyle().PopupRounding = 8.0f;
        ImGui::GetStyle().ScrollbarRounding = 8.0f;
        ImGui::GetStyle().TabRounding= 8.0f;
        ImGui::GetStyle().WindowRounding= 8.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;

        colors[ImGuiCol_Text]                   = app_style_.BACKGROUND_COLOUR_TEXT;
        colors[ImGuiCol_TextDisabled]           = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_WindowBg]               = app_style_.BACKGROUND_COLOUR_DIM;
        colors[ImGuiCol_ChildBg]                = app_style_.BACKGROUND_COLOUR_DIM;
        colors[ImGuiCol_Border]                 = app_style_.BACKGROUND_COLOUR_BRIGHT;
        colors[ImGuiCol_BorderShadow]           = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_TitleBg]                = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_TitleBgCollapsed]       = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_ScrollbarBg]            = app_style_.BACKGROUND_COLOUR_DIM;
        colors[ImGuiCol_ScrollbarGrab]          = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_ScrollbarGrabHovered]   = app_style_.BACKGROUND_COLOUR_BRIGHT;
        colors[ImGuiCol_ScrollbarGrabActive]    = app_style_.BACKGROUND_COLOUR_TEXT;
        colors[ImGuiCol_Separator]              = app_style_.BACKGROUND_COLOUR_MEDIUM;
        colors[ImGuiCol_FrameBg]                = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_FrameBgHovered]         = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_FrameBgActive]          = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_TitleBgActive]          = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_MenuBarBg]              = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_PopupBg]                = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_CheckMark]              = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_SliderGrab]             = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_SliderGrabActive]       = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_Button]                 = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_ButtonHovered]          = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_ButtonActive]           = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_Header]                 = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_HeaderHovered]          = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_HeaderActive]           = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_SeparatorHovered]       = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_SeparatorActive]        = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_ResizeGrip]             = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_ResizeGripHovered]      = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_ResizeGripActive]       = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_Tab]                    = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_TabHovered]             = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_TabActive]              = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_TabUnfocused]           = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_TabUnfocusedActive]     = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_DockingPreview]         = app_style_.ACCENT_COLOUR_DIM;
        colors[ImGuiCol_DockingEmptyBg]         = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_PlotLines]              = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_PlotLinesHovered]       = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_PlotHistogram]          = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_PlotHistogramHovered]   = app_style_.ACCENT_COLOUR_BRIGHT;
        colors[ImGuiCol_TableHeaderBg]          = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_TableBorderStrong]      = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_TableBorderLight]       = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_TableRowBg]             = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_TableRowBgAlt]          = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_TextSelectedBg]         = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_DragDropTarget]         = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_NavHighlight]           = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_NavWindowingHighlight]  = app_style_.ACCENT_COLOUR_MEDIUM;
        colors[ImGuiCol_NavWindowingDimBg]      = app_style_.ACCENT_COLOUR_UNDEFINED;
        colors[ImGuiCol_ModalWindowDimBg]       = app_style_.ACCENT_COLOUR_UNDEFINED;
    }

    void MainWindow::SetLayout() {
        ImGuiID dockspace_id = ImGui::GetID("root_window_dockspace");

        ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags_ | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, main_imgui_viewport_->Size);

        ImGuiID dock_id_left_pane, dock_id_grid, dock_id_grid_top_left, dock_id_grid_bottom_left, dock_id_grid_top_right, dock_id_grid_bottom_right;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15f, &dock_id_left_pane, &dock_id_grid);

        //Construct the grid.
        ImGui::DockBuilderSplitNode(dock_id_grid, ImGuiDir_Up, 0.5f, &dock_id_grid_top_left, &dock_id_grid_bottom_left);
        ImGui::DockBuilderSplitNode(dock_id_grid_top_left, ImGuiDir_Left, 0.5f, &dock_id_grid_top_left, &dock_id_grid_top_right);
        ImGui::DockBuilderSplitNode(dock_id_grid_bottom_left, ImGuiDir_Left, 0.5f, &dock_id_grid_bottom_left, &dock_id_grid_bottom_right);

        //Auto hide main_imgui_viewport_ tab bars.
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

        ImGui::DockBuilderDockWindow("Log", dock_id_grid_bottom_left);
        ImGui::DockBuilderDockWindow("Render Window 2", dock_id_grid_bottom_left);

        ImGui::DockBuilderDockWindow("Render Window 3", dock_id_grid_bottom_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    void MainWindow::Update() {
        // We don't want this when drawing UI elements
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        RenderGuiMainMenu();
        
        ImGui::SetNextWindowPos(main_imgui_viewport_->Pos);
        ImGui::SetNextWindowSize(main_imgui_viewport_->Size);
        ImGui::SetNextWindowViewport(main_imgui_viewport_->ID);
        //Set some styles just for the root dockspace.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        
        window_flags_ |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags_ & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags_ |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("root_window", nullptr, window_flags_);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // Setup Dockspace
        ImGuiID dockspace_id = ImGui::GetID("root_window_dockspace");

        if (imgui_io_->ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags_);

            //TODO: move this out of the update loop.
            if (first_time_) {
                first_time_ = false;
                SetLayout();
            }
        }

        ImGui::End();

        gui_logger_sink_->Update();


        // Render Windows
        for (const auto& r : (gui_render_windows)) {
            r->Draw();
        }

        // Settings Window
        gui_project_->Draw();

        if(show_demo_window_) {
            ImGui::ShowDemoWindow(&show_demo_window_);
        }

        // Rendering
        ImGui::Render();

        int display_w;
        int display_h;
        glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(app_style_.BACKGROUND_COLOUR_MEDIUM.x * app_style_.BACKGROUND_COLOUR_MEDIUM.w, app_style_.BACKGROUND_COLOUR_MEDIUM.y * app_style_.BACKGROUND_COLOUR_MEDIUM.w, app_style_.BACKGROUND_COLOUR_MEDIUM.z * app_style_.BACKGROUND_COLOUR_MEDIUM.w, app_style_.BACKGROUND_COLOUR_MEDIUM.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(glfw_window);

        // Update and Render additional Platform Windows
        if (imgui_io_->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}