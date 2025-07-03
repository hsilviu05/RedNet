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

void printBanner() {
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

void demonstrateNodeCreation() {
    std::cout << "🔧 Creating Virtual Network Nodes...\n";
    
    // Create different types of nodes
    std::vector<std::unique_ptr<Node>> nodes;
    
    // Web server
    auto webServer = std::make_unique<Node>("192.168.1.10", LINUX);
    webServer->createWebServer();
    nodes.push_back(std::move(webServer));
    
    // File server
    auto fileServer = std::make_unique<Node>("192.168.1.20", LINUX);
    fileServer->createFileServer();
    nodes.push_back(std::move(fileServer));
    
    // Workstation
    auto workstation = std::make_unique<Node>("192.168.1.30", WINDOWS);
    workstation->createWorkstation();
    nodes.push_back(std::move(workstation));
    
    // Domain controller
    auto domainController = std::make_unique<Node>("192.168.1.40", WINDOWS);
    domainController->createDomainController();
    nodes.push_back(std::move(domainController));
    
    std::cout << "✅ Created " << nodes.size() << " virtual nodes\n\n";
}

void demonstrateScanning() {
    std::cout << "🔍 Demonstrating Network Scanning...\n";
    
    // Create a target node
    Node target("192.168.1.100", LINUX);
    target.createWebServer();
    
    // Create scanner and scan the target
    Scanner scanner;
    scanner.setVerbose(true);
    scanner.scanHost("192.168.1.100");
    
    // Get and display results
    auto results = scanner.getResults();
    std::cout << "📊 Scan Results:\n";
    for (const auto& result : results) {
        if (result.isOpen) {
            std::cout << "  ✅ Port " << result.port << " (" << result.service << ") - " << result.banner << "\n";
        }
    }
    std::cout << "\n";
}

void demonstrateExploitation() {
    std::cout << "💣 Demonstrating Exploit Framework...\n";
    
    // Create a target node
    Node target("192.168.1.200", LINUX);
    target.createWorkstation();
    
    // Create exploit database
    ExploitDatabase exploits;
    
    // Get applicable exploits for the target
    auto applicableExploits = exploits.getExploitsForNode(target);
    std::cout << "🎯 Found " << applicableExploits.size() << " applicable exploits\n";
    
    // Execute some exploits
    for (auto exploit : applicableExploits) {
        std::cout << "🔨 Executing: " << exploit->getName() << "\n";
        ExploitResult result = exploit->execute(target);
        
        if (result.success) {
            std::cout << "  ✅ Success: " << result.message << "\n";
            std::cout << "  🔓 Access Level: " << (result.accessLevel == USER_ACCESS ? "User" : 
                                                   result.accessLevel == ADMIN_ACCESS ? "Admin" : 
                                                   result.accessLevel == ROOT_ACCESS ? "Root" : "None") << "\n";
        } else {
            std::cout << "  ❌ Failed: " << result.errorMessage << "\n";
        }
    }
    std::cout << "\n";
}

void demonstratePayloads() {
    std::cout << "🧪 Demonstrating Payload System...\n";
    
    // Create a compromised target
    Node target("192.168.1.300", LINUX);
    target.createFileServer();
    target.markCompromised();
    
    // Create payload database
    PayloadDatabase payloads;
    
    // Execute reconnaissance payload
    auto reconPayload = payloads.findPayloadByName("Reconnaissance");
    if (reconPayload) {
        std::cout << "🔍 Executing reconnaissance...\n";
        PayloadResult result = reconPayload->execute(target);
        if (result.success) {
            std::cout << "  ✅ Reconnaissance successful\n";
            std::cout << "  📋 Data collected: " << result.dataCollected.size() << " items\n";
        }
    }
    
    // Execute shell payload
    auto shellPayload = payloads.findPayloadByName("Shell Access");
    if (shellPayload) {
        std::cout << "💻 Establishing shell access...\n";
        PayloadResult result = shellPayload->execute(target);
        if (result.success) {
            std::cout << "  ✅ Shell access established\n";
            
            // Execute some commands
            ShellPayload* shell = dynamic_cast<ShellPayload*>(shellPayload);
            if (shell) {
                auto cmdResult = shell->executeCommand(target, "whoami");
                std::cout << "  👤 User: " << cmdResult.output;
                
                cmdResult = shell->executeCommand(target, "ls");
                std::cout << "  📁 Files: " << cmdResult.output;
            }
        }
    }
    
    // Execute data exfiltration
    auto exfilPayload = payloads.findPayloadByName("Data Exfiltration");
    if (exfilPayload) {
        std::cout << "📤 Executing data exfiltration...\n";
        PayloadResult result = exfilPayload->execute(target);
        if (result.success) {
            std::cout << "  ✅ Exfiltrated " << result.filesAccessed.size() << " files\n";
        }
    }
    
    std::cout << "\n";
}

void demonstrateExploitChaining() {
    std::cout << "⛓️ Demonstrating Exploit Chaining...\n";
    
    // Create a target
    Node target("192.168.1.400", LINUX);
    target.createWorkstation();
    
    // Create exploit chain
    ExploitChain chain("Initial Access Chain");
    
    // Add exploits to chain
    ExploitDatabase exploits;
    auto bruteForce = exploits.findExploitByName("Brute Force Attack");
    auto privilegeEscalation = exploits.findExploitByName("Privilege Escalation");
    
    if (bruteForce) chain.addExploit(bruteForce);
    if (privilegeEscalation) chain.addExploit(privilegeEscalation);
    
    // Execute the chain
    std::cout << "🔗 Executing exploit chain with " << chain.getExploitCount() << " exploits\n";
    ExploitResult result = chain.executeChain(target);
    
    if (result.success) {
        std::cout << "  ✅ Chain completed successfully\n";
        std::cout << "  🎯 Final access level: " << (result.accessLevel == USER_ACCESS ? "User" : 
                                                     result.accessLevel == ADMIN_ACCESS ? "Admin" : 
                                                     result.accessLevel == ROOT_ACCESS ? "Root" : "None") << "\n";
    } else {
        std::cout << "  ❌ Chain failed: " << result.errorMessage << "\n";
    }
    
    std::cout << "\n";
}

void demonstrateNetworkTopology() {
    std::cout << "🌐 Demonstrating Network Topology Generation...\n";
    
    // Create topology generator
    TopologyGenerator generator;
    
    // Generate different topologies
    std::cout << "📊 Generating Star Topology...\n";
    NetworkGraph starGraph = generator.generateStarTopology(5);
    std::cout << "  ✅ Created star topology with " << starGraph.getNodeCount() << " nodes and " 
              << starGraph.getLinkCount() << " links\n";
    
    std::cout << "📊 Generating Mesh Topology...\n";
    NetworkGraph meshGraph = generator.generateMeshTopology(4);
    std::cout << "  ✅ Created mesh topology with " << meshGraph.getNodeCount() << " nodes and " 
              << meshGraph.getLinkCount() << " links\n";
    
    std::cout << "📊 Generating Corporate Network...\n";
    NetworkGraph corporateGraph = generator.generateCorporateNetwork();
    std::cout << "  ✅ Created corporate network with " << corporateGraph.getNodeCount() << " nodes and " 
              << corporateGraph.getLinkCount() << " links\n";
    
    std::cout << "📊 Generating Data Center...\n";
    NetworkGraph datacenterGraph = generator.generateDataCenter();
    std::cout << "  ✅ Created data center with " << datacenterGraph.getNodeCount() << " nodes and " 
              << datacenterGraph.getLinkCount() << " links\n";
    
    std::cout << "\n";
}

void demonstrateNetworkSimulation() {
    std::cout << "🔄 Demonstrating Network Simulation...\n";
    
    // Create a simple network
    NetworkGraph network;
    
    // Add nodes
    auto node1 = std::make_shared<Node>("192.168.1.10", LINUX);
    node1->createWorkstation();
    NetworkNode netNode1("192.168.1.10", node1);
    netNode1.hostname = "workstation-1";
    network.addNode(netNode1);
    
    auto node2 = std::make_shared<Node>("192.168.1.20", LINUX);
    node2->createWebServer();
    NetworkNode netNode2("192.168.1.20", node2);
    netNode2.hostname = "webserver";
    network.addNode(netNode2);
    
    auto node3 = std::make_shared<Node>("192.168.1.30", LINUX);
    node3->createFileServer();
    NetworkNode netNode3("192.168.1.30", node3);
    netNode3.hostname = "fileserver";
    network.addNode(netNode3);
    
    // Add links
    NetworkLink link1("192.168.1.10", "192.168.1.20");
    NetworkLink link2("192.168.1.20", "192.168.1.30");
    network.addLink(link1);
    network.addLink(link2);
    
    // Add firewall rules
    FirewallRule rule;
    rule.sourceIP = "192.168.1.10";
    rule.destIP = "192.168.1.20";
    rule.destPort = 80;
    rule.protocol = HTTP;
    rule.allow = true;
    rule.description = "Allow HTTP from workstation to web server";
    network.addFirewallRule(rule);
    
    // Simulate packet traffic
    std::cout << "📦 Simulating packet traffic...\n";
    
    // Send HTTP packet
    Packet httpPacket("192.168.1.10", "192.168.1.20", HTTP);
    httpPacket.sourcePort = 12345;
    httpPacket.destPort = 80;
    httpPacket.payload = "GET / HTTP/1.1";
    
    bool sent = network.sendPacket(httpPacket);
    if (sent) {
        std::cout << "  ✅ HTTP packet sent successfully\n";
    } else {
        std::cout << "  ❌ HTTP packet blocked by firewall\n";
    }
    
    // Send blocked packet
    Packet blockedPacket("192.168.1.10", "192.168.1.20", SSH);
    blockedPacket.sourcePort = 12346;
    blockedPacket.destPort = 22;
    blockedPacket.payload = "SSH connection attempt";
    
    sent = network.sendPacket(blockedPacket);
    if (sent) {
        std::cout << "  ✅ SSH packet sent successfully\n";
    } else {
        std::cout << "  ❌ SSH packet blocked by firewall\n";
    }
    
    // Process packets
    network.simulateNetworkTraffic();
    
    // Show packet history
    auto packetHistory = network.getPacketHistory();
    std::cout << "📋 Packet History (" << packetHistory.size() << " packets):\n";
    for (const auto& packet : packetHistory) {
        std::string status = (packet.status == DELIVERED) ? "✅ DELIVERED" :
                            (packet.status == BLOCKED) ? "🚫 BLOCKED" :
                            (packet.status == DROPPED) ? "❌ DROPPED" : "⏳ PENDING";
        
        std::cout << "  " << status << " " << packet.sourceIP << ":" << packet.sourcePort 
                  << " -> " << packet.destIP << ":" << packet.destPort << " (" 
                  << (packet.protocol == HTTP ? "HTTP" : "SSH") << ")\n";
    }
    
    // Network discovery
    std::cout << "🔍 Performing network discovery...\n";
    auto reachableNodes = network.discoverNetwork("192.168.1.10");
    std::cout << "  ✅ Discovered " << reachableNodes.size() << " reachable nodes\n";
    
    // Find path between nodes
    auto path = network.findPath("192.168.1.10", "192.168.1.30");
    std::cout << "  🛣️ Path from workstation to file server: ";
    for (size_t i = 0; i < path.size(); i++) {
        if (i > 0) std::cout << " -> ";
        std::cout << path[i];
    }
    std::cout << "\n";
    
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "🚀 RedNet AI Cyber Range Simulator\n";
    std::cout << "=====================================\n\n";
    
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
    
    if (useGUI) {
        std::cout << "Starting GUI Dashboard...\n";
        auto dashboard = std::make_shared<ImGuiDashboard>(aiManager, network, config, visualizer, shell);
        dashboard->run();
    } else if (useShell) {
        std::cout << "Starting Command-Line Shell...\n";
        shell->run();
    }
    
    return 0;
}
