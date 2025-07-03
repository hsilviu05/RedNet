#include "ImGuiDashboard.h"
#include "../../ai/AIAgentManager.h"
#include "../../ai/AIAgent.h"
#include "../../network/NetworkGraph.h"
#include "../../network/TopologyGenerator.h"
#include "../../utils/Config.h"
#include "../AdvancedVisualizer.h"
#include "../../shell/Shell.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

ImGuiDashboard::ImGuiDashboard(std::shared_ptr<AIAgentManager> ai,
                               std::shared_ptr<NetworkGraph> net,
                               std::shared_ptr<ConfigManager> cfg,
                               std::shared_ptr<AdvancedVisualizer> vis,
                               std::shared_ptr<Shell> sh)
    : aiManager(ai), network(net), config(cfg), visualizer(vis), shell(sh) {
    initializeSidebarCategories();
}

ImGuiDashboard::~ImGuiDashboard() {
    shutdown();
}

void ImGuiDashboard::initializeSidebarCategories() {
    sidebarCategories = {
        {"AI Agents", "\xef\x95\x9a", "Manage AI agents and their strategies", true, true, {"Overview", "Configuration", "Performance"}},
        {"Network", "\xef\x83\xa3", "Network topology and discovery", false, true, {"Topology", "Scanning", "Monitoring"}},
        {"Security", "\xef\x94\x92", "Security settings and monitoring", false, true, {"Detection", "Response", "Threats"}},
        {"System", "\xef\x8a\x9e", "System configuration and status", false, true, {"General", "Performance", "Logs"}},
        {"Simulation", "\xef\x8b\x85", "Simulation controls and statistics", false, true, {"Controls", "Statistics", "Export"}}
    };
}

void ImGuiDashboard::applyMacOSStyle(bool darkMode) {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Base style settings
    style.WindowRounding = 12.0f;
    style.FrameRounding = 12.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 12.0f;
    style.ChildRounding = 12.0f;
    style.PopupRounding = 12.0f;
    style.TabRounding = 12.0f;
    
    style.FramePadding = ImVec2(12, 8);
    style.ItemSpacing = ImVec2(14, 10);
    style.WindowPadding = ImVec2(20, 16);
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    
    if (darkMode) {
        ImGui::StyleColorsDark();
        // Dark mode colors
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.95f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.95f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.25f, 0.50f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.18f, 0.90f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.25f, 0.90f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.25f, 0.30f, 0.90f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.18f, 0.90f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.25f, 0.90f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.30f, 0.90f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.15f, 0.90f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.18f, 0.18f, 0.22f, 0.90f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.22f, 0.28f, 0.90f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
    } else {
        ImGui::StyleColorsLight();
        // Light mode colors - macOS style
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.97f, 0.97f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.95f, 0.95f, 0.97f, 0.95f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.85f, 0.85f, 0.88f, 0.50f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.92f, 0.92f, 0.95f, 0.90f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.90f, 0.90f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.85f, 0.90f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.90f, 0.90f, 0.93f, 0.90f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.85f, 0.90f, 0.90f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.80f, 0.85f, 0.90f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.93f, 0.93f, 0.95f, 0.90f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.88f, 0.88f, 0.92f, 0.90f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.85f, 0.85f, 0.90f, 0.90f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
    }
    
    // Blue accent color for selections
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.40f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.40f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.48f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.40f, 0.90f, 1.00f);
}

