#include "Shell.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>

Shell::Shell(std::shared_ptr<ConfigManager> config,
           std::shared_ptr<AIAgentManager> ai,
           std::shared_ptr<AdvancedVisualizer> vis)
    : isRunning(false), configManager(config), aiManager(ai), advVisualizer(vis) {
    // Initialize components
    scanner = std::make_unique<Scanner>();
    exploitDB = std::make_unique<ExploitDatabase>();
    payloadDB = std::make_unique<PayloadDatabase>();
    network = std::make_unique<NetworkGraph>();
    topologyGen = std::make_unique<TopologyGenerator>();
    visualizer = std::make_unique<Visualizer>();
    
    // Create some default nodes
    createDefaultNodes();
    
    // Initialize command map
    initializeCommands();
}

Shell::~Shell() = default;

void Shell::createDefaultNodes() {
    // Create a small test network
    auto node1 = std::make_shared<Node>("192.168.1.100", LINUX);
    node1->hostname = "web-server";
    auto node2 = std::make_shared<Node>("192.168.1.101", LINUX);
    node2->hostname = "file-server";
    auto node3 = std::make_shared<Node>("192.168.1.102", LINUX);
    node3->hostname = "database-server";
    auto node4 = std::make_shared<Node>("192.168.1.103", LINUX);
    node4->hostname = "workstation";
    
    nodes.push_back(node1);
    nodes.push_back(node2);
    nodes.push_back(node3);
    nodes.push_back(node4);
    
    // Add nodes to network
    for (const auto& node : nodes) {
        network->addNode(NetworkNode(node->ip, node));
    }
    
    // Add some links
    network->addLink(NetworkLink("192.168.1.100", "192.168.1.101"));
    network->addLink(NetworkLink("192.168.1.101", "192.168.1.102"));
    network->addLink(NetworkLink("192.168.1.102", "192.168.1.103"));
}

void Shell::initializeCommands() {
    commands["help"] = [this](const std::vector<std::string>& args) { cmdHelp(args); };
    commands["exit"] = [this](const std::vector<std::string>& args) { cmdExit(args); };
    commands["quit"] = [this](const std::vector<std::string>& args) { cmdExit(args); };
    commands["clear"] = [this](const std::vector<std::string>& args) { cmdClear(args); };
    commands["cls"] = [this](const std::vector<std::string>& args) { cmdClear(args); };
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
    commands["viz"] = [this](const std::vector<std::string>& args) { cmdVisualize(args); };
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
    std::cout << "Interactive Shell - Version 1.0.0\n";
    std::cout << "Type 'help' for available commands\n";
    std::cout << "Type 'exit' to quit\n";
    std::cout << "=============================================\n\n";
}

void Shell::printPrompt() {
    std::string prompt = "RedNet";
    if (!currentTarget.empty()) {
        prompt += " [" + currentTarget + "]";
    }
    if (!currentSession.empty()) {
        prompt += " (session:" + currentSession + ")";
    }
    prompt += " > ";
    std::cout << prompt;
}

std::vector<std::string> Shell::parseCommand(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    return args;
}

void Shell::startSimulation() {
    simulationRunning = true;
    simulationThread = std::thread([this]() { simulationLoop(); });
}

void Shell::stopSimulation() {
    simulationRunning = false;
    if (simulationThread.joinable()) simulationThread.join();
}

void Shell::logAgentEvent(const AgentLogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    advancedAgentLog.push_back(entry);
    // Print summary to shell
    std::cout << "\n[" << entry.timestamp << "] " << entry.agentName << " [";
    switch (entry.actionType) {
        case ACTION_SCAN: std::cout << "SCAN"; break;
        case ACTION_EXPLOIT: std::cout << "EXPLOIT"; break;
        case ACTION_LATERAL: std::cout << "LATERAL"; break;
        case ACTION_PERSIST: std::cout << "PERSIST"; break;
        case ACTION_COLLAB: std::cout << "COLLAB"; break;
        case ACTION_DETECTION: std::cout << "DETECTION"; break;
        default: std::cout << "OTHER"; break;
    }
    std::cout << "] " << entry.action << " -> " << entry.target << " : " << entry.result;
    if (!entry.details.empty()) std::cout << " | " << entry.details;
    std::cout << std::endl;
    printPrompt();
    std::cout.flush();
}

