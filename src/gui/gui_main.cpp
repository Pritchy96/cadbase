#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <nfd.h>

#include "cad-base/gui/gui_main.hpp"
#include "cad-base/gui/gui_logger.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using std::make_unique;

GuiMain::GuiMain(GLFWwindow* glfw_window, std::shared_ptr<GuiLogger> gui_logger_sink) : glfw_window(glfw_window), gui_logger_sink_(gui_logger_sink) {
    if (!SetupImgui()) {
        //Todo: error handling.
    }
    
    main_imgui_viewport_ = ImGui::GetMainViewport();

    gui_settings_ = make_unique<GuiProject>("Project", glfw_window);
}

bool GuiMain::SetupImgui() {
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

void GuiMain::RenderGuiMainMenu() {   //NOLINT: Nesting is easy to understand.
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

void GuiMain::SetupGuiTheme() {
    //Set some global style settings
    //Todo: load this from a struct?
    ImGui::GetStyle().WindowPadding = ImVec2(5.0f, 5.0f);
    ImGui::GetStyle().FrameBorderSize = 0;
    ImGui::GetStyle().PopupBorderSize = 1;

    ImGui::GetStyle().PopupRounding = 8.0f;
    ImGui::GetStyle().FrameRounding = 8.0f;

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

void GuiMain::SetLayout() {
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
    ImGui::DockBuilderDockWindow("Render Window 2", dock_id_grid_bottom_left);
    ImGui::DockBuilderDockWindow("Render Window 3", dock_id_grid_bottom_right);

    ImGui::DockBuilderFinish(dockspace_id);
}

void GuiMain::Update(double deltaTime) {
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
        r->Draw(deltaTime);
    }

    // Settings Window
    gui_settings_->Draw(deltaTime);

    if(show_demo_window_) {
        ImGui::ShowDemoWindow(&show_demo_window_);
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

    // Update and Render additional Platform Windows
    if (imgui_io_->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

}