void ImGuiDashboard::renderMacOSWindowControls() {
    ImGui::SetCursorPos(ImVec2(16.0f, 12.0f));
    float dot_radius = 6.0f;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // Red close button
    draw_list->AddCircleFilled(ImVec2(p.x + dot_radius, p.y + dot_radius), dot_radius, IM_COL32(0xFF, 0x5F, 0x56, 0xFF));
    draw_list->AddCircle(ImVec2(p.x + dot_radius, p.y + dot_radius), dot_radius, IM_COL32(0x00, 0x00, 0x00, 0x20), 0, 1.0f);
    
    // Yellow minimize button
    draw_list->AddCircleFilled(ImVec2(p.x + 3*dot_radius + 4, p.y + dot_radius), dot_radius, IM_COL32(0xFF, 0xBD, 0x2E, 0xFF));
    draw_list->AddCircle(ImVec2(p.x + 3*dot_radius + 4, p.y + dot_radius), dot_radius, IM_COL32(0x00, 0x00, 0x00, 0x20), 0, 1.0f);
    
    // Green maximize button
    draw_list->AddCircleFilled(ImVec2(p.x + 5*dot_radius + 8, p.y + dot_radius), dot_radius, IM_COL32(0x27, 0xC9, 0x42, 0xFF));
    draw_list->AddCircle(ImVec2(p.x + 5*dot_radius + 8, p.y + dot_radius), dot_radius, IM_COL32(0x00, 0x00, 0x00, 0x20), 0, 1.0f);
    
    ImGui::Dummy(ImVec2(60.0f, 20.0f));
}

void ImGuiDashboard::renderSidebar() {
    ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, 0), true);
    
    // Sidebar header
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.7f, 1.0f), "REDNET");
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Categories
    for (size_t i = 0; i < sidebarCategories.size(); i++) {
        auto& category = sidebarCategories[i];
        
        // Category header
        ImGui::PushID(("category_" + std::to_string(i)).c_str());
        
        bool isSelected = (selectedCategoryIndex == static_cast<int>(i));
        ImGui::PushStyleColor(ImGuiCol_Header, isSelected ? 
            ImVec4(0.00f, 0.48f, 1.00f, 0.20f) : ImGui::GetStyle().Colors[ImGuiCol_Header]);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, isSelected ? 
            ImVec4(0.00f, 0.48f, 1.00f, 0.30f) : ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, isSelected ? 
            ImVec4(0.00f, 0.48f, 1.00f, 0.40f) : ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
        
        if (ImGui::CollapsingHeader((category.icon + " " + category.name).c_str(), 
                                   ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap)) {
            category.isExpanded = true;
            
            // Sub-items
            for (size_t j = 0; j < category.subItems.size(); j++) {
                ImGui::Indent(20.0f);
                bool subSelected = (selectedCategoryIndex == static_cast<int>(i) && selectedSubItemIndex == static_cast<int>(j));
                
                if (ImGui::Selectable(category.subItems[j].c_str(), subSelected, 
                                     ImGuiSelectableFlags_AllowItemOverlap)) {
                    selectedCategoryIndex = static_cast<int>(i);
                    selectedSubItemIndex = static_cast<int>(j);
                }
                ImGui::Unindent(20.0f);
            }
        } else {
            category.isExpanded = false;
        }
        
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    }
    
    ImGui::EndChild();
}

void ImGuiDashboard::renderContentArea() {
    ImGui::SameLine();
    ImGui::BeginChild("Content", ImVec2(0, 0), true);
    
    // Content header
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
    if (selectedCategoryIndex < static_cast<int>(sidebarCategories.size())) {
        auto& category = sidebarCategories[selectedCategoryIndex];
        ImGui::Text("%s %s", category.icon.c_str(), category.name.c_str());
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.7f, 1.0f), "%s", category.description.c_str());
    }
    ImGui::Separator();
    ImGui::Spacing();
    
    // Render content based on selected category
    switch (selectedCategoryIndex) {
        case 0: // AI Agents
            renderAITab();
            break;
        case 1: // Network
            renderNetworkTab();
            break;
        case 2: // Security
            renderSecurityTab();
            break;
        case 3: // System
            renderSystemTab();
            break;
        case 4: // Simulation
            renderControlsTab();
            break;
        default:
            ImGui::Text("Select a category from the sidebar");
            break;
    }
    
    ImGui::EndChild();
}

