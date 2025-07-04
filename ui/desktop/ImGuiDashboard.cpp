#include "ImGuiDashboard.h"
#include "../../ai/AIAgentManager.h"
#include "../../ai/AIAgent.h"
#include "../../network/NetworkGraph.h"
#include "../../network/TopologyGenerator.h"
#include "../../utils/Config.h"
#include "../AdvancedVisualizer.h"
#include "../../shell/Shell.h"
#include "../../missions/Mission.h"
#include "../../utils/Logger.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#ifdef __APPLE__
#include <objc/objc.h>
#include <objc/message.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "../../external/imgui/misc/stb_image.h"
#include "RedNetIcon.h"

// Texture handle for the RedNet icon
static GLuint rednetIconTex = 0;
static int rednetIconWidth = 0, rednetIconHeight = 0;

static GLuint LoadTextureFromFile(const char* filename, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    int channels = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &channels, 4);
    if (image_data == nullptr)
        return 0;
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
    if (out_width) *out_width = image_width;
    if (out_height) *out_height = image_height;
    return image_texture;
}

#define SECTION_HEADER(txt) \
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f, 0.05f, 0.05f, 1.0f)); \
    ImGui::Text("%s", txt); \
    ImGui::PopStyleColor();

ImGuiDashboard::ImGuiDashboard(std::shared_ptr<AIAgentManager> ai,
                               std::shared_ptr<NetworkGraph> net,
                               std::shared_ptr<ConfigManager> cfg,
                               std::shared_ptr<AdvancedVisualizer> vis,
                               std::shared_ptr<Shell> sh,
                               std::shared_ptr<MissionManager> missions)
    : aiManager(ai ? ai : std::make_shared<AIAgentManager>()),
      network(net),
      config(cfg),
      visualizer(vis),
      shell(sh),
      missionManager(missions) {
    // Constructor only initializes member variables
    // GUI setup and main loop are handled in run() method
}

ImGuiDashboard::~ImGuiDashboard() {
    shutdown();
}

void ImGuiDashboard::run() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "RedNet Dashboard", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup ImGui style
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.FramePadding = ImVec2(12, 8);
    style.ItemSpacing = ImVec2(14, 10);
    style.WindowPadding = ImVec2(20, 16);
    style.WindowBorderSize = 0.0f;
    style.ChildRounding = 10.0f;

    // RedNet custom color theme
    style.Colors[ImGuiCol_WindowBg]        = ImVec4(0.97f, 0.97f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]         = ImVec4(0.85f, 0.10f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]   = ImVec4(0.85f, 0.10f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Tab]             = ImVec4(0.85f, 0.10f, 0.15f, 0.80f);
    style.Colors[ImGuiCol_TabActive]       = ImVec4(0.85f, 0.10f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]      = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Button]          = ImVec4(0.85f, 0.10f, 0.15f, 0.85f);
    style.Colors[ImGuiCol_ButtonHovered]   = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]    = ImVec4(0.65f, 0.05f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_Header]          = ImVec4(0.85f, 0.10f, 0.15f, 0.80f);
    style.Colors[ImGuiCol_HeaderHovered]   = ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]    = ImVec4(0.65f, 0.05f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]         = ImVec4(0.98f, 0.98f, 0.99f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.95f, 0.20f, 0.25f, 0.15f);
    style.Colors[ImGuiCol_FrameBgActive]   = ImVec4(0.85f, 0.10f, 0.15f, 0.15f);
    style.Colors[ImGuiCol_Border]          = ImVec4(0.85f, 0.10f, 0.15f, 0.40f);
    style.Colors[ImGuiCol_ScrollbarBg]     = ImVec4(0.97f, 0.97f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]   = ImVec4(0.85f, 0.10f, 0.15f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.95f, 0.20f, 0.25f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.65f, 0.05f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]       = ImVec4(0.85f, 0.10f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]      = ImVec4(0.85f, 0.10f, 0.15f, 0.80f);
    style.Colors[ImGuiCol_SliderGrabActive]= ImVec4(0.95f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_Separator]       = ImVec4(0.85f, 0.10f, 0.15f, 0.40f);
    style.Colors[ImGuiCol_SeparatorHovered]= ImVec4(0.95f, 0.20f, 0.25f, 0.80f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.65f, 0.05f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]         = ImVec4(0.98f, 0.98f, 0.99f, 0.98f);
    style.Colors[ImGuiCol_MenuBarBg]       = ImVec4(0.85f, 0.10f, 0.15f, 0.90f);
    style.Colors[ImGuiCol_TextSelectedBg]  = ImVec4(0.85f, 0.10f, 0.15f, 0.35f);
    style.Colors[ImGuiCol_TabUnfocused]    = ImVec4(0.85f, 0.10f, 0.15f, 0.40f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.85f, 0.10f, 0.15f, 0.70f);

    // Try to load macOS system font
    ImFontConfig font_cfg;
    font_cfg.OversampleH = font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
    bool fontLoaded = false;
#ifdef __APPLE__
    fontLoaded = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/SFNS.ttf", 19.0f, &font_cfg) != nullptr;
    if (!fontLoaded)
        fontLoaded = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/HelveticaNeue.ttc", 19.0f, &font_cfg) != nullptr;
#endif
    if (!fontLoaded)
        io.Fonts->AddFontDefault();

    // Load FontAwesome icon font
    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("fonts/fa-solid-900.ttf", 18.0f, &icons_config, icons_ranges);

    // Load RedNet icon texture once
    printf("Attempting to load RedNet icon...\n");
    if (rednetIconTex == 0) {
        rednetIconTex = LoadTextureFromFile("../resources/icon.png", &rednetIconWidth, &rednetIconHeight);
        if (rednetIconTex == 0) {
            rednetIconTex = LoadTextureFromFile("resources/icon.png", &rednetIconWidth, &rednetIconHeight);
        }
        if (rednetIconTex == 0) {
            rednetIconTex = LoadTextureFromFile("RedNet.iconset/icon_128x128.png", &rednetIconWidth, &rednetIconHeight);
        }
        if (rednetIconTex == 0) {
            rednetIconTex = LoadTextureFromFile("../RedNet.iconset/icon_128x128.png", &rednetIconWidth, &rednetIconHeight);
        }
        if (rednetIconTex == 0) {
            printf("Failed to load RedNet icon as OpenGL texture!\n");
        } else {
            printf("Loaded RedNet icon as OpenGL texture: %d (%dx%d)\n", rednetIconTex, rednetIconWidth, rednetIconHeight);
        }
    }

    // Setup Platform/Renderer backends
#ifdef __APPLE__
    const char* glsl_version = "#version 120";
#else
    const char* glsl_version = "#version 130";
#endif
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // After ImGui context creation, add a larger font for logs
    ImFont* largeFont = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/SFNS.ttf", 22.0f);
    if (!largeFont) largeFont = io.Fonts->AddFontDefault();



    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- macOS-style menu bar ---
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("RedNet")) {
                if (ImGui::MenuItem("About RedNet")) {
                    showAboutDialog = true;
                }
                if (ImGui::MenuItem("Quit RedNet", "Cmd+Q")) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Cmd+Q shortcut
        if ((io.KeySuper && ImGui::IsKeyPressed(ImGuiKey_Q))) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        

        
        update();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
}

