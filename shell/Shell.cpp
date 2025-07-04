#include "Shell.h"
#include "../ai/AIAgentManager.h"
#include "../ui/AdvancedVisualizer.h"
#include "../utils/Config.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

Shell::Shell(std::shared_ptr<ConfigManager> config,
             std::shared_ptr<AIAgentManager> ai,
             std::shared_ptr<AdvancedVisualizer> vis)
    : configManager(config), aiManager(ai), advVisualizer(vis), isRunning(false), simulationRunning(false) {
    scanner = std::make_unique<Scanner>();
    exploitDB = std::make_unique<ExploitDatabase>();
    payloadDB = std::make_unique<PayloadDatabase>();
    network = std::make_unique<NetworkGraph>();
    topologyGen = std::make_unique<TopologyGenerator>();
    visualizer = std::make_unique<Visualizer>();
    initializeCommands();
}

Shell::~Shell() {
    stop();
}

void Shell::initializeCommands() {
    commands["help"] = [this](const std::vector<std::string>& args) { cmdHelp(args); };
    commands["exit"] = [this](const std::vector<std::string>& args) { cmdExit(args); };
    commands["clear"] = [this](const std::vector<std::string>& args) { cmdClear(args); };
    commands["nodes"] = [this](const std::vector<std::string>& args) { cmdNodes(args); };
    commands["scan"] = [this](const std::vector<std::string>& args) { cmdScan(args); };
    commands["exploit"] = [this](const std::vector<std::string>& args) { cmdExploit(args); };
    commands["payload"] = [this](const std::vector<std::string>& args) { cmdPayload(args); };
    commands["network"] = [this](const std::vector<std::string>& args) { cmdNetwork(args); };
    commands["topology"] = [this](const std::vector<std::string>& args) { cmdTopology(args); };
    commands["target"] = [this](const std::vector<std::string>& args) { cmdTarget(args); };
    commands["sessions"] = [this](const std::vector<std::string>& args) { cmdSessions(args); };
    commands["info"] = [this](const std::vector<std::string>& args) { cmdInfo(args); };
    commands["demo"] = [this](const std::vector<std::string>& args) { cmdDemo(args); };
    commands["visualize"] = [this](const std::vector<std::string>& args) { cmdVisualize(args); };
    commands["attackpath"] = [this](const std::vector<std::string>& args) { cmdAttackPath(args); };
    commands["config"] = [this](const std::vector<std::string>& args) { cmdConfig(args); };
    commands["scenario"] = [this](const std::vector<std::string>& args) { cmdScenario(args); };
    commands["agent"] = [this](const std::vector<std::string>& args) { cmdAgent(args); };
    commands["vismode"] = [this](const std::vector<std::string>& args) { cmdVisMode(args); };
    commands["heatmap"] = [this](const std::vector<std::string>& args) { cmdHeatmap(args); };
}

void Shell::printBanner() {
    std::cout << "\n";
    std::cout << "██████╗ ███████╗██████╗ ███╗   ██╗███████╗████████╗\n";
    std::cout << "██╔══██╗██╔════╝██╔══██╗████╗  ██║██╔════╝╚══██╔══╝\n";
    std::cout << "██████╔╝█████╗  ██║  ██║██╔██╗ ██║█████╗     ██║   \n";
    std::cout << "██╔══██╗██╔══╝  ██║  ██║██║╚██╗██║██╔══╝     ██║   \n";
    std::cout << "██║  ██║███████╗██████╔╝██║ ╚████║███████╗   ██║   \n";
    std::cout << "╚═╝  ╚═╝╚══════╝╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝   \n";
    std::cout << "\n";
    std::cout << "AI-Powered Offensive Cybersecurity Simulator\n";
    std::cout << "Version 1.0.0 - Educational Use Only\n";
    std::cout << "=============================================\n\n";
}

void Shell::printHelp() {
    std::cout << "Type 'help' to see available commands.\n";
}

void Shell::printPrompt() {
    std::cout << "rednet> ";
}

std::vector<std::string> Shell::parseCommand(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Shell::run() {
    isRunning = true;
    printBanner();
    printHelp();
    while (isRunning) {
        printPrompt();
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;
        auto tokens = parseCommand(input);
        if (tokens.empty()) continue;
        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        auto it = commands.find(cmd);
        if (it != commands.end()) {
            it->second(tokens);
        } else {
            std::cout << "Unknown command: " << cmd << ". Type 'help' for available commands.\n";
        }
    }
}

void Shell::stop() {
    isRunning = false;
}

// Stub implementations for all command handlers
template<typename T> void ignore(const T&) {}
void Shell::cmdHelp(const std::vector<std::string>& args) { std::cout << "[help] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdExit(const std::vector<std::string>& args) { std::cout << "Exiting shell...\n"; stop(); ignore(args); }
void Shell::cmdClear(const std::vector<std::string>& args) { std::cout << "\033[2J\033[1;1H"; printBanner(); ignore(args); }
void Shell::cmdNodes(const std::vector<std::string>& args) { std::cout << "[nodes] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdScan(const std::vector<std::string>& args) { std::cout << "[scan] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdExploit(const std::vector<std::string>& args) { std::cout << "[exploit] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdPayload(const std::vector<std::string>& args) { std::cout << "[payload] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdNetwork(const std::vector<std::string>& args) { std::cout << "[network] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdTopology(const std::vector<std::string>& args) { std::cout << "[topology] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdTarget(const std::vector<std::string>& args) { std::cout << "[target] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdSessions(const std::vector<std::string>& args) { std::cout << "[sessions] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdInfo(const std::vector<std::string>& args) { std::cout << "[info] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdDemo(const std::vector<std::string>& args) { std::cout << "[demo] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdVisualize(const std::vector<std::string>& args) { std::cout << "[visualize] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdAttackPath(const std::vector<std::string>& args) { std::cout << "[attackpath] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdConfig(const std::vector<std::string>& args) { std::cout << "[config] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdScenario(const std::vector<std::string>& args) { std::cout << "[scenario] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdAgent(const std::vector<std::string>& args) { std::cout << "[agent] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdVisMode(const std::vector<std::string>& args) { std::cout << "[vismode] Command not yet implemented.\n"; ignore(args); }
void Shell::cmdHeatmap(const std::vector<std::string>& args) { std::cout << "[heatmap] Command not yet implemented.\n"; ignore(args); } 