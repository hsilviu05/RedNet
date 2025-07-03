#include "AIAgentManager.h"
#include <iostream>
#include <algorithm>

AIAgentManager::AIAgentManager() : maxAgents(10), enableCollaboration(true), enableLearning(true) {
}

AIAgentManager::~AIAgentManager() {
    shutdown();
}

void AIAgentManager::addAgent(std::shared_ptr<AIAgent> agent) {
    if (agents.size() < maxAgents) {
        agents.push_back(agent);
        agentMap[agent->getName()] = agent;
        agent->setNetwork(network);
        agent->setExploitDB(exploitDB);
        agent->setScanner(scanner);
        std::cout << "🤖 Added AI agent: " << agent->getName() << std::endl;
    }
}

void AIAgentManager::removeAgent(std::string agentName) {
    agents.erase(
        std::remove_if(agents.begin(), agents.end(),
                      [&agentName](const std::shared_ptr<AIAgent>& agent) {
                          return agent->getName() == agentName;
                      }),
        agents.end()
    );
    agentMap.erase(agentName);
}

std::shared_ptr<AIAgent> AIAgentManager::getAgent(const std::string& name) {
    auto it = agentMap.find(name);
    if (it != agentMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<AIAgent>> AIAgentManager::getAllAgents() const {
    return agents;
}

void AIAgentManager::startAllAgents() {
    for (auto& agent : agents) {
        agent->initialize();
        agent->run();
    }
    running = true;
    std::cout << "🤖 Started " << agents.size() << " AI agents" << std::endl;
}

void AIAgentManager::stopAllAgents() {
    for (auto& agent : agents) {
        agent->shutdown();
    }
    running = false;
    std::cout << "🤖 Stopped all AI agents" << std::endl;
}

void AIAgentManager::pauseAllAgents() {
    for (auto& agent : agents) {
        agent->pause();
    }
}

void AIAgentManager::resumeAllAgents() {
    for (auto& agent : agents) {
        agent->resume();
    }
}

void AIAgentManager::setNetwork(std::shared_ptr<NetworkGraph> net) {
    network = net;
    for (auto& agent : agents) {
        agent->setNetwork(net);
    }
}

void AIAgentManager::setExploitDB(std::shared_ptr<ExploitDatabase> db) {
    exploitDB = db;
    for (auto& agent : agents) {
        agent->setExploitDB(db);
    }
}

void AIAgentManager::setScanner(std::shared_ptr<Scanner> scan) {
    scanner = scan;
    for (auto& agent : agents) {
        agent->setScanner(scan);
    }
}

void AIAgentManager::run() {
    if (running) {
        for (auto& agent : agents) {
            AgentDecision decision = agent->makeDecision();
            agent->executeDecision(decision);
        }
    }
}

void AIAgentManager::shutdown() {
    stopAllAgents();
    if (managerThread.joinable()) {
        managerThread.join();
    }
}

int AIAgentManager::getAgentCount() const {
    return agents.size();
}

int AIAgentManager::getActiveAgentCount() const {
    int count = 0;
    for (const auto& agent : agents) {
        if (agent->getState() != IDLE) {
            count++;
        }
    }
    return count;
}

double AIAgentManager::getOverallSuccessRate() const {
    if (agents.empty()) return 0.0;
    
    double totalRate = 0.0;
    for (const auto& agent : agents) {
        totalRate += agent->getSuccessRate();
    }
    return totalRate / agents.size();
}

void AIAgentManager::managerLoop() {
    while (running) {
        coordinateAgents();
        shareIntelligence();
        monitorPerformance();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void AIAgentManager::coordinateAgents() {
    if (!enableCollaboration) return;
    
    // Simple coordination logic
    for (auto& agent : agents) {
        // Share discovered nodes between agents
        for (auto& otherAgent : agents) {
            if (agent != otherAgent) {
                agent->shareInfoWith(*otherAgent);
            }
        }
    }
}

void AIAgentManager::shareIntelligence() {
    if (!enableCollaboration) return;
    
    // Share intelligence between agents
    for (auto& agent : agents) {
        for (auto& otherAgent : agents) {
            if (agent != otherAgent) {
                // Share information about successful exploits, discovered vulnerabilities, etc.
            }
        }
    }
}

void AIAgentManager::monitorPerformance() {
    // Monitor agent performance and adjust strategies if needed
    for (auto& agent : agents) {
        double successRate = agent->getSuccessRate();
        if (successRate < 0.3) {
            // Agent is struggling, might need to adjust strategy
            agent->adaptStrategy();
        }
    }
} 