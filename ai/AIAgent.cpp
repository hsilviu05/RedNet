#include "AIAgent.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

// Base AI Agent implementation
AIAgent::AIAgent(std::string agentName, AgentType agentType, AgentStrategy agentStrategy)
    : name(agentName), type(agentType), strategy(agentStrategy), state(IDLE),
      intelligence(0.7), adaptability(0.6), persistence(0.5), stealth(0.4),
      gen(rd()) {
}

AgentDecision AIAgent::makeDecision() {
    AgentDecision decision;
    decision.action = "idle";
    decision.target = "";
    decision.reason = "No suitable targets available";
    decision.confidence = 0.0;
    decision.priority = 0;
    
    // Generate possible options
    auto options = generateOptions();
    if (!options.empty()) {
        decision = selectBestOption(options);
    }
    
    return decision;
}

bool AIAgent::executeDecision(const AgentDecision& decision) {
    if (decision.action == "idle") {
        return true;
    }
    
    // Simulate decision execution
    std::cout << "🤖 " << name << " executing: " << decision.action;
    if (!decision.target.empty()) {
        std::cout << " on " << decision.target;
    }
    std::cout << " (confidence: " << (int)(decision.confidence * 100) << "%)" << std::endl;
    
    // Simulate execution time
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Determine success based on confidence and intelligence
    bool success = (gen() % 100) < (decision.confidence * intelligence * 100);
    
    updateMemory(decision, success);
    return success;
}

void AIAgent::updateMemory(const AgentDecision& decision, bool success) {
    memory.totalAttacks++;
    if (success) {
        memory.successfulAttacks++;
        learnFromSuccess(decision, nullptr);
    } else {
        learnFromFailure(decision, nullptr);
    }
}

void AIAgent::analyzeTarget(std::shared_ptr<Node> target) {
    if (!target) return;
    
    std::cout << "🔍 " << name << " analyzing target: " << target->ip << std::endl;
    
    // Analyze services
    for (const auto& service : target->services) {
        if (service.isRunning) {
            memory.serviceVulnerabilityRates[service.serviceName] = 0.5; // Default
        }
    }
    
    // Analyze vulnerabilities
    for (const auto& vuln : target->vulnerabilities) {
        memory.serviceVulnerabilityRates[vuln.type] = 0.7; // Vulnerable services
    }
}

void AIAgent::initialize() {
    state = IDLE;
    std::cout << "🤖 " << name << " initialized" << std::endl;
}

void AIAgent::run() {
            state = DOING_RECONNAISSANCE;
    std::cout << "🤖 " << name << " started running" << std::endl;
}

void AIAgent::pause() {
    std::cout << "🤖 " << name << " paused" << std::endl;
}

void AIAgent::resume() {
    std::cout << "🤖 " << name << " resumed" << std::endl;
}

void AIAgent::shutdown() {
    state = IDLE;
    std::cout << "🤖 " << name << " shutdown" << std::endl;
}

std::vector<AgentDecision> AIAgent::generateOptions() {
    std::vector<AgentDecision> options;
    
    if (!network) return options;
    
    // Generate reconnaissance options
    AgentDecision recon;
    recon.action = "reconnaissance";
    recon.reason = "Discover network topology";
    recon.confidence = 0.8;
    recon.priority = 1;
    options.push_back(recon);
    
    // Generate exploit options if targets are available
    if (!discoveredNodes.empty()) {
        for (const auto& node : discoveredNodes) {
            if (node && !node->isCompromised()) {
                AgentDecision exploit;
                exploit.action = "exploit";
                exploit.target = node->ip;
                exploit.reason = "Attempt to compromise " + node->ip;
                exploit.confidence = 0.6;
                exploit.priority = 2;
                options.push_back(exploit);
            }
        }
    }
    
    return options;
}

AgentDecision AIAgent::selectBestOption(const std::vector<AgentDecision>& options) {
    if (options.empty()) {
        return AgentDecision();
    }
    
    // Simple selection based on priority and confidence
    AgentDecision best = options[0];
    double bestScore = best.priority * best.confidence;
    
    for (const auto& option : options) {
        double score = option.priority * option.confidence;
        if (score > bestScore) {
            best = option;
            bestScore = score;
        }
    }
    
    return best;
}

double AIAgent::evaluateTarget(std::shared_ptr<Node> target) {
    if (!target) return 0.0;
    
    double score = 0.0;
    
    // Evaluate based on services
    for (const auto& service : target->services) {
        if (service.isRunning) {
            auto it = memory.serviceVulnerabilityRates.find(service.serviceName);
            if (it != memory.serviceVulnerabilityRates.end()) {
                score += it->second;
            }
        }
    }
    
    // Evaluate based on vulnerabilities
    score += target->vulnerabilities.size() * 0.2;
    
    // Normalize score
    return std::min(score, 1.0);
}

double AIAgent::predictExploitSuccess(std::string exploitName, std::shared_ptr<Node> target) {
    auto it = memory.exploitSuccessRates.find(exploitName);
    if (it != memory.exploitSuccessRates.end()) {
        return it->second;
    }
    
    // Default prediction based on intelligence
    return intelligence * 0.7;
}