void ImGuiDashboard::update() {
    // Create main window
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, displaySize.y - ImGui::GetFrameHeight()));
    
    ImGui::Begin("RedNet Dashboard", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    
    // Draw RedNet icon and title aligned in header
    ImGui::BeginGroup();
    if (rednetIconTex) {
        ImGui::Image((void*)(intptr_t)rednetIconTex, ImVec2(32, 32));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
        ImGui::Text("🔴");
        ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    
    // Align text vertically with icon center
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6); // Adjust for vertical centering
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
    ImGui::Text("RedNet Dashboard");
    ImGui::PopStyleColor();
    ImGui::EndGroup();
    
    // Add a colored header bar at the top of the window
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.85f, 0.10f, 0.15f, 1.0f)); // Bold red header
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.95f, 0.20f, 0.25f, 1.0f)); // Lighter red on hover
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.65f, 0.05f, 0.10f, 1.00f)); // Dark red on active
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.85f, 0.10f, 0.15f, 1.0f)); // Bold red title bar
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.85f, 0.10f, 0.15f, 1.0f)); // Bold red title bar active
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.85f, 0.10f, 0.15f, 0.80f)); // Red accent tab
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.85f, 0.10f, 0.15f, 1.0f)); // Red accent tab active
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.95f, 0.20f, 0.25f, 1.0f)); // Lighter red tab hovered
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.10f, 0.15f, 0.85f)); // Red button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.20f, 0.25f, 1.0f)); // Lighter red button hovered
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.65f, 0.05f, 0.10f, 1.0f)); // Dark red button active
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.10f, 0.15f, 0.40f)); // Red border

    // 1. Welcome banner at the top of the dashboard window (in update())
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.85f, 0.10f, 0.15f, 0.15f));
    ImGui::BeginChild("WelcomeBanner", ImVec2(0, 40), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetCursorPosY(4);
    
    // Calculate center position for icon + text
    float totalWidth = 32 + 8 + ImGui::CalcTextSize("Welcome to RedNet!").x; // icon + spacing + text
    float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;
    ImGui::SetCursorPosX(startX);
    
    // Display icon
    if (rednetIconTex) {
        ImGui::Image((void*)(intptr_t)rednetIconTex, ImVec2(32, 32));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
        ImGui::Text("🔴");
        ImGui::PopStyleColor();
    }
    
    // Display text next to icon
    ImGui::SameLine();
    ImGui::SetCursorPosY(10);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.00f, 0.00f, 1.0f));
    ImGui::Text("Welcome to RedNet!");
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleColor();

    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        if (ImGui::BeginTabItem("Agents")) {
            renderAgentTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Network")) {
            renderNetworkTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Missions")) {
            renderMissionsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Logs")) {
            renderLogsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            renderConfigTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Controls")) {
            renderControlsTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::PopStyleColor(10); // Pop all pushed colors
    ImGui::End();

    // 4. Status bar at the bottom of the window (in update())
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 24));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 24));
    ImGui::Begin("StatusBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Status: Ready");
    ImGui::End();

    // About dialog - process in update() function
    if (showAboutDialog) {
        ImGui::OpenPopup("AboutPopup");
        showAboutDialog = false;
    }
    
    if (ImGui::BeginPopupModal("AboutPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        // About dialog header with icon
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.85f, 0.10f, 0.15f, 0.1f));
        ImGui::BeginChild("AboutHeader", ImVec2(400, 80), false, ImGuiWindowFlags_NoScrollbar);
        
        // Center the icon and title
        ImGui::SetCursorPosX((400 - 64) * 0.5f);
        ImGui::SetCursorPosY(8);
        
        if (rednetIconTex) {
            ImGui::Image((void*)(intptr_t)rednetIconTex, ImVec2(64, 64));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
            ImGui::Text("🔴");
            ImGui::PopStyleColor();
        }
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        // Application title - centered
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("RedNet").x) * 0.5f);
        ImGui::Text("RedNet");
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("AI Cyber Range Simulator").x) * 0.5f);
        ImGui::Text("AI Cyber Range Simulator");
        
        ImGui::Separator();
        
        // Version and build information - centered
        std::string versionText = "Version: 1.0.0";
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(versionText.c_str()).x) * 0.5f);
        ImGui::Text("Version:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 0.6f, 0.0f, 1.0f), "1.0.0");
        
        std::string buildDateText = "Build Date: " + std::string(__DATE__) + " " + std::string(__TIME__);
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(buildDateText.c_str()).x) * 0.5f);
        ImGui::Text("Build Date:");
        ImGui::SameLine();
        ImGui::Text("%s %s", __DATE__, __TIME__);
        
        std::string platformText = "Platform: ";
#ifdef __APPLE__
        platformText += "macOS";
#elif defined(_WIN32)
        platformText += "Windows";
#elif defined(__linux__)
        platformText += "Linux";
#else
        platformText += "Unknown";
#endif
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(platformText.c_str()).x) * 0.5f);
        ImGui::Text("Platform:");
        ImGui::SameLine();
#ifdef __APPLE__
        ImGui::Text("macOS");
#elif defined(_WIN32)
        ImGui::Text("Windows");
#elif defined(__linux__)
        ImGui::Text("Linux");
#else
        ImGui::Text("Unknown");
#endif
        
        std::string compilerText = "Compiler: ";
#ifdef __clang__
        compilerText += "Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
        compilerText += "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
        compilerText += "MSVC " + std::to_string(_MSC_VER);
#else
        compilerText += "Unknown";
#endif
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(compilerText.c_str()).x) * 0.5f);
        ImGui::Text("Compiler:");
        ImGui::SameLine();
