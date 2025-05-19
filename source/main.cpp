// main.cpp

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>   // Needs linking with glfw
#include <stdio.h>

// GLFW error callback
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    // 1. Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;
    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Example", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // 2. Initialize OpenGL loader (e.g., GLAD)
    // if (!gladLoadGL()) { /* handle error */ }

    // 3. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 4. Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 5. Load Fonts (optional)
    // io.Fonts->AddFontDefault();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll inputs
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 6. Your UI code
        {
            static bool show_demo = true;
            ImGui::Begin("Hello, ImGui!");                          
            ImGui::Text("This is a simple window.");               
            ImGui::Checkbox("Demo Window", &show_demo);            
            ImGui::SliderFloat("float", &io.DisplayFramebufferScale.x, 0.0f, 1.0f);
            if (ImGui::Button("Close")) window_should_close = true;
            ImGui::End();

            // Optional: show ImGui’s built‑in demo
            if (show_demo)
                ImGui::ShowDemoWindow(&show_demo);
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 7. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
