#include <iostream>
#include <vector>
#include <memory>
#include "engine/Node.h"
#include "engine/Scanner.h"
#include "engine/Exploit.h"
#include "engine/Payload.h"

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

int main() {
    printBanner();
    
    try {
        demonstrateNodeCreation();
        demonstrateScanning();
        demonstrateExploitation();
        demonstratePayloads();
        demonstrateExploitChaining();
        
        std::cout << "🎉 RedNet demonstration completed successfully!\n";
        std::cout << "📚 This is a simulation for educational purposes only.\n";
        std::cout << "🔗 Visit https://github.com/hsilviu05/RedNet for more information.\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
