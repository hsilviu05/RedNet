#include "ImGuiDashboard.h"
#include "../../ai/AIAgentManager.h"
#include "../../ai/AIAgent.h"
#include "../../network/NetworkGraph.h"
#include "../../network/TopologyGenerator.h"
#include "../../utils/Config.h"
#include "../AdvancedVisualizer.h"
#include "../../shell/Shell.h"
#include "../../missions/Mission.h"
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
    printf("Attempting to load RedNet.png icon...\n");
    if (rednetIconTex == 0) {
        rednetIconTex = LoadTextureFromFile("../resources/RedNet.png", &rednetIconWidth, &rednetIconHeight);
    if (rednetIconTex == 0) {
        rednetIconTex = LoadTextureFromFile("resources/RedNet.png", &rednetIconWidth, &rednetIconHeight);
    }
    if (rednetIconTex == 0) {
        printf("Failed to load RedNet.png as OpenGL texture!\n");
    } else {
        printf("Loaded RedNet.png as OpenGL texture: %d\n", rednetIconTex);
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

    // After ImGui/OpenGL context creation, before the main loop in run():
    printf("Attempting to load RedNet.png icon...\n");
    if (rednetIconTex == 0) {
        rednetIconTex = LoadTextureFromFile("../resources/RedNet.png", &rednetIconWidth, &rednetIconHeight);
        if (rednetIconTex == 0) {
            rednetIconTex = LoadTextureFromFile("resources/RedNet.png", &rednetIconWidth, &rednetIconHeight);
        }
        if (rednetIconTex == 0) {
            printf("Failed to load RedNet.png as OpenGL texture!\n");
        } else {
            printf("Loaded RedNet.png as OpenGL texture: %d\n", rednetIconTex);
        }
    }

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
                    ImGui::OpenPopup("AboutPopup");
                }
                if (ImGui::MenuItem("Quit RedNet", "Cmd+Q")) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        if (ImGui::BeginPopupModal("AboutPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("RedNet\nAI Cyber Range Simulator\n\nVersion 1.0\n\nCreated by Silviu");
            if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
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
    
    // Draw RedNet icon in header
    if (rednetIconTex) {
        ImGui::Image((void*)(intptr_t)rednetIconTex, ImVec2(32, 32));
        ImGui::SameLine();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.10f, 0.15f, 1.0f));
        ImGui::Text("🔴");
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }
    ImGui::Text("RedNet Dashboard");
    
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
    ImGui::SetCursorPosY(10);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 80);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.00f, 0.00f, 1.0f));
    ImGui::Text("%s", "*** Welcome to RedNet! ***");
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

    // Debug: Draw plain and colored text at the top of the window
    ImGui::Text("PLAIN TEXT TEST");
    ImGui::TextColored(ImVec4(1,0,0,1), "RED TEXT TEST");
    // Print style alpha and text color alpha to console
    ImGuiStyle& style = ImGui::GetStyle();
    printf("ImGui style alpha: %f\n", style.Alpha);
    printf("ImGui text color alpha: %f\n", style.Colors[ImGuiCol_Text].w);
}