void AIAgent::learnFromSuccess(const AgentDecision& decision, std::shared_ptr<Node> target) {
    if (decision.action == "exploit") {
        memory.exploitSuccessRates[decision.target] = 0.8; // Increase success rate
        memory.successfulTargets.push_back(decision.target);
    }
}

void AIAgent::learnFromFailure(const AgentDecision& decision, std::shared_ptr<Node> target) {
    if (decision.action == "exploit") {
        memory.exploitSuccessRates[decision.target] = 0.3; // Decrease success rate
        memory.failedTargets.push_back(decision.target);
    }
}

void AIAgent::adaptStrategy() {
    double successRate = getSuccessRate();
    
    if (successRate < 0.3) {
        // Switch to more aggressive strategy
        if (strategy == STEALTHY) strategy = METHODICAL;
        else if (strategy == METHODICAL) strategy = AGGRESSIVE;
    } else if (successRate > 0.7) {
        // Switch to more stealthy strategy
        if (strategy == AGGRESSIVE) strategy = METHODICAL;
        else if (strategy == METHODICAL) strategy = STEALTHY;
    }
}

double AIAgent::getSuccessRate() const {
    if (memory.totalAttacks == 0) return 0.0;
    return (double)memory.successfulAttacks / memory.totalAttacks;
}

void AIAgent::shareInfoWith(AIAgent& other) {
    for (const auto& cred : knownCredentials) other.receiveInfo(cred);
    for (const auto& host : knownHosts) other.receiveInfo(host);
    for (const auto& vuln : knownVulnerabilities) other.receiveInfo(vuln);
}

void AIAgent::receiveInfo(const std::string& info) {
    // Simple: add to knownHosts or knownCredentials or knownVulnerabilities
    if (info.find("vuln:") == 0) knownVulnerabilities.push_back(info.substr(5));
    else if (info.find("cred:") == 0) knownCredentials.push_back(info.substr(5));
    else knownHosts.push_back(info);
}

void AIAgent::recordFailure() {
    failureCount++;
    if (failureCount > 2) adaptStrategy();
}

void AIAgent::recordDetection() {
    detectionCount++;
    if (stealthLevel < 1.0) stealthLevel += 0.2;
}

// ReconAgent implementation
ReconAgent::ReconAgent(std::string name, AgentStrategy strategy)
    : AIAgent(name, RECON_AGENT, strategy) {
    
    scanTechniques = {"ping_sweep", "port_scan", "service_enum", "os_detection"};
    techniqueEffectiveness["ping_sweep"] = 0.9;
    techniqueEffectiveness["port_scan"] = 0.8;
    techniqueEffectiveness["service_enum"] = 0.7;
    techniqueEffectiveness["os_detection"] = 0.6;
}

AgentDecision ReconAgent::makeDecision() {
    AgentDecision decision;
    
    if (discoveredNodes.empty()) {
        decision.action = "network_discovery";
        decision.reason = "Perform initial network discovery";
        decision.confidence = 0.9;
        decision.priority = 1;
    } else {
        // Choose a node for detailed analysis
        std::shared_ptr<Node> target = discoveredNodes[gen() % discoveredNodes.size()];
        decision.action = "service_enumeration";
        decision.target = target->ip;
        decision.reason = "Enumerate services on " + target->ip;
        decision.confidence = 0.8;
        decision.priority = 2;
    }
    
    return decision;
}

bool ReconAgent::executeDecision(const AgentDecision& decision) {
    if (decision.action == "network_discovery") {
        performNetworkDiscovery();
        return true;
    } else if (decision.action == "service_enumeration") {
        // Find target node
        std::shared_ptr<Node> target = nullptr;
        for (const auto& node : discoveredNodes) {
            if (node->ip == decision.target) {
                target = node;
                break;
            }
        }
        
        if (target) {
            performServiceEnumeration(target);
            return true;
        }
    }
    
    return false;
}

void ReconAgent::performNetworkDiscovery() {
    if (!network) return;
    
    // Get all nodes from network
    auto networkNodes = network->getAllNodes();
    
    for (const auto& networkNode : networkNodes) {
        discoveredNodes.push_back(networkNode.nodePtr);
    }
    
    std::cout << "🔍 " << name << " discovered " << discoveredNodes.size() << " nodes" << std::endl;
}

void ReconAgent::performServiceEnumeration(std::shared_ptr<Node> target) {
    std::cout << "🔍 " << name << " enumerating services on " << target->ip << std::endl;
    
    for (const auto& service : target->services) {
        if (service.isRunning) {
            std::cout << "  ✅ Service: " << service.serviceName << ":" << service.port << std::endl;
        }
    }
}

void ReconAgent::performVulnerabilityAssessment(std::shared_ptr<Node> target) {
    std::cout << "🔍 " << name << " assessing vulnerabilities on " << target->ip << std::endl;
    
    for (const auto& vuln : target->vulnerabilities) {
        std::cout << "  ⚠️  Vulnerability: " << vuln.type << std::endl;
    }
}

