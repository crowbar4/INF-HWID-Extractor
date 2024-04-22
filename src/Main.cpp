// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "Imgui/imgui.h"
#include "Imgui/backends/imgui_impl_glfw.h"
#include "Imgui/backends/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"

#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Custom header
#include "StringConv.h"
#include "FileDialog.h"
#include "Widgets.h"
#include "INFHwidExtractor.h"
#include "ImguiHelper.h"

#include <vector>
#include <iostream>


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(500, 650, "INF HWID Extractor", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Custom error messages
    int iCurrError = 0;
    std::vector<const char*> vStateMsg
    {
        "Ready",
        "Done",
        "Fail",
        "Error: Ouput path cannot be empty",
        "Error: No .Inf file selected",
        "Parsing... Please don't close this app"
    };
    const char* strMsg = vStateMsg[0];

    INFHwidExtractor infHwidExtractor;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static std::string strOutDir;
            static std::string strINFPaths;
            static std::vector<std::wstring> vINFPaths;
            static std::vector<std::string> vHwidRaw;
            static std::vector<std::string> vHwidTrim;

            SetFitParentWindow(window);

            // ImGui Main Begin
            ImGui::Begin("Hwid Extractor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);                          // Create a window called "Hello, world!" and append into it.

            // Stretch items
            ImGui::PushItemWidth(-60);

            // Browse Output Dir. 
            CustomWidget::BrowseFile("Output Dir.", "output", true, strOutDir);

            // TODO: Wrap INF select module to a widget
            ImGui::Text("inf selected");

            // Left panel (Add/Reset button)
            ImGui::BeginGroup();

            // If new inf file selected, update the path list string
            if (CustomWidget::BrowseToContainer("infpaths", vINFPaths, ImVec2(45, 0)))
            {
                strINFPaths += (strINFPaths.empty() ? "" : "\n") + STDWStringToSTDString(vINFPaths.back());
            }

            // Clear all inf files selected
            if (ImGui::Button("Reset", ImVec2(45, 0)))
            {
                vINFPaths.clear();
                strINFPaths.clear();
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            // Right panel (inf path list)
            ImGui::InputTextMultiline("##rplist", &strINFPaths, ImVec2(0, 60), ImGuiInputTextFlags_ReadOnly);

            ImGui::Spacing();

            // TODO: Move vStateMsg to enum
            // TODO: Move extract tasks to new thread
            if (ImGui::Button("Extract"))
            {
                if (strOutDir.empty())
                {
                    strMsg = vStateMsg[3];
                }
                else if (vINFPaths.empty())
                {
                    strMsg = vStateMsg[4];
                }
                else
                {
                    if (!infHwidExtractor.BeginExtract(STDStringToSTDWString(strOutDir), vINFPaths))
                    {
                        strMsg = vStateMsg[2];
                    }
                    else
                    {
                        strMsg = vStateMsg[1];
                    }

                    vHwidRaw.clear();
                    vHwidTrim.clear();

                    for (const INFObj& infObj : infHwidExtractor.GetINFObjects())
                    {
                        for (const std::wstring& raw : infObj.vHwidRaw)
                        {
                            vHwidRaw.push_back(STDWStringToSTDString(raw));
                        }
                        for (const std::wstring& trim : infObj.vHwidTrim)
                        {
                            vHwidTrim.push_back(STDWStringToSTDString(trim));
                        }
                    }
                }
            }
            ImGui::SameLine();
            ImGui::Text(strMsg);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Hwids list 
            CustomWidget::TextTable(
                "hwidraw",
                "HWIDs (Raw)",
                ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, -1),
                ImVec2(0, -1),
                vHwidRaw);

            ImGui::SameLine();

            CustomWidget::TextTable(
                "hwidtrim",
                "HWIDs (Trim)",
                ImVec2(0, -1),
                ImVec2(0, -1),
                vHwidTrim);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
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

    return 0;
}