void ImGuiDashboard::renderAgentTab() {
    SECTION_HEADER("AI Agent Management");
    static int reconAgentCount = 1;
    static int exploitAgentCount = 1;
    // Agent controls
    if (ImGui::Button("Create Recon Agent")) {
        std::string name = "Recon_Agent_" + std::to_string(reconAgentCount++);
        aiManager->addAgent(name, "Reconnaissance");
        addLogEntry("Created new reconnaissance agent: " + name, "System", "Create Agent", name, "Success");
        ImGui::OpenPopup("AgentCreated");
    }
    if (ImGui::Button("Create Exploit Agent")) {
        std::string name = "Exploit_Agent_" + std::to_string(exploitAgentCount++);
        aiManager->addAgent(name, "Exploitation");
        addLogEntry("Created new exploitation agent: " + name, "System", "Create Agent", name, "Success");
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
                addLogEntry("Stopped and removed agent: " + agent.name, "System", "Stop Agent", agent.name, "Info");
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
        addLogEntry("Generated new network topology with " + std::to_string(network->getAllNodes().size()) + " nodes", "System", "Generate Topology", "Network", "Success");
        ImGui::OpenPopup("TopologyGenerated");
    }
    if (ImGui::BeginPopupModal("TopologyGenerated", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New topology generated!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    if (ImGui::Button("Scan Network")) {
        addLogEntry("Network scan initiated", "System", "Scan Network", "Network", "Info");
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
                missionManager->loadMissionFromJson(""); // Load the selected mission
                addLogEntry("Loaded mission: " + mission.name, "System", "Load Mission", mission.id, "Info");
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
            addLogEntry("Started mission: " + currentMission->name, "System", "Start Mission", currentMission->id, "Success");
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
    static bool showInfo = true;
    static bool showWarning = true;
    static bool showError = true;
    
    // Log controls
    if (ImGui::Button("Clear Logs")) {
        logData.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Test Log")) {
        addLogEntry("Test log entry added manually");
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Agent Log")) {
        addLogEntry("Recon_Agent_1 discovered new host 192.168.1.15");
    }
    
    // Filter controls
    ImGui::Separator();
    ImGui::Text("Filters:");
    ImGui::SameLine();
    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &showWarning);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &showError);
    ImGui::SameLine();
    ImGui::InputText("Search", searchFilter, sizeof(searchFilter));
    
    ImGui::Separator();
    ImGui::Text("System Activity Log:");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.98f, 0.98f, 0.99f, 1.0f));
    ImGui::BeginChild("Logs", ImVec2(0, 0), true, ImGuiWindowFlags_None);
    
    // Add some initial log entries if empty
    if (logData.empty()) {
        addLogEntry("RedNet system initialized");
        addLogEntry("GUI dashboard started successfully");
        addLogEntry("Network topology generator ready");
        addLogEntry("Mission system loaded");
    }
    
    for (const auto& log : logData) {
        // Apply filters
        bool shouldShow = true;
        if (!showInfo && log.result == "Info") shouldShow = false;
        if (!showWarning && log.result == "Warning") shouldShow = false;
        if (!showError && log.result == "Error") shouldShow = false;
        
        if (!shouldShow) continue;
        
        // Apply search filter
        if (strlen(searchFilter) > 0) {
            std::string logText = log.timestamp + " " + log.agent + " " + log.action + " " + log.target + " " + log.result + " " + log.details;
            if (logText.find(searchFilter) == std::string::npos) continue;
        }
        
        // Color code by log level
        ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        if (log.result == "Warning") textColor = ImVec4(0.8f, 0.6f, 0.0f, 1.0f);
        else if (log.result == "Error") textColor = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);
        else if (log.result == "Success") textColor = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
        
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        ImGui::Text("[%s] %s: %s", log.timestamp.c_str(), log.agent.c_str(), log.details.c_str());
        ImGui::PopStyleColor();
        
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Action: %s\nTarget: %s\nResult: %s", log.action.c_str(), log.target.c_str(), log.result.c_str());
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
        addLogEntry("Loaded default configuration", "System", "Load Config", "Default", "Info");
        configChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset to Defaults")) {
        config->resetToDefaults();
        addLogEntry("Reset configuration to defaults", "System", "Reset Config", "All", "Warning");
        configChanged = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Configuration")) {
        if (config->saveConfig()) {
            addLogEntry("Configuration saved successfully", "System", "Save Config", "File", "Success");
            ImGui::OpenPopup("ConfigSaved");
        } else {
            addLogEntry("Failed to save configuration", "System", "Save Config", "File", "Error");
        }
    }
    
    if (ImGui::BeginPopupModal("ConfigSaved", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Configuration saved successfully!");
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
    }
}

