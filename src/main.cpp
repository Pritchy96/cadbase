
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/cad-base/shader.hpp"

int main(int argc, const char* argv[]) { //NOLINT: main function.
    std::cout << "Launching Program" << std::endl;


    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For MacOS compat, apparently
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CAD-BASE", nullptr, nullptr);
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 150";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat VERTEX_BUFFER_DATA[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

	GLuint basic_shader = shader::LoadShaders((char*)"./shaders/basic.vertshader", (char*)"./shaders/basic.fragshader");

    unsigned int vbo;
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_BUFFER_DATA), VERTEX_BUFFER_DATA, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    //Framebuffer config
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo); //Generate fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); //Bind fbo to config it

    //Create and bind a colour attatchment texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0,GL_RGB, GL_UNSIGNED_BYTE, nullptr); //Setup empty texture

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    //GLuint depthrenderbuffer;
    //glGenRenderbuffers(1, &depthrenderbuffer);
    
    //Depth buffer
    //glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 500, 500);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    //Check that framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer not ok!" << std::endl;
        return -1;
    }

    //Unbind framebuffer (bind default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        //Render opengl window to texture
        glEnable(GL_DEPTH_TEST); // enable depth-testing
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
 
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 500, 500);

        glUseProgram(basic_shader);   
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //Bind default framebuffer again.
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //We don't want this when drawing UI elements
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 0. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        
        //1. Render Window
        {
            ImGui::Begin("GameWindow");
            // Using a Child allow to fill all the space of the window.
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image((ImTextureID)texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
            ImGui::End();
        }

        
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color)); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            }
            
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) {
                show_another_window = false;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
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
