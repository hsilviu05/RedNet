#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "engine/Node.h"
#include "engine/Scanner.h"
#include "engine/Exploit.h"
#include "engine/Payload.h"
#include "network/NetworkGraph.h"
#include "network/TopologyGenerator.h"
#include "shell/Shell.h"
#include "utils/Config.h"
#include "ai/AIAgentManager.h"
#include "ui/AdvancedVisualizer.h"
#include "ui/desktop/ImGuiDashboard.h"
#include "missions/Mission.h"





int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool useGUI = true; // Default to GUI
    bool useShell = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--shell" || arg == "-s") {
            useShell = true;
            useGUI = false;
        } else if (arg == "--gui" || arg == "-g") {
            useGUI = true;
            useShell = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --gui, -g     Run GUI dashboard (default)\n";
            std::cout << "  --shell, -s   Run command-line shell\n";
            std::cout << "  --help, -h    Show this help message\n";
            return 0;
        }
    }
    
    // Initialize components
    auto config = std::make_shared<ConfigManager>();
    auto network = std::make_shared<NetworkGraph>();
    auto aiManager = std::make_shared<AIAgentManager>();
    auto exploitDB = std::make_shared<ExploitDatabase>();
    auto visualizer = std::make_shared<AdvancedVisualizer>(network, exploitDB);
    auto shell = std::make_shared<Shell>(config, aiManager, visualizer);
    auto missionManager = std::make_shared<MissionManager>();
    
    if (useGUI) {
        std::cout << "🚀 Starting RedNet GUI Dashboard...\n";
        auto dashboard = std::make_shared<ImGuiDashboard>(aiManager, network, config, visualizer, shell, missionManager);
        dashboard->run();
    } else if (useShell) {
        std::cout << "🚀 Starting RedNet Command-Line Shell...\n";
        shell->run();
    }
    
    return 0;
}