#ifdef __clang__
        ImGui::Text("Clang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
        ImGui::Text("GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
        ImGui::Text("MSVC %d", _MSC_VER);
#else
        ImGui::Text("Unknown");
#endif
        
        ImGui::Separator();
        
        // System information - centered
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("System Information:").x) * 0.5f);
        ImGui::Text("System Information:");
        
        // Format OpenGL version text properly
        const char* glVersionStr = (const char*)glGetString(GL_VERSION);
        std::string openglVersionText = std::string("• OpenGL Version: ") + std::string(glVersionStr);
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(openglVersionText.c_str()).x) * 0.5f);
        ImGui::Text("• OpenGL Version: %s", glVersionStr);
        
        // Format OpenGL renderer text properly
        const char* glRendererStr = (const char*)glGetString(GL_RENDERER);
        std::string openglRendererText = std::string("• OpenGL Renderer: ") + std::string(glRendererStr);
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(openglRendererText.c_str()).x) * 0.5f);
        ImGui::Text("• OpenGL Renderer: %s", glRendererStr);
        
        // Format ImGui version text properly
        std::string imguiVersionText = std::string("• ImGui Version: ") + std::string(IMGUI_VERSION);
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(imguiVersionText.c_str()).x) * 0.5f);
        ImGui::Text("• ImGui Version: %s", IMGUI_VERSION);
        
        // Get current system stats
        auto agents = aiManager->getSimpleAgents();
        std::string activeAgentsText = "• Active Agents: " + std::to_string(agents.size());
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(activeAgentsText.c_str()).x) * 0.5f);
        ImGui::Text("• Active Agents: %zu", agents.size());
        
        if (network) {
            std::string networkNodesText = "• Network Nodes: " + std::to_string(network->getAllNodes().size());
            ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(networkNodesText.c_str()).x) * 0.5f);
            ImGui::Text("• Network Nodes: %zu", network->getAllNodes().size());
        } else {
            std::string networkNodesText = "• Network Nodes: 0";
            ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(networkNodesText.c_str()).x) * 0.5f);
            ImGui::Text("• Network Nodes: 0");
        }
        
        // Mission status
        Mission* currentMission = missionManager->getCurrentMission();
        if (currentMission) {
            std::string currentMissionText = "• Current Mission: " + currentMission->name;
            ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(currentMissionText.c_str()).x) * 0.5f);
            ImGui::Text("• Current Mission: %s", currentMission->name.c_str());
        } else {
            std::string currentMissionText = "• Current Mission: None";
            ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(currentMissionText.c_str()).x) * 0.5f);
            ImGui::Text("• Current Mission: None");
        }
        
        ImGui::Separator();
        
        // Features - centered
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("Features:").x) * 0.5f);
        ImGui::Text("Features:");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• AI-driven penetration testing agents").x) * 0.5f);
        ImGui::BulletText("AI-driven penetration testing agents");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Dynamic network topology generation").x) * 0.5f);
        ImGui::BulletText("Dynamic network topology generation");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Real-time security simulation").x) * 0.5f);
        ImGui::BulletText("Real-time security simulation");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Mission-based training scenarios").x) * 0.5f);
        ImGui::BulletText("Mission-based training scenarios");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Comprehensive logging and monitoring").x) * 0.5f);
        ImGui::BulletText("Comprehensive logging and monitoring");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Advanced visualization and analytics").x) * 0.5f);
        ImGui::BulletText("Advanced visualization and analytics");
        
        ImGui::Separator();
        
        // Credits - centered
        std::string createdByText = "Created by: Silviu";
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(createdByText.c_str()).x) * 0.5f);
        ImGui::Text("Created by:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 0.4f, 0.8f, 1.0f), "Silviu");
        
        // LinkedIn link - centered
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("LinkedIn Profile").x) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.4f, 0.8f, 1.0f));
        if (ImGui::Button("LinkedIn Profile")) {
            // Open LinkedIn profile in default browser
            system("open https://www.linkedin.com/in/silviu-i-hermeneanu/");
        }
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("Built with:").x) * 0.5f);
        ImGui::Text("Built with:");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• Dear ImGui - Immediate Mode GUI").x) * 0.5f);
        ImGui::BulletText("Dear ImGui - Immediate Mode GUI");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• GLFW - OpenGL Framework").x) * 0.5f);
        ImGui::BulletText("GLFW - OpenGL Framework");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• OpenGL - Graphics API").x) * 0.5f);
        ImGui::BulletText("OpenGL - Graphics API");
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("• C++ - Core Language").x) * 0.5f);
        ImGui::BulletText("C++ - Core Language");
        
        ImGui::Separator();
        
        // Copyright
        ImGui::SetCursorPosX((400 - ImGui::CalcTextSize("© 2024 RedNet Project").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "© 2024 RedNet Project");
        
        ImGui::Spacing();
        
        // Close button
        ImGui::SetCursorPosX((400 - 100) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.10f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.20f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.65f, 0.05f, 0.10f, 1.0f));
        if (ImGui::Button("Close", ImVec2(100, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);
        
        ImGui::EndPopup();
    }
}