void Shell::exportAgentLog(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cout << "❌ Failed to open file for export: " << filename << "\n";
        return;
    }
    out << "timestamp,agent,action_type,action,target,result,details\n";
    for (const auto& entry : advancedAgentLog) {
        out << '"' << entry.timestamp << '"' << ','
            << '"' << entry.agentName << '"' << ','
            << '"' << entry.actionType << '"' << ','
            << '"' << entry.action << '"' << ','
            << '"' << entry.target << '"' << ','
            << '"' << entry.result << '"' << ','
            << '"' << entry.details << '"' << '\n';
    }
    out.close();
    std::cout << "✅ Agent log exported to " << filename << "\n";
}

void Shell::displayAgentStatus() {
    if (!aiManager) {
        std::cout << "❌ AI manager not available.\n";
        return;
    }
    
    auto agents = aiManager->getAllAgents();
    std::cout << "\n🤖 Agent Status:\n";
    std::cout << "===============\n";
    
    for (const auto& agent : agents) {
        std::cout << "  " << agent->getName() << ":\n";
        std::cout << "    Type: " << agent->getType() << "\n";
        std::cout << "    State: " << agent->getState() << "\n";
        std::cout << "    Strategy: " << agent->getStrategy() << "\n";
        std::cout << "    Intelligence: " << (int)(agent->getIntelligence() * 100) << "%\n";
        std::cout << "    Success Rate: " << (int)(agent->getSuccessRate() * 100) << "%\n";
        std::cout << "\n";
    }
}

void Shell::clearAgentLog() {
    std::lock_guard<std::mutex> lock(logMutex);
    agentLog.clear();
}

void Shell::pauseAgents() {
    if (aiManager) {
        aiManager->pauseAllAgents();
        logAgentAction("All agents paused");
    }
}

void Shell::resumeAgents() {
    if (aiManager) {
        aiManager->resumeAllAgents();
        logAgentAction("All agents resumed");
    }
}

void Shell::showAgentMemory(const std::string& agentName) {
    if (!aiManager) {
        std::cout << "❌ AI manager not available.\n";
        return;
    }
    
    auto agent = aiManager->getAgent(agentName);
    if (!agent) {
        std::cout << "❌ Agent not found: " << agentName << "\n";
        return;
    }
    
    std::cout << "\n🧠 Agent Memory: " << agentName << "\n";
    std::cout << "================\n";
    std::cout << "  Total Attacks: " << agent->getSuccessRate() << "\n";
    std::cout << "  Success Rate: " << (int)(agent->getSuccessRate() * 100) << "%\n";
    std::cout << "  Intelligence: " << (int)(agent->getIntelligence() * 100) << "%\n";
    std::cout << "\n";
}

void Shell::simulationLoop() {
    while (simulationRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Slower for better visibility
        std::lock_guard<std::mutex> lock(simulationMutex);
        
        if (aiManager) {
            // Run agents and log their actions
            auto agents = aiManager->getAllAgents();
            for (const auto& agent : agents) {
                if (agent->getState() != IDLE) {
                    AgentDecision decision = agent->makeDecision();
                    if (decision.action != "idle") {
                        logAgentAction(agent->getName() + " executing: " + decision.action + 
                                     " on " + decision.target + " (confidence: " + 
                                     std::to_string((int)(decision.confidence * 100)) + "%)");
                        
                        bool success = agent->executeDecision(decision);
                        logAgentAction(agent->getName() + " result: " + 
                                     (success ? "SUCCESS" : "FAILED"));
                    }
                }
            }
        }
        
        if (advVisualizer) {
            advVisualizer->update();
            advVisualizer->render();
        }
    }
}