void ReconAgent::performOSFingerprinting(std::shared_ptr<Node> target) {
    std::cout << "🔍 " << name << " fingerprinting OS on " << target->ip << std::endl;
    std::cout << "  💻 OS: Linux (Ubuntu 20.04)" << std::endl;
}

// ExploitAgent implementation
ExploitAgent::ExploitAgent(std::string name, AgentStrategy strategy)
    : AIAgent(name, EXPLOIT_AGENT, strategy) {
    
    preferredExploits = {"brute_force", "service_exploit", "sql_injection", "xss"};
    exploitPreferences["brute_force"] = 0.8;
    exploitPreferences["service_exploit"] = 0.7;
    exploitPreferences["sql_injection"] = 0.6;
    exploitPreferences["xss"] = 0.5;
}

AgentDecision ExploitAgent::makeDecision() {
    AgentDecision decision;
    
    // Find vulnerable targets
    std::vector<std::shared_ptr<Node>> vulnerableTargets;
    for (const auto& node : discoveredNodes) {
        if (node && !node->isCompromised() && !node->vulnerabilities.empty()) {
            vulnerableTargets.push_back(node);
        }
    }
    
    if (vulnerableTargets.empty()) {
        decision.action = "wait";
        decision.reason = "No vulnerable targets available";
        decision.confidence = 1.0;
        decision.priority = 0;
    } else {
        // Choose target based on vulnerability score
        std::shared_ptr<Node> bestTarget = vulnerableTargets[0];
        double bestScore = evaluateTarget(bestTarget);
        
        for (const auto& target : vulnerableTargets) {
            double score = evaluateTarget(target);
            if (score > bestScore) {
                bestTarget = target;
                bestScore = score;
            }
        }
        
        decision.action = "exploit";
        decision.target = bestTarget->ip;
        decision.reason = "Exploit vulnerabilities on " + bestTarget->ip;
        decision.confidence = bestScore;
        decision.priority = 3;
    }
    
    return decision;
}

bool ExploitAgent::executeDecision(const AgentDecision& decision) {
    if (decision.action == "exploit") {
        // Find target node
        std::shared_ptr<Node> target = nullptr;
        for (const auto& node : discoveredNodes) {
            if (node->ip == decision.target) {
                target = node;
                break;
            }
        }
        
        if (target && exploitDB) {
            auto exploits = selectExploits(target);
            if (!exploits.empty()) {
                return executeExploit(exploits[0], target);
            }
        }
    }
    
    return false;
}

std::vector<Exploit*> ExploitAgent::selectExploits(std::shared_ptr<Node> target) {
    std::vector<Exploit*> selectedExploits;
    
    if (!exploitDB) return selectedExploits;
    
    auto applicableExploits = exploitDB->getExploitsForNode(*target);
    
    // Sort by preference
    std::sort(applicableExploits.begin(), applicableExploits.end(),
              [this](Exploit* a, Exploit* b) {
                  auto itA = exploitPreferences.find(a->getName());
                  auto itB = exploitPreferences.find(b->getName());
                  double prefA = (itA != exploitPreferences.end()) ? itA->second : 0.5;
                  double prefB = (itB != exploitPreferences.end()) ? itB->second : 0.5;
                  return prefA > prefB;
              });
    
    return applicableExploits;
}

bool ExploitAgent::executeExploit(Exploit* exploit, std::shared_ptr<Node> target) {
    if (!exploit || !target) return false;
    
    std::cout << "💣 " << name << " executing " << exploit->getName() << " on " << target->ip << std::endl;
    
    ExploitResult result = exploit->execute(*target);
    
    if (result.success) {
        compromisedNodes.push_back(target);
        std::cout << "  ✅ Exploit successful!" << std::endl;
        return true;
    } else {
        std::cout << "  ❌ Exploit failed: " << result.errorMessage << std::endl;
        return false;
    }
}

void ExploitAgent::chainExploits(std::vector<Exploit*> exploits, std::shared_ptr<Node> target) {
    std::cout << "⛓️  " << name << " chaining " << exploits.size() << " exploits on " << target->ip << std::endl;
    
    for (Exploit* exploit : exploits) {
        if (!executeExploit(exploit, target)) {
            break; // Stop chain if one fails
        }
    }
} 

// PersistenceAgent implementation
PersistenceAgent::PersistenceAgent(std::string name, AgentStrategy strategy)
    : AIAgent(name, PERSISTENCE_AGENT, strategy) {
    persistenceMethods = {"backdoor", "startup_script", "user_account"};
    methodReliability["backdoor"] = 0.8;
    methodReliability["startup_script"] = 0.7;
    methodReliability["user_account"] = 0.6;
}

AgentDecision PersistenceAgent::makeDecision() {
    AgentDecision decision;
    decision.action = "persist";
    decision.reason = "Maintain access";
    decision.confidence = 0.8;
    decision.priority = 1;
    return decision;
}

bool PersistenceAgent::executeDecision(const AgentDecision& decision) {
    // Stub: just print action
    std::cout << "🔒 " << name << " executing persistence action: " << decision.action << std::endl;
    return true;
} 