void ImGuiDashboard::run() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return;
    }

    // Create window with macOS-style properties
    GLFWwindow* window = glfwCreateWindow(1400, 900, "RedNet", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
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

    // Simple dark mode detection (no Objective-C)
    bool darkMode = false;
#ifdef __APPLE__
    // Use a simple heuristic for dark mode
    darkMode = true; // Default to dark mode for macOS
#endif

    // Apply macOS style
    applyMacOSStyle(darkMode);

    // Load fonts
    ImFontConfig font_cfg;
    font_cfg.OversampleH = font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
    bool fontLoaded = false;
#ifdef __APPLE__
    fontLoaded = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/SFNS.ttf", 16.0f, &font_cfg) != nullptr;
    if (!fontLoaded)
        fontLoaded = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/HelveticaNeue.ttc", 16.0f, &font_cfg) != nullptr;
#endif
    if (!fontLoaded)
        io.Fonts->AddFontDefault();

    // Load FontAwesome icon font
    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("fonts/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);

    // Setup Platform/Renderer backends
#ifdef __APPLE__
    const char* glsl_version = "#version 120";
#else
    const char* glsl_version = "#version 130";
#endif
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // macOS-style menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("RedNet")) {
                if (ImGui::MenuItem("About RedNet")) {
                    ImGui::OpenPopup("AboutPopup");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Quit RedNet", "Cmd+Q")) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Toggle Sidebar", "Cmd+S")) {
                    sidebarCollapsed = !sidebarCollapsed;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (ImGui::BeginPopupModal("AboutPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("RedNet\nAI Cyber Range Simulator\n\nVersion 1.0");
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
    ImVec2 size = ImGui::GetIO().DisplaySize;
    
    // Force window to be uncollapsed
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("RedNet Dashboard", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

    // macOS window controls
    renderMacOSWindowControls();

    // Main layout
    if (!sidebarCollapsed) {
        renderSidebar();
        renderContentArea();
    } else {
        renderContentArea();
    }

    ImGui::End();
}

void ImGuiDashboard::shutdown() {
    // Cleanup if needed
}

void ImGuiDashboard::addLogEntry(const std::string& entry) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] " << entry;
    logBuffer.push_back(ss.str());
    
    // Keep only last 1000 entries
    if (logBuffer.size() > 1000) {
        logBuffer.pop_front();
    }
}

void ImGuiDashboard::renderAITab() {
    // Overview section
    if (ImGui::CollapsingHeader("Overview", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        // Statistics cards
        ImGui::BeginChild("StatsCards", ImVec2(0, 120), true);
        
        // Active agents card
        ImGui::BeginGroup();
        ImGui::Text("\xef\x95\x9a  Active Agents");
        ImGui::Text("%d", aiManager ? static_cast<int>(aiManager->getAllAgents().size()) : 0);
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Success rate card
        ImGui::BeginGroup();
        ImGui::Text("\xef\x87\x9b  Success Rate");
        ImGui::Text("85%%");
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Intelligence card
        ImGui::BeginGroup();
        ImGui::Text("\xef\x87\x8f  Avg Intelligence");
        ImGui::Text("78%%");
        ImGui::EndGroup();
        
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    
    // Agent management
    if (ImGui::CollapsingHeader("Agent Management", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        if (ImGui::Button("\xef\x81\x9e  Add Recon Agent")) {
            if (aiManager) {
                auto agent = std::make_shared<ReconAgent>("GUI_Recon");
                aiManager->addAgent(agent);
                addLogEntry("Added new ReconAgent: GUI_Recon");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x87\x8f  Add Exploit Agent")) {
            if (aiManager) {
                auto agent = std::make_shared<ExploitAgent>("GUI_Exploit");
                aiManager->addAgent(agent);
                addLogEntry("Added new ExploitAgent: GUI_Exploit");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x81\xbb  Clear All")) {
            if (aiManager) {
                auto agents = aiManager->getAllAgents();
                int count = static_cast<int>(agents.size());
                for (const auto& agent : agents) {
                    aiManager->removeAgent(agent->getName());
                }
                addLogEntry("Cleared " + std::to_string(count) + " agents");
            }
        }
        
        ImGui::EndGroup();
        
        ImGui::Separator();
        
        // Agent table
        if (ImGui::BeginTable("Agents", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Intelligence");
            ImGui::TableSetupColumn("Success Rate");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();
            
            if (aiManager) {
                auto agents = aiManager->getAllAgents();
                for (const auto& agent : agents) {
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", agent->getName().c_str());
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("AI Agent");
                    
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("Active");
                    
                    ImGui::TableSetColumnIndex(3);
                    ImGui::ProgressBar(agent->getIntelligence(), ImVec2(80, 0));
                    ImGui::SameLine();
                    ImGui::Text("%.0f%%", agent->getIntelligence() * 100);
                    
                    ImGui::TableSetColumnIndex(4);
                    ImGui::ProgressBar(agent->getSuccessRate(), ImVec2(80, 0));
                    ImGui::SameLine();
                    ImGui::Text("%.0f%%", agent->getSuccessRate() * 100);
                    
                    ImGui::TableSetColumnIndex(5);
                    if (ImGui::Button("Stop")) {
                        aiManager->removeAgent(agent->getName());
                    }
                }
            }
            ImGui::EndTable();
        }
    }
}

void ImGuiDashboard::renderNetworkTab() {
    // Network controls
    if (ImGui::CollapsingHeader("Network Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        if (ImGui::Button("\xef\x83\xa3  Generate Topology")) {
            if (network) {
                TopologyGenerator generator;
                auto newNetwork = std::make_shared<NetworkGraph>(generator.generateTopology(HYBRID_TOPOLOGY, 15));
                network = newNetwork;
                addLogEntry("Generated new hybrid topology with 15 nodes");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x87\x9b  Scan Network")) {
            if (network) {
                auto discoveredNodes = network->discoverNetwork("192.168.1.1");
                for (const auto& ip : discoveredNodes) {
                    if (!network->nodeExists(ip)) {
                        NetworkNode newNode(ip, nullptr);
                        network->addNode(newNode);
                    }
                }
                addLogEntry("Network scan completed - discovered " + std::to_string(discoveredNodes.size()) + " nodes");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x8a\x9e  Monitor")) {
            addLogEntry("Started network monitoring");
        }
        
        ImGui::EndGroup();
    }
    
    // Network topology
    if (ImGui::CollapsingHeader("Network Topology", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Network statistics
        ImGui::BeginGroup();
        ImGui::Text("Total Nodes: %d", network ? network->getNodeCount() : 0);
        ImGui::SameLine();
        ImGui::Text("Active Connections: %d", 0);
        ImGui::SameLine();
        ImGui::Text("Vulnerable Nodes: %d", 0);
        ImGui::EndGroup();
        
        ImGui::Separator();
        
        // Network nodes table
        if (ImGui::BeginTable("NetworkNodes", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Node ID");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Status");
            ImGui::TableSetupColumn("Services");
            ImGui::TableSetupColumn("Vulnerabilities");
            ImGui::TableHeadersRow();
            
            if (network) {
                auto nodes = network->getAllNodes();
                for (const auto& node : nodes) {
                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", node.ip.c_str());
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("Host");
                    
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextColored(ImVec4(0.27f, 0.80f, 0.42f, 1.0f), "Online");
                    
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("SSH, HTTP, FTP");
                    
                    ImGui::TableSetColumnIndex(4);
                    ImGui::TextColored(ImVec4(0.95f, 0.37f, 0.37f, 1.0f), "3");
                }
            }
            ImGui::EndTable();
        }
    }
}

void ImGuiDashboard::renderSecurityTab() {
    // Security overview
    if (ImGui::CollapsingHeader("Security Overview", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        // Security status cards
        ImGui::BeginChild("SecurityCards", ImVec2(0, 120), true);
        
        // Threat level
        ImGui::BeginGroup();
        ImGui::Text("\xef\x94\x92  Threat Level");
        ImGui::TextColored(ImVec4(0.95f, 0.37f, 0.37f, 1.0f), "HIGH");
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Detections
        ImGui::BeginGroup();
        ImGui::Text("\xef\x87\x9b  Detections");
        ImGui::Text("12");
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Response time
        ImGui::BeginGroup();
        ImGui::Text("\xef\x8b\x85  Response Time");
        ImGui::Text("2.3s");
        ImGui::EndGroup();
        
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    
    // Security settings
    if (ImGui::CollapsingHeader("Security Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        static bool enableDetection = true;
        static bool enableResponse = true;
        static float detectionSensitivity = 0.7f;
        static float responseTime = 5.0f;
        
        ImGui::Checkbox("Enable Intrusion Detection", &enableDetection);
        ImGui::Checkbox("Enable Automated Response", &enableResponse);
        ImGui::SliderFloat("Detection Sensitivity", &detectionSensitivity, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Response Time (seconds)", &responseTime, 1.0f, 30.0f, "%.1f");
    }
    
    // Threat feed
    if (ImGui::CollapsingHeader("Recent Threats", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginTable("Threats", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Threat Type");
            ImGui::TableSetupColumn("Target");
            ImGui::TableSetupColumn("Status");
            ImGui::TableHeadersRow();
            
            // Sample threat data
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("10:30:15");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.95f, 0.37f, 0.37f, 1.0f), "SQL Injection");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("192.168.1.10:80");
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(ImVec4(0.27f, 0.80f, 0.42f, 1.0f), "Blocked");
            
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("10:29:45");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(0.95f, 0.37f, 0.37f, 1.0f), "Port Scan");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("192.168.1.15");
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(ImVec4(0.95f, 0.65f, 0.18f, 1.0f), "Alerted");
            
            ImGui::EndTable();
        }
    }
}

void ImGuiDashboard::renderSystemTab() {
    // System status
    if (ImGui::CollapsingHeader("System Status", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        // System metrics
        ImGui::BeginChild("SystemMetrics", ImVec2(0, 120), true);
        
        // CPU usage
        ImGui::BeginGroup();
        ImGui::Text("\xef\x8a\x9e  CPU Usage");
        ImGui::ProgressBar(0.45f, ImVec2(100, 0));
        ImGui::Text("45%%");
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Memory usage
        ImGui::BeginGroup();
        ImGui::Text("\xef\x8b\x85  Memory Usage");
        ImGui::ProgressBar(0.62f, ImVec2(100, 0));
        ImGui::Text("62%%");
        ImGui::EndGroup();
        ImGui::SameLine();
        
        // Network traffic
        ImGui::BeginGroup();
        ImGui::Text("\xef\x83\xa3  Network Traffic");
        ImGui::ProgressBar(0.28f, ImVec2(100, 0));
        ImGui::Text("28%%");
        ImGui::EndGroup();
        
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    
    // Configuration
    if (ImGui::CollapsingHeader("Configuration", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int maxAgents = 10;
        static int maxNodes = 50;
        static bool autoSave = true;
        static bool debugMode = false;
        
        ImGui::SliderInt("Maximum Agents", &maxAgents, 1, 50);
        ImGui::SliderInt("Maximum Nodes", &maxNodes, 10, 200);
        ImGui::Checkbox("Auto-save Configuration", &autoSave);
        ImGui::Checkbox("Debug Mode", &debugMode);
    }
    
    // Logs
    if (ImGui::CollapsingHeader("System Logs", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        if (ImGui::Button("\xef\x81\xbb  Clear Logs")) {
            logBuffer.clear();
            addLogEntry("Logs cleared by user");
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x87\x8a  Export Logs")) {
            addLogEntry("Log export requested");
        }
        ImGui::SameLine();
        static bool autoScroll = true;
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        
        ImGui::EndGroup();
        
        ImGui::Separator();
        
        // Log entries
        ImGui::BeginChild("LogEntries", ImVec2(0, 300), true);
        
        if (logBuffer.empty()) {
            ImGui::Text("[2024-01-15 10:30:15] System: RedNet initialized successfully");
            ImGui::Text("[2024-01-15 10:30:20] Network: Topology generated with 15 nodes");
            ImGui::Text("[2024-01-15 10:30:25] AI: Agent pool initialized");
            ImGui::Text("[2024-01-15 10:30:30] Security: Intrusion detection system active");
        } else {
            for (const auto& logEntry : logBuffer) {
                ImGui::Text("%s", logEntry.c_str());
            }
        }
        
        if (autoScroll) {
            ImGui::SetScrollHereY(1.0f);
        }
        
        ImGui::EndChild();
    }
}

void ImGuiDashboard::renderControlsTab() {
    // Simulation controls
    if (ImGui::CollapsingHeader("Simulation Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        if (ImGui::Button("\xef\x8b\x85  Start Simulation", ImVec2(200, 40))) {
            if (aiManager) {
                aiManager->startAllAgents();
                addLogEntry("Started simulation - all agents activated");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x8a\x9e  Pause Simulation", ImVec2(200, 40))) {
            if (aiManager) {
                aiManager->pauseAllAgents();
                addLogEntry("Simulation paused - all agents suspended");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x81\xbb  Stop Simulation", ImVec2(200, 40))) {
            if (aiManager) {
                aiManager->stopAllAgents();
                addLogEntry("Simulation stopped - all agents terminated");
            }
        }
        
        ImGui::EndGroup();
        
        ImGui::Separator();
        
        // Advanced controls
        ImGui::BeginGroup();
        
        if (ImGui::Button("\xef\x83\xa3  Generate Network", ImVec2(200, 30))) {
            if (network) {
                TopologyGenerator generator;
                auto newNetwork = std::make_shared<NetworkGraph>(generator.generateCorporateNetwork());
                network = newNetwork;
                addLogEntry("Generated new corporate network topology");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x87\x9b  Run Full Scan", ImVec2(200, 30))) {
            if (network && aiManager) {
                auto reconAgent = std::make_shared<ReconAgent>("FullScan_Agent");
                aiManager->addAgent(reconAgent);
                addLogEntry("Created FullScan_Agent for comprehensive network scan");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("\xef\x94\x92  Launch Attack", ImVec2(200, 30))) {
            if (network && aiManager) {
                auto reconAgent = std::make_shared<ReconAgent>("Attack_Recon");
                auto exploitAgent = std::make_shared<ExploitAgent>("Attack_Exploit");
                auto persistenceAgent = std::make_shared<PersistenceAgent>("Attack_Persist");
                
                aiManager->addAgent(reconAgent);
                aiManager->addAgent(exploitAgent);
                aiManager->addAgent(persistenceAgent);
                
                aiManager->startAllAgents();
                addLogEntry("Launched coordinated attack with 3 agents");
            }
        }
        
        ImGui::EndGroup();
    }
    
    // Statistics
    if (ImGui::CollapsingHeader("Simulation Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
        
        ImGui::Text("Active Agents: %d", aiManager ? aiManager->getActiveAgentCount() : 0);
        ImGui::Text("Total Nodes: %d", network ? network->getNodeCount() : 0);
        ImGui::Text("Compromised Nodes: 0");
        ImGui::Text("Detection Events: 0");
        ImGui::Text("Simulation Time: 00:05:23");
        
        ImGui::EndGroup();
    }
}

// Legacy tab methods for compatibility
void ImGuiDashboard::renderAgentTab() {
    // This is now handled by renderAITab()
}

void ImGuiDashboard::renderLogsTab() {
    // This is now handled by the System section in the sidebar
}

void ImGuiDashboard::renderConfigTab() {
    // This is now handled by the System section in the sidebar
} 