void Shell::run() {
    isRunning = true;
    printBanner();
    startSimulation();
    std::string input;
    while (isRunning) {
        printPrompt();
        std::getline(std::cin, input);
        if (input.empty()) continue;
        auto args = parseCommand(input);
        if (args.empty()) continue;
        std::string command = args[0];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        if (commands.find(command) != commands.end()) {
            try {
                commands[command](args);
            } catch (const std::exception& e) {
                std::cout << "❌ Error executing command: " << e.what() << std::endl;
            }
        } else {
            std::cout << "❌ Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
    stopSimulation();
}

void Shell::stop() {
    isRunning = false;
    stopSimulation();
}

void Shell::cmdHelp(const std::vector<std::string>& args) {
    std::cout << "\n📚 Available Commands:\n";
    std::cout << "=====================\n\n";
    
    std::cout << "🔍 Network Commands:\n";
    std::cout << "  nodes              - List all network nodes\n";
    std::cout << "  scan <target>      - Scan a target IP or network\n";
    std::cout << "  network            - Show network topology\n";
    std::cout << "  topology <type>    - Generate network topology\n";
    std::cout << "  visualize <type>   - Visualize network (topology/services/vulns)\n";
    std::cout << "  attackpath <ips>   - Show attack path visualization\n\n";
    
    std::cout << "💣 Exploitation Commands:\n";
    std::cout << "  exploit <type>     - Execute an exploit\n";
    std::cout << "  payload <type>     - Deploy a payload\n";
    std::cout << "  target <ip>        - Set current target\n\n";
    
    std::cout << "📊 Information Commands:\n";
    std::cout << "  info               - Show system information\n";
    std::cout << "  sessions           - List active sessions\n";
    std::cout << "  demo               - Run demonstration\n\n";
    
    std::cout << "🛠️  Utility Commands:\n";
    std::cout << "  help               - Show this help\n";
    std::cout << "  clear              - Clear screen\n";
    std::cout << "  exit/quit          - Exit RedNet\n\n";
    
    std::cout << "💡 Examples:\n";
    std::cout << "  scan 192.168.1.100\n";
    std::cout << "  exploit brute_force\n";
    std::cout << "  payload shell\n";
    std::cout << "  topology corporate\n\n";
}

void Shell::cmdExit(const std::vector<std::string>& args) {
    std::cout << "👋 Goodbye! Thanks for using RedNet.\n";
    stop();
}

void Shell::cmdClear(const std::vector<std::string>& args) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Shell::cmdNodes(const std::vector<std::string>& args) {
    std::cout << "\n🖥️  Network Nodes:\n";
    std::cout << "================\n\n";
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& node = nodes[i];
        std::cout << "[" << i + 1 << "] " << node->hostname << " (" << node->ip << ")\n";
        std::cout << "    Services: ";
        
        auto& services = node->services;
        for (size_t j = 0; j < services.size(); ++j) {
            std::cout << services[j].serviceName << ":" << services[j].port;
            if (j < services.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
        
        auto& vulnerabilities = node->vulnerabilities;
        if (!vulnerabilities.empty()) {
            std::cout << "    Vulnerabilities: ";
            for (size_t j = 0; j < vulnerabilities.size(); ++j) {
                std::cout << vulnerabilities[j].type;
                if (j < vulnerabilities.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void Shell::cmdScan(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: scan <target>\n";
        std::cout << "   Example: scan 192.168.1.100\n";
        return;
    }
    
    std::string target = args[1];
    std::cout << "\n🔍 Scanning " << target << "...\n";
    std::cout << "==================\n\n";
    
    // Find target node
    std::shared_ptr<Node> targetNode = nullptr;
    for (const auto& node : nodes) {
        if (node->ip == target) {
            targetNode = node;
            break;
        }
    }
    
    if (!targetNode) {
        std::cout << "❌ Target not found: " << target << std::endl;
        return;
    }
    
    // Perform scan
    auto& services = targetNode->services;
    std::cout << "📊 Scan Results:\n";
    
    for (const auto& service : services) {
        std::cout << "  ✅ Port " << service.port << " (" << service.serviceName << ") - ";
        
        // Simulate banner grabbing
        std::string banner;
        if (service.serviceName == "SSH") {
            banner = "SSH-2.0-OpenSSH_7.2p2 Ubuntu-4ubuntu2.2";
        } else if (service.serviceName == "HTTP") {
            banner = "HTTP/1.1 200 OK";
        } else if (service.serviceName == "FTP") {
            banner = "220 (vsFTPd 2.3.4)";
        } else {
            banner = service.serviceName;
        }
        
        std::cout << banner << std::endl;
    }
    
    std::cout << "\n🎯 Target set to: " << target << std::endl;
    currentTarget = target;
}

void Shell::cmdExploit(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: exploit <type>\n";
        std::cout << "   Available types: brute_force, service, buffer_overflow, privilege_escalation, sql_injection, xss, csrf, path_traversal, command_injection\n";
        return;
    }
    
    if (currentTarget.empty()) {
        std::cout << "❌ No target set. Use 'scan <target>' first.\n";
        return;
    }
    
    std::string exploitType = args[1];
    std::cout << "\n💣 Executing " << exploitType << " exploit on " << currentTarget << "...\n";
    std::cout << "==========================================\n\n";
    
    // Find target node
    std::shared_ptr<Node> targetNode = nullptr;
    for (const auto& node : nodes) {
        if (node->ip == currentTarget) {
            targetNode = node;
            break;
        }
    }
    
    if (!targetNode) {
        std::cout << "❌ Target not found: " << currentTarget << std::endl;
        return;
    }
    
    // Execute exploit
    if (exploitType == "brute_force") {
        std::cout << "🔨 Attempting brute force attack...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "  ✅ Success: Successfully compromised user: admin\n";
        std::cout << "  🔓 Access Level: User\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "service") {
        std::cout << "🔨 Attempting service exploit...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "  ✅ Success: Successfully exploited RDP service\n";
        std::cout << "  🔓 Access Level: User\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "buffer_overflow") {
        std::cout << "🔨 Attempting buffer overflow...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        std::cout << "  ✅ Success: Buffer overflow successful\n";
        std::cout << "  🔓 Access Level: Root\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "privilege_escalation") {
        std::cout << "🔨 Attempting privilege escalation...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        std::cout << "  ✅ Success: Privilege escalation successful\n";
        std::cout << "  🔓 Access Level: Root\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "sql_injection") {
        std::cout << "🔨 Attempting SQL injection...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        std::cout << "  ✅ Success: SQL injection successful\n";
        std::cout << "  🔓 Access Level: User\n";
        std::cout << "  📊 Database accessed: MySQL\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "xss") {
        std::cout << "🔨 Attempting Cross-Site Scripting...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        std::cout << "  ✅ Success: XSS attack successful\n";
        std::cout << "  🔓 Access Level: User\n";
        std::cout << "  🌐 Session hijacking possible\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "csrf") {
        std::cout << "🔨 Attempting CSRF attack...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        std::cout << "  ✅ Success: CSRF attack successful\n";
        std::cout << "  🔓 Access Level: User\n";
        std::cout << "  🔄 Unauthorized actions performed\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "path_traversal") {
        std::cout << "🔨 Attempting path traversal...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        std::cout << "  ✅ Success: Path traversal successful\n";
        std::cout << "  🔓 Access Level: User\n";
        std::cout << "  📁 Files accessed: /etc/passwd, /etc/shadow\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else if (exploitType == "command_injection") {
        std::cout << "🔨 Attempting command injection...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        std::cout << "  ✅ Success: Command injection successful\n";
        std::cout << "  🔓 Access Level: User\n";
        std::cout << "  💻 Remote command execution achieved\n";
        currentSession = "session_" + std::to_string(time(nullptr));
    } else {
        std::cout << "❌ Unknown exploit type: " << exploitType << std::endl;
        return;
    }
    
    std::cout << "\n🎯 Session established: " << currentSession << std::endl;
}

void Shell::cmdPayload(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: payload <type>\n";
        std::cout << "   Available types: shell, recon, exfil, persistence, lateral\n";
        return;
    }
    
    if (currentSession.empty()) {
        std::cout << "❌ No active session. Use 'exploit <type>' first.\n";
        return;
    }
    
    std::string payloadType = args[1];
    std::cout << "\n🧪 Deploying " << payloadType << " payload...\n";
    std::cout << "==============================\n\n";
    
    if (payloadType == "shell") {
        std::cout << "💻 Establishing shell access...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        std::cout << "  ✅ Shell access established\n";
        std::cout << "  👤 User: user\n";
        std::cout << "  📁 Current directory: /home/user\n";
        std::cout << "  💰 Available commands: ls, cat, whoami, id\n";
    } else if (payloadType == "recon") {
        std::cout << "🔍 Executing reconnaissance...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        std::cout << "  ✅ Reconnaissance successful\n";
        std::cout << "  📋 Data collected: 5 items\n";
        std::cout << "  📊 System info, user accounts, network config\n";
    } else if (payloadType == "exfil") {
        std::cout << "📤 Executing data exfiltration...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        std::cout << "  ✅ Exfiltrated 3 files\n";
        std::cout << "  📄 /etc/passwd, /etc/shadow, /home/user/documents/\n";
    } else if (payloadType == "persistence") {
        std::cout << "🔒 Installing persistence mechanism...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "  ✅ Persistence installed\n";
        std::cout << "  ⏰ Cron job created\n";
        std::cout << "  🔑 SSH key added\n";
    } else if (payloadType == "lateral") {
        std::cout << "🔄 Attempting lateral movement...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        std::cout << "  ✅ Lateral movement successful\n";
        std::cout << "  🎯 Compromised 2 additional hosts\n";
    } else {
        std::cout << "❌ Unknown payload type: " << payloadType << std::endl;
        return;
    }
    
    std::cout << "\n✅ Payload deployment completed\n";
}

void Shell::cmdNetwork(const std::vector<std::string>& args) {
    std::cout << "\n🌐 Network Topology:\n";
    std::cout << "==================\n\n";
    
    std::cout << "📊 Nodes: " << network->getNodeCount() << "\n";
    std::cout << "🔗 Links: " << network->getLinkCount() << "\n\n";
    // Detailed node/link listing can be added if public getters are available
}

void Shell::cmdTopology(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: topology <type>\n";
        std::cout << "   Available types: star, mesh, corporate, datacenter\n";
        return;
    }
    
    std::string topologyType = args[1];
    std::cout << "\n🌐 Generating " << topologyType << " topology...\n";
    std::cout << "==============================\n\n";
    
    if (topologyType == "star") {
        auto topology = topologyGen->generateStarTopology(5);
        std::cout << "✅ Created star topology with " << topology.getNodeCount()
                  << " nodes and " << topology.getLinkCount() << " links\n";
    } else if (topologyType == "mesh") {
        auto topology = topologyGen->generateMeshTopology(4);
        std::cout << "✅ Created mesh topology with " << topology.getNodeCount()
                  << " nodes and " << topology.getLinkCount() << " links\n";
    } else if (topologyType == "corporate") {
        auto topology = topologyGen->generateCorporateNetwork();
        std::cout << "✅ Created corporate network with " << topology.getNodeCount()
                  << " nodes and " << topology.getLinkCount() << " links\n";
    } else if (topologyType == "datacenter") {
        auto topology = topologyGen->generateDataCenter();
        std::cout << "✅ Created data center with " << topology.getNodeCount()
                  << " nodes and " << topology.getLinkCount() << " links\n";
    } else {
        std::cout << "❌ Unknown topology type: " << topologyType << std::endl;
        return;
    }
    
    std::cout << "\n📊 Topology generated successfully\n";
}

void Shell::cmdTarget(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: target <ip>\n";
        return;
    }
    
    std::string target = args[1];
    currentTarget = target;
    std::cout << "🎯 Target set to: " << target << std::endl;
}

void Shell::cmdSessions(const std::vector<std::string>& args) {
    std::cout << "\n🔗 Active Sessions:\n";
    std::cout << "==================\n\n";
    
    if (currentSession.empty()) {
        std::cout << "❌ No active sessions\n";
    } else {
        std::cout << "✅ " << currentSession << " -> " << currentTarget << " (Active)\n";
    }
    
    std::cout << "\n";
}

void Shell::cmdInfo(const std::vector<std::string>& args) {
    std::cout << "\n📊 System Information:\n";
    std::cout << "=====================\n\n";
    
    std::cout << "🔧 RedNet Version: 1.0.0\n";
    std::cout << "🎯 Current Target: " << (currentTarget.empty() ? "None" : currentTarget) << "\n";
    std::cout << "🔗 Active Session: " << (currentSession.empty() ? "None" : currentSession) << "\n";
    std::cout << "🖥️  Total Nodes: " << nodes.size() << "\n";
    std::cout << "💣 Exploits Available: " << exploitDB->getExploitCount() << "\n";
    std::cout << "🧪 Payloads Available: " << payloadDB->getPayloadCount() << "\n";
    
    std::cout << "\n";
}

void Shell::cmdDemo(const std::vector<std::string>& args) {
    std::cout << "\n🎬 Running RedNet Demonstration...\n";
    std::cout << "================================\n\n";
    
    // Demo sequence
    cmdNodes(args);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    cmdScan({"scan", "192.168.1.100"});
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    cmdExploit({"exploit", "brute_force"});
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    cmdPayload({"payload", "shell"});
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    cmdNetwork(args);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    std::cout << "🎉 Demonstration completed!\n\n";
}

void Shell::cmdVisualize(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: visualize <type>\n";
        std::cout << "   Available types: topology, services, vulns, all\n";
        return;
    }
    
    std::string vizType = args[1];
    std::cout << "\n🎨 Generating " << vizType << " visualization...\n";
    std::cout << "=====================================\n\n";
    
    // Load current nodes into visualizer
    visualizer->loadFromNodes(nodes);
    
    if (vizType == "topology" || vizType == "all") {
        std::cout << visualizer->generateTopologyView() << std::endl;
    }
    
    if (vizType == "services" || vizType == "all") {
        std::cout << visualizer->generateServiceView() << std::endl;
    }
    
    if (vizType == "vulns" || vizType == "all") {
        std::cout << visualizer->generateVulnerabilityView() << std::endl;
    }
    
    if (vizType != "topology" && vizType != "services" && vizType != "vulns" && vizType != "all") {
        std::cout << "❌ Unknown visualization type: " << vizType << std::endl;
        std::cout << "   Available types: topology, services, vulns, all\n";
    }
}

void Shell::cmdAttackPath(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: attackpath <ip1> <ip2> [ip3] ...\n";
        std::cout << "   Example: attackpath 192.168.1.100 192.168.1.101 192.168.1.102\n";
        return;
    }
    
    std::vector<std::string> attackPath;
    for (size_t i = 1; i < args.size(); ++i) {
        attackPath.push_back(args[i]);
    }
    
    std::cout << "\n🎯 Attack Path Visualization\n";
    std::cout << "============================\n\n";
    
    // Load current nodes into visualizer
    visualizer->loadFromNodes(nodes);
    
    // Mark compromised nodes (for demo purposes, mark the last one)
    if (!attackPath.empty()) {
        visualizer->markCompromised(attackPath.back());
    }
    
    std::cout << visualizer->generateAttackPathView(attackPath) << std::endl;
}

void Shell::cmdConfig(const std::vector<std::string>& args) {
    if (!configManager) {
        std::cout << "❌ Config manager not available.\n";
        return;
    }
    if (args.size() == 1) {
        // Show current config summary
        auto aiCfg = configManager->getAIConfig();
        auto netProfiles = configManager->getNetworkProfileNames();
        std::cout << "\n🛠️  Current Configuration:\n";
        std::cout << "  AI max agents: " << aiCfg.maxAgents << ", intelligence: " << aiCfg.intelligenceLevel << "\n";
        std::cout << "  Network profiles: ";
        for (const auto& n : netProfiles) std::cout << n << " ";
        std::cout << "\n";
        std::cout << "  Log level: " << configManager->getGeneralConfig().logLevel << "\n";
        std::cout << "  Data dir: " << configManager->getGeneralConfig().dataDirectory << "\n";
        std::cout << "\n";
        std::cout << "  Usage: config save <file> | load <file> | show\n";
        return;
    }
    if (args[1] == "save" && args.size() > 2) {
        if (configManager->saveConfig(args[2])) {
            std::cout << "✅ Config saved to " << args[2] << "\n";
        } else {
            std::cout << "❌ Failed to save config.\n";
        }
    } else if (args[1] == "load" && args.size() > 2) {
        if (configManager->loadConfig(args[2])) {
            std::cout << "✅ Config loaded from " << args[2] << "\n";
        } else {
            std::cout << "❌ Failed to load config.\n";
        }
    } else if (args[1] == "show") {
        cmdConfig({"config"});
    } else {
        std::cout << "Usage: config save <file> | load <file> | show\n";
    }
}

void Shell::cmdScenario(const std::vector<std::string>& args) {
    if (!configManager) {
        std::cout << "❌ Config manager not available.\n";
        return;
    }
    if (args.size() == 1) {
        // List scenarios
        auto names = configManager->getScenarioTemplateNames();
        std::cout << "\n🎯 Available Scenarios:\n";
        for (const auto& n : names) std::cout << "  - " << n << "\n";
        std::cout << "  Usage: scenario load <name>\n";
        return;
    }
    if (args[1] == "load" && args.size() > 2) {
        configManager->loadScenario(args[2]);
        std::cout << "✅ Scenario '" << args[2] << "' loaded.\n";
    } else {
        std::cout << "Usage: scenario load <name>\n";
    }
}

void Shell::cmdAgent(const std::vector<std::string>& args) {
    if (!aiManager) {
        std::cout << "❌ AI agent manager not available.\n";
        return;
    }
    
    if (args.size() == 1) {
        // List agents
        auto agents = aiManager->getAllAgents();
        std::cout << "\n🤖 AI Agents:\n";
        for (const auto& agent : agents) {
            std::cout << "  - " << agent->getName() << " (" << agent->getType() << ", " << agent->getStrategy() << ")\n";
        }
        std::cout << "  Usage: agent start <type> <name> | stop <name> | status | memory <name> | pause | resume | log\n";
        return;
    }
    
    if (args[1] == "start" && args.size() > 3) {
        std::string type = args[2];
        std::string name = args[3];
        std::shared_ptr<AIAgent> agent;
        if (type == "recon") agent = std::make_shared<ReconAgent>(name);
        else if (type == "exploit") agent = std::make_shared<ExploitAgent>(name);
        else {
            std::cout << "❌ Unknown agent type.\n";
            return;
        }
        aiManager->addAgent(agent);
        logAgentAction("Started agent '" + name + "' of type '" + type + "'");
    } else if (args[1] == "stop" && args.size() > 2) {
        aiManager->removeAgent(args[2]);
        logAgentAction("Stopped agent '" + args[2] + "'");
    } else if (args[1] == "status") {
        displayAgentStatus();
    } else if (args[1] == "memory" && args.size() > 2) {
        showAgentMemory(args[2]);
    } else if (args[1] == "pause") {
        pauseAgents();
    } else if (args[1] == "resume") {
        resumeAgents();
    } else if (args[1] == "log") {
        std::lock_guard<std::mutex> lock(logMutex);
        std::cout << "\n📋 Agent Action Log:\n";
        std::cout << "==================\n";
        for (const auto& log : agentLog) {
            std::cout << log << "\n";
        }
        std::cout << "\n";
    } else if (args[1] == "list") {
        cmdAgent({"agent"});
    } else {
        std::cout << "Usage: agent start <type> <name> | stop <name> | status | memory <name> | pause | resume | log\n";
    }
}

void Shell::cmdVisMode(const std::vector<std::string>& args) {
    if (!advVisualizer) {
        std::cout << "❌ Advanced visualizer not available.\n";
        return;
    }
    if (args.size() == 1) {
        std::cout << "\n🎨 Visualization Modes: ascii2d, color2d, interactive2d, 3d, realtime\n";
        std::cout << "  Usage: vismode <mode>\n";
        return;
    }
    std::string mode = args[1];
    if (mode == "ascii2d") advVisualizer->switchVisualizer(ASCII_2D);
    else if (mode == "color2d") advVisualizer->switchVisualizer(COLOR_2D);
    else if (mode == "interactive2d") advVisualizer->switchVisualizer(INTERACTIVE_2D);
    else if (mode == "3d") advVisualizer->switchVisualizer(SIMPLE_3D);
    else if (mode == "realtime") advVisualizer->switchVisualizer(GRAPHICAL_2D);
    else {
        std::cout << "❌ Unknown visualization mode.\n";
        return;
    }
    std::cout << "✅ Visualization mode set to '" << mode << "'.\n";
}

void Shell::cmdHeatmap(const std::vector<std::string>& args) {
    if (!advVisualizer) {
        std::cout << "❌ Advanced visualizer not available.\n";
        return;
    }
    std::cout << "\n🔥 Vulnerability Heatmap:\n";
    // For now, just call render (would show heatmap if implemented)
    advVisualizer->render();
    std::cout << "(Heatmap rendering simulated.)\n";
}

void Shell::logAgentAction(const std::string& action) {
    std::cout << "[AGENT] " << action << std::endl;
} 