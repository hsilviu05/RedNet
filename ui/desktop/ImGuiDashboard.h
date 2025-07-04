#pragma once
#include <memory>
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <deque>
#include "ai/AIAgentManager.h"
#include "utils/Config.h"
#include "../../network/NetworkGraph.h"
#include "../../network/TopologyGenerator.h"
#include "../../missions/Mission.h"
#include "../../utils/Logger.h"

// Forward declarations
class NetworkGraph;
class ConfigManager;
class AdvancedVisualizer;
class Shell;

class ImGuiDashboard {
private:
    // Core components
    std::shared_ptr<AIAgentManager> aiManager;
    std::shared_ptr<NetworkGraph> network;
    std::shared_ptr<ConfigManager> config;
    std::shared_ptr<AdvancedVisualizer> visualizer;
    std::shared_ptr<Shell> shell;
    std::shared_ptr<MissionManager> missionManager;
    
    // UI state
    std::atomic<bool> simulationRunning{false};
    std::atomic<bool> dashboardRunning{true};
    std::thread simulationThread;
    std::mutex dataMutex;
    bool showAboutDialog = false;
    
    // Agent data
    struct AgentData {
        std::string name;
        std::string type;
        std::string state;
        std::string strategy;
        double intelligence;
        double successRate;
        double stealthLevel;
        int detectionCount;
        std::string currentGoal;
    };
    std::vector<AgentData> agentData;
    
    // Network data
    struct NodeData {
        std::string id;
        std::string type;
        std::string status;
        std::vector<std::string> services;
        std::vector<std::string> vulnerabilities;
    };
    std::vector<NodeData> nodeData;
    
    // Log data
    struct LogEntry {
        std::string timestamp;
        std::string agent;
        std::string action;
        std::string target;
        std::string result;
        std::string details;
    };
    std::vector<LogEntry> logData;
    std::deque<std::string> logBuffer; // Simple log buffer for clearing
    
    // UI helpers
    void renderAgentTab();
    void renderNetworkTab();
    void renderMissionsTab();
    void renderLogsTab();
    void renderConfigTab();
    void renderControlsTab();
    void updateData();
    void startSimulation();
    void stopSimulation();
    void simulationLoop();
    void addLogEntry(const std::string& entry, const std::string& agent = "System", const std::string& action = "Log", const std::string& target = "Dashboard", const std::string& result = "Info");
    void applyConfigurationChanges();

public:
    ImGuiDashboard(std::shared_ptr<AIAgentManager> ai,
                   std::shared_ptr<NetworkGraph> net,
                   std::shared_ptr<ConfigManager> cfg,
                   std::shared_ptr<AdvancedVisualizer> vis,
                   std::shared_ptr<Shell> sh,
                   std::shared_ptr<MissionManager> missions);
    ~ImGuiDashboard();
    
    void run();
    void update();
    void shutdown();
    void stop() { dashboardRunning = false; }
}; 