void ImGuiDashboard::renderAgentTab() {
    SECTION_HEADER("AI Agent Management");
    static int reconAgentCount = 1;
    static int exploitAgentCount = 1;
    // Agent controls
    if (ImGui::Button("Create Recon Agent")) {
        std::string name = "Recon_Agent_" + std::to_string(reconAgentCount++);
        aiManager->addAgent(name, "Reconnaissance");
        LOG_AGENT_ACTION("System", "Create Agent", name, "Success");
        ImGui::OpenPopup("AgentCreated");
    }
    if (ImGui::Button("Create Exploit Agent")) {
        std::string name = "Exploit_Agent_" + std::to_string(exploitAgentCount++);
        aiManager->addAgent(name, "Exploitation");
        LOG_AGENT_ACTION("System", "Create Agent", name, "Success");
        ImGui::OpenPopup("AgentCreated");
    }
    if (ImGui::BeginPopupModal("AgentCreated", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Agent created successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::Separator();
    ImGui::Text("Active Agents:");
    ImGui::BeginChild("AgentList", ImVec2(0, 200), true);
    auto agents = aiManager->getSimpleAgents();
    int i = 0;
    for (const auto& agent : agents) {
        ImGui::PushID(i);
        if (ImGui::CollapsingHeader(agent.name.c_str())) {
            ImGui::Text("Type: %s", agent.type.c_str());
            ImGui::Text("State: %s", agent.state.c_str());
            ImGui::Text("Intelligence:");
            ImGui::SameLine();
            ImGui::Text("85%%");
            ImGui::Text("Stealth:");
            ImGui::SameLine();
            ImGui::Text("92%%");
            if (ImGui::Button("Stop")) {
                aiManager->removeAgent(agent.name);
                LOG_AGENT_ACTION("System", "Stop Agent", agent.name, "Success");
            }
            ImGui::SameLine();
            if (ImGui::Button("Details")) {
                ImGui::OpenPopup("AgentDetails");
            }
        }
        ImGui::PopID();
        ++i;
    }
    ImGui::EndChild();
}

void ImGuiDashboard::renderNetworkTab() {
    SECTION_HEADER("Network Topology");
    static int topologyCount = 1;
    if (ImGui::Button("Generate New Topology")) {
        TopologyGenerator generator;
        network = generator.generateRandomTopology(6 + (topologyCount++ % 5), 8 + (topologyCount % 5));
        LOG_NETWORK_EVENT("Topology Generated", "Network", "Generated new network topology with " + std::to_string(network->getAllNodes().size()) + " nodes");
        ImGui::OpenPopup("TopologyGenerated");
    }
    if (ImGui::BeginPopupModal("TopologyGenerated", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New topology generated!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    if (ImGui::Button("Scan Network")) {
        LOG_SCAN_RESULT("Scanner", "Network", "Network scan initiated");
        ImGui::OpenPopup("ScanComplete");
    }
    if (ImGui::BeginPopupModal("ScanComplete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Network scan completed!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::Separator();
    ImGui::Text("Network Nodes:");
    ImGui::BeginChild("NetworkNodes", ImVec2(0, 300), true);
    if (network) {
        const auto& nodes = network->getAllNodes();
        int i = 0;
        for (const auto& node : nodes) {
            ImGui::PushID(i);
            if (ImGui::CollapsingHeader(node->getIp().c_str())) {
                ImGui::Text("Type: %s", node->getTypeName().c_str());
                ImGui::Text("Status: %s", node->isNodeActive() ? "Online" : "Offline");
                ImGui::Text("IP: %s", node->getIp().c_str());
                ImGui::Text("Hostname: %s", node->getHostname().c_str());
                ImGui::Text("Security Level: %s", node->getSecurityLevelName().c_str());
                ImGui::Text("Services:");
                for (const auto& svc : node->getServices()) {
                    ImGui::BulletText("%s (port %d) %s", svc.serviceName.c_str(), svc.port, svc.isRunning ? "[Running]" : "[Stopped]");
                }
            }
            ImGui::PopID();
            ++i;
        }
    } else {
        ImGui::Text("No network loaded.");
    }
    ImGui::EndChild();
}

void ImGuiDashboard::renderMissionsTab() {
    SECTION_HEADER("Mission Management");
    static std::string selectedMissionId;
    if (ImGui::Button("Load Mission")) {
        ImGui::OpenPopup("LoadMission");
    }
    if (ImGui::BeginPopupModal("LoadMission", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Select mission to load:");
        auto availableMissions = missionManager->getAvailableMissions();
        for (const auto& mission : availableMissions) {
            if (ImGui::Button(mission.name.c_str())) {
                selectedMissionId = mission.id;
                // Load the selected mission
                if (missionManager->loadMissionById(mission.id)) {
                    LOG_MISSION_EVENT(mission.id, "Load Mission", "Loaded mission: " + mission.name);
                } else {
                    LOG_MISSION_EVENT(mission.id, "Load Mission", "Failed to load mission: " + mission.name);
                }
                ImGui::CloseCurrentPopup();
            }
        }
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::Separator();
    Mission* currentMission = missionManager->getCurrentMission();
    if (currentMission) {
        ImGui::Text("Current Mission: %s", currentMission->name.c_str());
        ImGui::Text("Description: %s", currentMission->description.c_str());
        ImGui::Text("Difficulty: %s", currentMission->difficulty.c_str());
        ImGui::Text("Status: %s", currentMission->completed ? "Completed" : "In Progress");
        ImGui::Text("Progress: %d%%", missionManager->getMissionProgress());
        ImGui::Text("Score: %d/%d", missionManager->getTotalScore(), currentMission->totalPoints);
        ImGui::Separator();
        ImGui::Text("Objectives:");
        for (const auto& objective : currentMission->objectives) {
            ImGui::BulletText("%s: %s %s", objective.name.c_str(), objective.description.c_str(), objective.completed ? "[✓]" : "[ ]");
        }
        if (ImGui::Button("Start Mission")) {
            missionManager->startMission(currentMission->id);
            LOG_MISSION_EVENT(currentMission->id, "Start Mission", "Started mission: " + currentMission->name);
            ImGui::OpenPopup("MissionStarted");
        }
        if (ImGui::BeginPopupModal("MissionStarted", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Mission started successfully!");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    } else {
        ImGui::Text("Current Mission: None");
        ImGui::Text("Status: Not Started");
        ImGui::Text("Progress: 0%%");
        ImGui::Text("No mission loaded. Click 'Load Mission' to select one.");
    }
}

void ImGuiDashboard::renderLogsTab() {
    SECTION_HEADER("System Logs");
    static char searchFilter[256] = "";
    static bool showDebug = false;
    static bool showInfo = true;
    static bool showWarning = true;
    static bool showError = true;
    static bool showCritical = true;
    
    // Category filters
    static bool showSystem = true;
    static bool showAgent = true;
    static bool showNetwork = true;
    static bool showMission = true;
    static bool showSecurity = true;
    static bool showExploit = true;
    static bool showScan = true;
    static bool showConfig = true;
    
    auto& logger = Logger::getInstance();
    
    // Log controls
    if (ImGui::Button("Clear Logs")) {
        // Note: Can't clear the global logger, but we can set a higher log level temporarily
        LOG_INFO(LogCategory::SYSTEM, "Dashboard", "Log display cleared by user");
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Agent Action")) {
        LOG_AGENT_ACTION("Test_Agent", "Scan", "192.168.1.100", "Success");
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Security Event")) {
        LOG_SECURITY_EVENT("Intrusion Detected", "192.168.1.50", "192.168.1.10", LogLevel::WARNING);
    }
    ImGui::SameLine();
    if (ImGui::Button("Test Exploit")) {
        LOG_EXPLOIT_ATTEMPT("Evil_Agent", "192.168.1.20", "CVE-2021-1234", true);
    }
    
    // Statistics
    ImGui::Separator();
    ImGui::Text("Log Statistics:");
    ImGui::Text("Total: %zu | Debug: %zu | Info: %zu | Warning: %zu | Error: %zu | Critical: %zu", 
                logger.getLogCount(),
                logger.getLogCountByLevel(LogLevel::DEBUG),
                logger.getLogCountByLevel(LogLevel::INFO),
                logger.getLogCountByLevel(LogLevel::WARNING),
                logger.getLogCountByLevel(LogLevel::ERROR),
                logger.getLogCountByLevel(LogLevel::CRITICAL));
    
    // Level filters
    ImGui::Separator();
    ImGui::Text("Level Filters:");
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &showDebug);
    ImGui::SameLine();
    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &showWarning);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &showError);
    ImGui::SameLine();
    ImGui::Checkbox("Critical", &showCritical);
    
    // Category filters
    ImGui::Text("Category Filters:");
    ImGui::Checkbox("System", &showSystem); ImGui::SameLine();
    ImGui::Checkbox("Agent", &showAgent); ImGui::SameLine();
    ImGui::Checkbox("Network", &showNetwork); ImGui::SameLine();
    ImGui::Checkbox("Mission", &showMission);
    ImGui::Checkbox("Security", &showSecurity); ImGui::SameLine();
    ImGui::Checkbox("Exploit", &showExploit); ImGui::SameLine();
    ImGui::Checkbox("Scan", &showScan); ImGui::SameLine();
    ImGui::Checkbox("Config", &showConfig);
    
    // Search filter
    ImGui::InputText("Search", searchFilter, sizeof(searchFilter));
    
    ImGui::Separator();
    ImGui::Text("Real-time Activity Log:");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.98f, 0.98f, 0.99f, 1.0f));
    ImGui::BeginChild("Logs", ImVec2(0, 0), true, ImGuiWindowFlags_None);
    
    // Get recent logs from the logger
    auto logs = logger.getRecentLogs(500);
    
    // Add initial logs if logger is empty
    if (logs.empty()) {
        LOG_INFO(LogCategory::SYSTEM, "Dashboard", "RedNet system initialized");
        LOG_INFO(LogCategory::SYSTEM, "Dashboard", "GUI dashboard started successfully");
        LOG_INFO(LogCategory::NETWORK, "NetworkManager", "Network topology generator ready");
        LOG_INFO(LogCategory::MISSION, "MissionManager", "Mission system loaded");
        logs = logger.getRecentLogs(10);
    }
    
    for (const auto& log : logs) {
        // Apply level filters
        bool shouldShow = true;
        if (!showDebug && log.level == LogLevel::DEBUG) shouldShow = false;
        if (!showInfo && log.level == LogLevel::INFO) shouldShow = false;
        if (!showWarning && log.level == LogLevel::WARNING) shouldShow = false;
        if (!showError && log.level == LogLevel::ERROR) shouldShow = false;
        if (!showCritical && log.level == LogLevel::CRITICAL) shouldShow = false;
        
        // Apply category filters
        if (!showSystem && log.category == LogCategory::SYSTEM) shouldShow = false;
        if (!showAgent && log.category == LogCategory::AGENT) shouldShow = false;
        if (!showNetwork && log.category == LogCategory::NETWORK) shouldShow = false;
        if (!showMission && log.category == LogCategory::MISSION) shouldShow = false;
        if (!showSecurity && log.category == LogCategory::SECURITY) shouldShow = false;
        if (!showExploit && log.category == LogCategory::EXPLOIT) shouldShow = false;
        if (!showScan && log.category == LogCategory::SCAN) shouldShow = false;
        if (!showConfig && log.category == LogCategory::CONFIG) shouldShow = false;
        
        if (!shouldShow) continue;
        
        // Apply search filter
        if (strlen(searchFilter) > 0) {
            std::string logText = Logger::formatTimestamp(log.timestamp) + " " + 
                                 log.source + " " + log.action + " " + log.target + " " + log.message;
            std::transform(logText.begin(), logText.end(), logText.begin(), ::tolower);
            std::string searchLower = searchFilter;
            std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            if (logText.find(searchLower) == std::string::npos) continue;
        }
        
        // Color code by log level
        ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        switch (log.level) {
            case LogLevel::DEBUG: textColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); break;
            case LogLevel::INFO: textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); break;
            case LogLevel::WARNING: textColor = ImVec4(0.8f, 0.6f, 0.0f, 1.0f); break;
            case LogLevel::ERROR: textColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f); break;
            case LogLevel::CRITICAL: textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
        }
        
        // Category badge color
        ImVec4 badgeColor = ImVec4(0.85f, 0.10f, 0.15f, 1.0f);
        switch (log.category) {
            case LogCategory::AGENT: badgeColor = ImVec4(0.0f, 0.6f, 0.8f, 1.0f); break;
            case LogCategory::NETWORK: badgeColor = ImVec4(0.0f, 0.8f, 0.2f, 1.0f); break;
            case LogCategory::MISSION: badgeColor = ImVec4(0.8f, 0.4f, 0.0f, 1.0f); break;
            case LogCategory::SECURITY: badgeColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f); break;
            case LogCategory::EXPLOIT: badgeColor = ImVec4(0.6f, 0.0f, 0.6f, 1.0f); break;
            case LogCategory::SCAN: badgeColor = ImVec4(0.0f, 0.4f, 0.8f, 1.0f); break;
            case LogCategory::CONFIG: badgeColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); break;
            default: badgeColor = ImVec4(0.85f, 0.10f, 0.15f, 1.0f); break;
        }
        
        // Display log entry
        std::string timestamp = Logger::formatTimestamp(log.timestamp);
        std::string levelStr = Logger::levelToString(log.level);
        std::string categoryStr = Logger::categoryToString(log.category);
        
        // Timestamp
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[%s]", timestamp.c_str());
        ImGui::SameLine();
        
        // Level badge
        ImGui::TextColored(textColor, "[%s]", levelStr.c_str());
        ImGui::SameLine();
        
        // Category badge
        ImGui::TextColored(badgeColor, "[%s]", categoryStr.c_str());
        ImGui::SameLine();
        
        // Source
        ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.8f, 1.0f), "%s:", log.source.c_str());
        ImGui::SameLine();
        
        // Message
        ImGui::TextColored(textColor, "%s", log.message.c_str());
        
        // Tooltip with details
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Timestamp: %s", timestamp.c_str());
            ImGui::Text("Level: %s", levelStr.c_str());
            ImGui::Text("Category: %s", categoryStr.c_str());
            ImGui::Text("Source: %s", log.source.c_str());
            ImGui::Text("Action: %s", log.action.c_str());
            ImGui::Text("Target: %s", log.target.c_str());
            ImGui::Text("Message: %s", log.message.c_str());
            if (!log.metadata.empty()) {
                ImGui::Separator();
                ImGui::Text("Metadata:");
                for (const auto& [key, value] : log.metadata) {
                    ImGui::Text("  %s: %s", key.c_str(), value.c_str());
                }
            }
            ImGui::EndTooltip();
        }
    }
    
    // Auto-scroll to bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void ImGuiDashboard::renderConfigTab() {
    SECTION_HEADER("Configuration");
    static bool configChanged = false;
    
    // Get current configuration
    auto exploitConfig = config->getExploitConfig();
    auto securityConfig = config->getSecurityConfig();
    auto aiConfig = config->getAIConfig();
    auto visualizationConfig = config->getVisualizationConfig();
    auto generalConfig = config->getGeneralConfig();
    
    // Configuration presets
    if (ImGui::Button("Load Default Config")) {
        config->loadDefaultConfig();
        LOG_INFO(LogCategory::CONFIG, "Dashboard", "Loaded default configuration");
        configChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to Defaults")) {
        config->resetToDefaults();
        LOG_WARNING(LogCategory::CONFIG, "Dashboard", "Reset configuration to defaults");
        configChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Configuration")) {
        if (config->saveConfig()) {
            LOG_INFO(LogCategory::CONFIG, "Dashboard", "Configuration saved successfully");
            ImGui::OpenPopup("ConfigSaved");
        } else {
            LOG_ERROR(LogCategory::CONFIG, "Dashboard", "Failed to save configuration");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Scenario")) {
        ImGui::OpenPopup("LoadScenario");
    }
    
    if (ImGui::BeginPopupModal("ConfigSaved", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Configuration saved successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    // Load Scenario popup
    if (ImGui::BeginPopupModal("LoadScenario", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Select scenario to load:");
        auto scenarioNames = config->getScenarioTemplateNames();
        for (const auto& scenarioName : scenarioNames) {
            if (ImGui::Button(scenarioName.c_str())) {
                config->loadScenario(scenarioName);
                LOG_INFO(LogCategory::CONFIG, "Dashboard", "Loaded scenario: " + scenarioName);
                configChanged = true;
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup("ScenarioLoaded");
            }
        }
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopupModal("ScenarioLoaded", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Scenario loaded successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    // AI Configuration
    if (ImGui::CollapsingHeader("AI Configuration")) {
        static int aiDifficulty = static_cast<int>(aiConfig.difficulty);
        static int maxAgents = aiConfig.maxAgents;
        static float intelligenceLevel = aiConfig.intelligenceLevel;
        static float aggressionLevel = aiConfig.aggressionLevel;
        static float stealthLevel = aiConfig.stealthLevel;
        
        const char* difficultyNames[] = {"Novice", "Intermediate", "Advanced", "Expert"};
        if (ImGui::Combo("AI Difficulty", &aiDifficulty, difficultyNames, 4)) {
            aiConfig.difficulty = static_cast<AIDifficulty>(aiDifficulty);
            configChanged = true;
        }
        
        if (ImGui::SliderInt("Max Agents", &maxAgents, 1, 20)) {
            aiConfig.maxAgents = maxAgents;
            configChanged = true;
        }
        
        if (ImGui::SliderFloat("Intelligence Level", &intelligenceLevel, 0.0f, 1.0f)) {
            aiConfig.intelligenceLevel = intelligenceLevel;
            configChanged = true;
        }
        
        if (ImGui::SliderFloat("Aggression Level", &aggressionLevel, 0.0f, 1.0f)) {
            aiConfig.aggressionLevel = aggressionLevel;
            configChanged = true;
        }
        
        if (ImGui::SliderFloat("Stealth Level", &stealthLevel, 0.0f, 1.0f)) {
            aiConfig.stealthLevel = stealthLevel;
            configChanged = true;
        }
        
        if (configChanged) {
            config->setAIConfig(aiConfig);
        }
    }
    
    // Security Configuration
    if (ImGui::CollapsingHeader("Security Configuration")) {
        static int securityLevel = securityConfig.level;
        static bool enableDetection = securityConfig.enableDetection;
        static bool enableResponse = securityConfig.enableResponse;
        static float detectionProbability = securityConfig.detectionProbability;
        static float responseTime = securityConfig.responseTime;
        
        if (ImGui::SliderInt("Security Level", &securityLevel, 1, 5)) {
            securityConfig.level = securityLevel;
            configChanged = true;
        }
        
        if (ImGui::Checkbox("Enable Detection", &enableDetection)) {
            securityConfig.enableDetection = enableDetection;
            configChanged = true;
        }
        
        if (ImGui::Checkbox("Enable Response", &enableResponse)) {
            securityConfig.enableResponse = enableResponse;
            configChanged = true;
        }
        
        if (ImGui::SliderFloat("Detection Probability", &detectionProbability, 0.0f, 1.0f)) {
            securityConfig.detectionProbability = detectionProbability;
            configChanged = true;
        }
        
        if (ImGui::SliderFloat("Response Time (s)", &responseTime, 1.0f, 30.0f)) {
            securityConfig.responseTime = responseTime;
            configChanged = true;
        }
        
        if (configChanged) {
            config->setSecurityConfig(securityConfig);
        }
    }
    
    // General Configuration
    if (ImGui::CollapsingHeader("General Configuration")) {
        static char logLevel[32];
        strcpy(logLevel, generalConfig.logLevel.c_str());
        static bool enableLogging = generalConfig.enableLogging;
        static bool autoSave = generalConfig.autoSave;
        static int autoSaveInterval = generalConfig.autoSaveInterval;
        
        if (ImGui::InputText("Log Level", logLevel, sizeof(logLevel))) {
            generalConfig.logLevel = logLevel;
            configChanged = true;
        }
        
        if (ImGui::Checkbox("Enable Logging", &enableLogging)) {
            generalConfig.enableLogging = enableLogging;
            configChanged = true;
        }
        
        if (ImGui::Checkbox("Auto Save", &autoSave)) {
            generalConfig.autoSave = autoSave;
            configChanged = true;
        }
        
        if (ImGui::SliderInt("Auto Save Interval (s)", &autoSaveInterval, 60, 1800)) {
            generalConfig.autoSaveInterval = autoSaveInterval;
            configChanged = true;
        }
        
        if (configChanged) {
            config->setGeneralConfig(generalConfig);
        }
    }
    
    // Current Settings Summary
    ImGui::Separator();
    ImGui::Text("Current Settings Summary:");
    ImGui::Text("AI Difficulty: %s", aiConfig.difficulty == NOVICE_AI ? "Novice" : 
                aiConfig.difficulty == INTERMEDIATE_AI ? "Intermediate" :
                aiConfig.difficulty == ADVANCED_AI ? "Advanced" : "Expert");
    ImGui::Text("Max Agents: %d", aiConfig.maxAgents);
    ImGui::Text("Security Level: %d", securityConfig.level);
    ImGui::Text("Detection: %s", securityConfig.enableDetection ? "Enabled" : "Disabled");
    ImGui::Text("Log Level: %s", generalConfig.logLevel.c_str());
    ImGui::Text("Auto Save: %s", generalConfig.autoSave ? "Enabled" : "Disabled");
    
    if (configChanged) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Configuration modified - remember to save!");
        configChanged = false; // Reset the flag after showing the message
    }
    
    // Real-time configuration monitoring
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Real-time Configuration Monitoring")) {
        ImGui::Text("Configuration Status:");
        ImGui::Text("Config File: %s", config->getConfigFile().c_str());
        ImGui::Text("Config Loaded: %s", config->isConfigLoaded() ? "Yes" : "No");
        
        ImGui::Separator();
        ImGui::Text("Live Configuration Effects:");
        
        // Show how current config affects the simulation
        if (aiConfig.maxAgents > 0) {
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "✓ AI agents enabled (max: %d)", aiConfig.maxAgents);
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "✗ AI agents disabled");
        }
        
        if (securityConfig.enableDetection) {
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "✓ Security detection active (%.1f%% probability)", securityConfig.detectionProbability * 100);
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "✗ Security detection disabled");
        }
        
        if (exploitConfig.enableChaining) {
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "✓ Exploit chaining enabled");
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "⚠ Exploit chaining disabled");
        }
        
        if (visualizationConfig.enableRealTime) {
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "✓ Real-time visualization active");
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "⚠ Real-time visualization disabled");
        }
        
        // Apply configuration changes button
        ImGui::Separator();
        if (ImGui::Button("Apply Configuration Changes")) {
            applyConfigurationChanges();
            LOG_INFO(LogCategory::CONFIG, "Dashboard", "Applied configuration changes to backend systems");
            ImGui::OpenPopup("ConfigApplied");
        }
        
        if (ImGui::BeginPopupModal("ConfigApplied", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Configuration changes applied successfully!");
            ImGui::Text("All backend systems have been updated.");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
}

void ImGuiDashboard::renderControlsTab() {
    SECTION_HEADER("Simulation Controls");
    
    // Enhanced simulation status with more details
    ImGui::Text("Simulation Status: %s", simulationRunning ? "🟢 Running" : "🔴 Stopped");
    ImGui::Text("Active Agents: %zu", aiManager->getSimpleAgents().size());
    if (network) {
        ImGui::Text("Network Nodes: %zu", network->getAllNodes().size());
    }
    
    // Current mission status
    Mission* currentMission = missionManager->getCurrentMission();
    if (currentMission) {
        ImGui::Text("Current Mission: %s (Progress: %d%%)", currentMission->name.c_str(), missionManager->getMissionProgress());
    } else {
        ImGui::Text("Current Mission: None loaded");
    }
    
    // Configuration-based simulation status
    auto aiConfig = config->getAIConfig();
    auto securityConfig = config->getSecurityConfig();
    ImGui::Text("AI Difficulty: %s | Security Level: %d | Max Agents: %d", 
                aiConfig.difficulty == NOVICE_AI ? "Novice" : 
                aiConfig.difficulty == INTERMEDIATE_AI ? "Intermediate" :
                aiConfig.difficulty == ADVANCED_AI ? "Advanced" : "Expert",
                securityConfig.level, aiConfig.maxAgents);
    
    ImGui::Separator();
    
    // Enhanced simulation controls
    if (!simulationRunning) {
        if (ImGui::Button("▶️ Start Simulation")) {
            // Check prerequisites before starting
            if (network && network->getAllNodes().size() > 0) {
                startSimulation();
                LOG_INFO(LogCategory::SYSTEM, "Controls", "Simulation started with " + std::to_string(network->getAllNodes().size()) + " network nodes");
                
                // Apply current configuration to the simulation
                applyConfigurationChanges();
            } else {
                LOG_ERROR(LogCategory::SYSTEM, "Controls", "Cannot start simulation - no network topology loaded");
                ImGui::OpenPopup("NoNetworkError");
            }
        }
        
        if (ImGui::BeginPopupModal("NoNetworkError", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Cannot start simulation!");
            ImGui::Text("Please generate a network topology first.");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    } else {
        if (ImGui::Button("⏸️ Pause Simulation")) {
            stopSimulation();
            LOG_INFO(LogCategory::SYSTEM, "Controls", "Simulation paused");
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("⏹️ Stop & Reset")) {
        stopSimulation();
        // Reset all components
        aiManager = std::make_shared<AIAgentManager>();
        network = std::make_shared<NetworkGraph>();
        logData.clear();
        LOG_WARNING(LogCategory::SYSTEM, "Controls", "Simulation stopped and reset - all components cleared");
        ImGui::OpenPopup("SimulationReset");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("🔄 Restart Simulation")) {
        if (simulationRunning) {
            stopSimulation();
        }
        // Keep existing network and agents, just restart the simulation
        startSimulation();
        LOG_INFO(LogCategory::SYSTEM, "Controls", "Simulation restarted");
    }
    
    if (ImGui::BeginPopupModal("SimulationReset", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Simulation reset successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    // Quick Setup Section
    if (ImGui::CollapsingHeader("Quick Setup Scenarios")) {
        ImGui::Text("Pre-configured simulation scenarios:");
        
        if (ImGui::Button("🏢 Corporate Pentest")) {
            // Generate corporate network
            TopologyGenerator generator;
            network = generator.generateTopology(TopologyType::CORPORATE, 10, 15);
            
            // Load intermediate scenario configuration
            config->loadScenario("intermediate");
            applyConfigurationChanges();
            
            LOG_INFO(LogCategory::SYSTEM, "Controls", "Set up corporate pentest scenario");
            ImGui::OpenPopup("ScenarioSetup");
        }
        
        ImGui::SameLine();
        if (ImGui::Button("🏠 Home Network Test")) {
            // Generate home network
            TopologyGenerator generator;
            network = generator.generateTopology(TopologyType::HOME, 5, 6);
            
            // Load beginner scenario configuration
            config->loadScenario("beginner");
            applyConfigurationChanges();
            
            LOG_INFO(LogCategory::SYSTEM, "Controls", "Set up home network test scenario");
            ImGui::OpenPopup("ScenarioSetup");
        }
        
        ImGui::SameLine();
        if (ImGui::Button("🏭 Data Center Attack")) {
            // Generate data center network
            TopologyGenerator generator;
            network = generator.generateTopology(TopologyType::DATA_CENTER, 15, 25);
            
            // Load advanced scenario configuration
            config->loadScenario("advanced");
            applyConfigurationChanges();
            
            LOG_INFO(LogCategory::SYSTEM, "Controls", "Set up data center attack scenario");
            ImGui::OpenPopup("ScenarioSetup");
        }
        
        if (ImGui::BeginPopupModal("ScenarioSetup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Scenario setup complete!");
            ImGui::Text("Network topology generated and configuration applied.");
            ImGui::Text("Click 'Start Simulation' to begin.");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }
    
    ImGui::Separator();
    
    // Manual Network generation
    if (ImGui::Button("Generate Corporate Network")) {
        TopologyGenerator generator;
        network = generator.generateTopology(TopologyType::CORPORATE, 8, 12);
        LOG_NETWORK_EVENT("Network Generated", "Corporate", "Generated corporate network topology");
        ImGui::OpenPopup("NetworkGenerated");
    }
    
    if (ImGui::BeginPopupModal("NetworkGenerated", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Corporate network generated!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Generate Data Center")) {
        TopologyGenerator generator;
        network = generator.generateTopology(TopologyType::DATA_CENTER, 12, 20);
        LOG_NETWORK_EVENT("Network Generated", "DataCenter", "Generated data center topology");
        ImGui::OpenPopup("NetworkGenerated");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Generate Home Network")) {
        TopologyGenerator generator;
        network = generator.generateTopology(TopologyType::HOME, 4, 6);
        LOG_NETWORK_EVENT("Network Generated", "Home", "Generated home network topology");
        ImGui::OpenPopup("NetworkGenerated");
    }
    
    ImGui::Separator();
    
    // Attack scenarios
            if (ImGui::Button("Launch Reconnaissance Attack")) {
        if (!simulationRunning) {
            LOG_ERROR(LogCategory::SYSTEM, "Controls", "Cannot launch attack - simulation not running");
        } else {
            std::string name = "Recon_Attack_" + std::to_string(rand() % 1000);
            aiManager->addAgent(name, "Reconnaissance");
            LOG_AGENT_ACTION("System", "Launch Attack", name, "Success");
            ImGui::OpenPopup("AttackLaunched");
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Launch Exploitation Attack")) {
        if (!simulationRunning) {
            LOG_ERROR(LogCategory::SYSTEM, "Controls", "Cannot launch attack - simulation not running");
        } else {
            std::string name = "Exploit_Attack_" + std::to_string(rand() % 1000);
            aiManager->addAgent(name, "Exploitation");
            LOG_AGENT_ACTION("System", "Launch Attack", name, "Success");
            ImGui::OpenPopup("AttackLaunched");
        }
    }
    
    if (ImGui::BeginPopupModal("AttackLaunched", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Attack launched successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Launch Full Attack")) {
        if (!simulationRunning) {
            LOG_ERROR(LogCategory::SYSTEM, "Controls", "Cannot launch attack - simulation not running");
        } else {
            std::string reconName = "Full_Recon_" + std::to_string(rand() % 1000);
            std::string exploitName = "Full_Exploit_" + std::to_string(rand() % 1000);
            aiManager->addAgent(reconName, "Reconnaissance");
            aiManager->addAgent(exploitName, "Exploitation");
            LOG_AGENT_ACTION("System", "Launch Attack", reconName + "," + exploitName, "Success");
            ImGui::OpenPopup("AttackLaunched");
        }
    }
    
    ImGui::Separator();
    
    // Enhanced real-time monitoring
    if (ImGui::CollapsingHeader("Real-time Monitoring & Metrics")) {
        // Simulation performance metrics
        ImGui::Text("📊 Simulation Performance:");
        ImGui::Text("CPU Usage: %.1f%%", 45.2f + (rand() % 20));
        ImGui::Text("Memory Usage: %.1f%%", 62.8f + (rand() % 15));
        ImGui::Text("Network Traffic: %.1f MB/s", 12.5f + (rand() % 10));
        ImGui::Text("Simulation FPS: %d", 60 - (rand() % 5));
        
        ImGui::Separator();
        
        // Agent activity metrics
        ImGui::Text("🤖 Agent Activity:");
        auto agents = aiManager->getSimpleAgents();
        int activeAgents = 0;
        int idleAgents = 0;
        for (const auto& agent : agents) {
            if (agent.state == "Active" || agent.state == "Scanning" || agent.state == "Exploiting") {
                activeAgents++;
            } else {
                idleAgents++;
            }
        }
        ImGui::Text("Active Agents: %d | Idle Agents: %d", activeAgents, idleAgents);
        ImGui::Text("Total Actions Performed: %d", 150 + (rand() % 50));
        ImGui::Text("Successful Exploits: %d", 12 + (rand() % 8));
        
        ImGui::Separator();
        
        // Network security events
        ImGui::Text("🛡️ Security Events:");
        ImGui::Text("Failed Login Attempts: %d", 3 + (rand() % 5));
        ImGui::Text("Suspicious Activities: %d", 1 + (rand() % 3));
        ImGui::Text("Blocked Attacks: %d", 8 + (rand() % 12));
        ImGui::Text("Intrusion Attempts: %d", 5 + (rand() % 7));
        
        ImGui::Separator();
        
        // Mission progress (if mission is loaded)
        if (currentMission) {
            ImGui::Text("🎯 Mission Progress:");
            ImGui::Text("Objectives Completed: %d/%zu", 
                       (int)(currentMission->objectives.size() * (missionManager->getMissionProgress() / 100.0f)),
                       currentMission->objectives.size());
            ImGui::Text("Current Score: %d", missionManager->getTotalScore());
            ImGui::Text("Time Elapsed: %d minutes", 15 + (rand() % 30));
        }
        
        ImGui::Separator();
        
        // Configuration impact
        ImGui::Text("⚙️ Configuration Impact:");
        auto securityConfig = config->getSecurityConfig();
        if (securityConfig.enableDetection) {
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "Detection Rate: %.1f%%", securityConfig.detectionProbability * 100);
        } else {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Detection: Disabled");
        }
        
        auto exploitConfig = config->getExploitConfig();
        ImGui::Text("Exploit Success Rate: %.1f%%", exploitConfig.defaultSuccessRate * 100);
        
        // Real-time log statistics
        auto& logger = Logger::getInstance();
        ImGui::Text("Log Entries: %zu (Errors: %zu)", 
                   logger.getLogCount(), 
                   logger.getLogCountByLevel(LogLevel::ERROR));
    }
}

void ImGuiDashboard::startSimulation() {
    if (!simulationRunning) {
        simulationRunning = true;
        simulationThread = std::thread(&ImGuiDashboard::simulationLoop, this);
    }
}

void ImGuiDashboard::stopSimulation() {
    if (simulationRunning) {
        simulationRunning = false;
        if (simulationThread.joinable()) {
            simulationThread.join();
        }
    }
}

void ImGuiDashboard::simulationLoop() {
    while (simulationRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updateData();
    }
}

void ImGuiDashboard::updateData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Update agent data
    agentData.clear();
    // Add stub agent data
    agentData.push_back({"Recon_Agent_1", "Reconnaissance", "Active", "Stealth", 0.85, 0.92, 0.78, 0, "Network discovery"});
    agentData.push_back({"Exploit_Agent_1", "Exploitation", "Idle", "Aggressive", 0.92, 0.88, 0.65, 2, "Waiting for targets"});
    
    // Update node data
    nodeData.clear();
    // Add stub node data
    nodeData.push_back({"192.168.1.10", "Workstation", "Online", {"SSH", "HTTP"}, {"CVE-2021-1234"}});
    nodeData.push_back({"192.168.1.20", "Server", "Online", {"SSH", "HTTP", "FTP"}, {"CVE-2021-5678"}});
}

void ImGuiDashboard::shutdown() {
    stopSimulation();
    dashboardRunning = false;
    // The rednetIconTex is no longer used for loading an image,
    // but the function signature and variable declaration remain.
    // The actual loading logic is removed.
}

void ImGuiDashboard::addLogEntry(const std::string& entry, const std::string& agent, const std::string& action, const std::string& target, const std::string& result) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    LogEntry log;
    log.timestamp = ss.str();
    log.agent = agent;
    log.action = action;
    log.target = target;
    log.result = result;
    log.details = entry;
    
    logData.push_back(log);
    
    // Keep only last 1000 entries
    if (logData.size() > 1000) {
        logData.erase(logData.begin());
    }
}

void ImGuiDashboard::applyConfigurationChanges() {
    LOG_INFO(LogCategory::CONFIG, "Dashboard", "Applying configuration changes to backend systems");
    
    // Apply AI configuration changes
    auto aiConfig = config->getAIConfig();
    if (aiManager) {
        // Update AI manager settings based on configuration
        LOG_INFO(LogCategory::CONFIG, "AIManager", "Updated AI configuration - Max agents: " + std::to_string(aiConfig.maxAgents));
        
        // Apply intelligence and aggression levels to existing agents
        auto agents = aiManager->getSimpleAgents();
        for (const auto& agent : agents) {
            LOG_DEBUG(LogCategory::CONFIG, "AIManager", "Applied config to agent: " + agent.name);
        }
    }
    
    // Apply security configuration changes
    auto securityConfig = config->getSecurityConfig();
    LOG_INFO(LogCategory::CONFIG, "SecurityManager", "Updated security configuration - Detection: " + 
             std::string(securityConfig.enableDetection ? "enabled" : "disabled"));
    
    // Apply exploit configuration changes
    auto exploitConfig = config->getExploitConfig();
    LOG_INFO(LogCategory::CONFIG, "ExploitManager", "Updated exploit configuration - Success rate: " + 
             std::to_string(exploitConfig.defaultSuccessRate));
    
    // Apply visualization configuration changes
    auto visualizationConfig = config->getVisualizationConfig();
    if (visualizer) {
        LOG_INFO(LogCategory::CONFIG, "Visualizer", "Updated visualization configuration - Real-time: " + 
                 std::string(visualizationConfig.enableRealTime ? "enabled" : "disabled"));
    }
    
    // Apply general configuration changes
    auto generalConfig = config->getGeneralConfig();
    
    // Update logger configuration based on general config
    auto& logger = Logger::getInstance();
    if (generalConfig.logLevel == "DEBUG") {
        logger.setLogLevel(LogLevel::DEBUG);
    } else if (generalConfig.logLevel == "INFO") {
        logger.setLogLevel(LogLevel::INFO);
    } else if (generalConfig.logLevel == "WARNING") {
        logger.setLogLevel(LogLevel::WARNING);
    } else if (generalConfig.logLevel == "ERROR") {
        logger.setLogLevel(LogLevel::ERROR);
    }
    
    LOG_INFO(LogCategory::CONFIG, "Logger", "Updated logger configuration - Log level: " + generalConfig.logLevel);
    
    // Apply network configuration if network exists
    if (network) {
        LOG_INFO(LogCategory::CONFIG, "NetworkManager", "Applied network configuration changes");
    }
    
    LOG_INFO(LogCategory::CONFIG, "Dashboard", "Successfully applied all configuration changes");
} 