#ifndef REDNET_SHELL_H
#define REDNET_SHELL_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "../engine/Node.h"
#include "../engine/Scanner.h"
#include "../engine/Exploit.h"
#include "../engine/Payload.h"
#include "../network/NetworkGraph.h"
#include "../network/TopologyGenerator.h"
#include "../ui/Visualizer.h"
#include "../utils/Config.h"
#include "../ai/AIAgent.h"
#include "../ui/AdvancedVisualizer.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "AIAgentManager.h"

class Shell {
private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::unique_ptr<Scanner> scanner;
    std::unique_ptr<ExploitDatabase> exploitDB;
    std::unique_ptr<PayloadDatabase> payloadDB;
    std::unique_ptr<NetworkGraph> network;
    std::unique_ptr<TopologyGenerator> topologyGen;
    std::unique_ptr<Visualizer> visualizer;
    
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commands;
    
    // Current session state
    std::string currentTarget;
    std::string currentSession;
    bool isRunning;
    
    std::shared_ptr<ConfigManager> configManager;
    std::shared_ptr<AIAgentManager> aiManager;
    std::shared_ptr<AdvancedVisualizer> advVisualizer;
    
    std::thread simulationThread;
    std::atomic<bool> simulationRunning;
    std::mutex simulationMutex;
    
    // Agent logging and status
    std::vector<std::string> agentLog;
    std::mutex logMutex;
    void logAgentAction(const std::string& action);
    void displayAgentStatus();
    void clearAgentLog();
    
    // Agent control
    void pauseAgents();
    void resumeAgents();
    void showAgentMemory(const std::string& agentName);
    
    // Helper methods
    void initializeCommands();
    void printBanner();
    void printHelp();
    void printPrompt();
    std::vector<std::string> parseCommand(const std::string& input);
    void createDefaultNodes();
    
    // Command implementations
    void cmdHelp(const std::vector<std::string>& args);
    void cmdExit(const std::vector<std::string>& args);
    void cmdClear(const std::vector<std::string>& args);
    void cmdNodes(const std::vector<std::string>& args);
    void cmdScan(const std::vector<std::string>& args);
    void cmdExploit(const std::vector<std::string>& args);
    void cmdPayload(const std::vector<std::string>& args);
    void cmdNetwork(const std::vector<std::string>& args);
    void cmdTopology(const std::vector<std::string>& args);
    void cmdTarget(const std::vector<std::string>& args);
    void cmdSessions(const std::vector<std::string>& args);
    void cmdInfo(const std::vector<std::string>& args);
    void cmdDemo(const std::vector<std::string>& args);
    void cmdVisualize(const std::vector<std::string>& args);
    void cmdAttackPath(const std::vector<std::string>& args);
    void cmdConfig(const std::vector<std::string>& args);
    void cmdScenario(const std::vector<std::string>& args);
    void cmdAgent(const std::vector<std::string>& args);
    void cmdVisMode(const std::vector<std::string>& args);
    void cmdHeatmap(const std::vector<std::string>& args);
    
    void simulationLoop();
    void startSimulation();
    void stopSimulation();

    enum AgentActionType {
        ACTION_SCAN, ACTION_EXPLOIT, ACTION_LATERAL, ACTION_PERSIST, ACTION_COLLAB, ACTION_DETECTION, ACTION_OTHER
    };
    struct AgentLogEntry {
        std::string timestamp;
        std::string agentName;
        AgentActionType actionType;
        std::string action;
        std::string target;
        std::string result;
        std::string details;
    };
    std::vector<AgentLogEntry> advancedAgentLog;
    void logAgentEvent(const AgentLogEntry& entry);
    void exportAgentLog(const std::string& filename);

public:
    Shell(std::shared_ptr<ConfigManager> config,
          std::shared_ptr<AIAgentManager> ai,
          std::shared_ptr<AdvancedVisualizer> vis);
    ~Shell();
    
    void run();
    void stop();
};

#endif // REDNET_SHELL_H 