void ImGuiDashboard::renderControlsTab() {
    SECTION_HEADER("Simulation Controls");
    
    // Simulation status
    ImGui::Text("Simulation Status: %s", simulationRunning ? "Running" : "Stopped");
    ImGui::Text("Active Agents: %zu", aiManager->getSimpleAgents().size());
    if (network) {
        ImGui::Text("Network Nodes: %zu", network->getAllNodes().size());
    }
    
    ImGui::Separator();
    
    // Simulation controls
    if (!simulationRunning) {
        if (ImGui::Button("Start Simulation")) {
            startSimulation();
            addLogEntry("Simulation started", "System", "Start Simulation", "All", "Success");
        }
    } else {
        if (ImGui::Button("Stop Simulation")) {
            stopSimulation();
            addLogEntry("Simulation stopped", "System", "Stop Simulation", "All", "Info");
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Reset Simulation")) {
        stopSimulation();
        // Reset all components
        aiManager = std::make_shared<AIAgentManager>();
        network = std::make_shared<NetworkGraph>();
        logData.clear();
        addLogEntry("Simulation reset - all components cleared", "System", "Reset Simulation", "All", "Warning");
        ImGui::OpenPopup("SimulationReset");
    }
    
    if (ImGui::BeginPopupModal("SimulationReset", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Simulation reset successfully!");
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::Separator();
    
    // Network generation
    if (ImGui::Button("Generate Corporate Network")) {
        TopologyGenerator generator;
        network = generator.generateTopology(TopologyType::CORPORATE, 8, 12);
        addLogEntry("Generated corporate network topology", "System", "Generate Network", "Corporate", "Success");
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
        addLogEntry("Generated data center topology", "System", "Generate Network", "Data Center", "Success");
        ImGui::OpenPopup("NetworkGenerated");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Generate Home Network")) {
        TopologyGenerator generator;
        network = generator.generateTopology(TopologyType::HOME, 4, 6);
        addLogEntry("Generated home network topology", "System", "Generate Network", "Home", "Success");
        ImGui::OpenPopup("NetworkGenerated");
    }
    
    ImGui::Separator();
    
    // Attack scenarios
    if (ImGui::Button("Launch Reconnaissance Attack")) {
        if (!simulationRunning) {
            addLogEntry("Cannot launch attack - simulation not running", "System", "Launch Attack", "Recon", "Error");
        } else {
            std::string name = "Recon_Attack_" + std::to_string(rand() % 1000);
            aiManager->addAgent(name, "Reconnaissance");
            addLogEntry("Launched reconnaissance attack with agent: " + name, "System", "Launch Attack", "Recon", "Success");
            ImGui::OpenPopup("AttackLaunched");
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Launch Exploitation Attack")) {
        if (!simulationRunning) {
            addLogEntry("Cannot launch attack - simulation not running", "System", "Launch Attack", "Exploit", "Error");
        } else {
            std::string name = "Exploit_Attack_" + std::to_string(rand() % 1000);
            aiManager->addAgent(name, "Exploitation");
            addLogEntry("Launched exploitation attack with agent: " + name, "System", "Launch Attack", "Exploit", "Success");
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
            addLogEntry("Cannot launch attack - simulation not running", "System", "Launch Attack", "Full", "Error");
        } else {
            std::string reconName = "Full_Recon_" + std::to_string(rand() % 1000);
            std::string exploitName = "Full_Exploit_" + std::to_string(rand() % 1000);
            aiManager->addAgent(reconName, "Reconnaissance");
            aiManager->addAgent(exploitName, "Exploitation");
            addLogEntry("Launched full attack with agents: " + reconName + ", " + exploitName, "System", "Launch Attack", "Full", "Success");
            ImGui::OpenPopup("AttackLaunched");
        }
    }
    
    ImGui::Separator();
    
    // Real-time monitoring
    if (ImGui::CollapsingHeader("Real-time Monitoring")) {
        ImGui::Text("System Metrics:");
        ImGui::Text("CPU Usage: %.1f%%", 45.2f + (rand() % 20));
        ImGui::Text("Memory Usage: %.1f%%", 62.8f + (rand() % 15));
        ImGui::Text("Network Traffic: %.1f MB/s", 12.5f + (rand() % 10));
        ImGui::Text("Active Connections: %d", 15 + (rand() % 20));
        
        ImGui::Separator();
        ImGui::Text("Security Events:");
        ImGui::Text("Failed Login Attempts: %d", 3 + (rand() % 5));
        ImGui::Text("Suspicious Activities: %d", 1 + (rand() % 3));
        ImGui::Text("Blocked Attacks: %d", 8 + (rand() % 12));
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