#include "AIAgentManager.h"
#include <iostream>
#include <algorithm>
#include <chrono>

AIAgentManager::AIAgentManager() : maxAgents(10), enableCollaboration(true), enableLearning(true), running(false) {
}

AIAgentManager::~AIAgentManager() {
    shutdown();
}

void AIAgentManager::addAgent(const std::string& name, const std::string& type) {
    SimpleAgent agent;
    agent.name = name;
    agent.type = type;
    agent.state = "Active";
    simpleAgents.push_back(agent);
}

void AIAgentManager::removeAgent(const std::string& name) {
    simpleAgents.erase(std::remove_if(simpleAgents.begin(), simpleAgents.end(), [&](const SimpleAgent& a) {
        return a.name == name;
    }), simpleAgents.end());
}

std::vector<SimpleAgent> AIAgentManager::getSimpleAgents() const {
    return simpleAgents;
}

void AIAgentManager::startAllAgents() {
    for (auto& agent : agents) {
        agent->activate();
    }
    running = true;
    std::cout << "🤖 Started " << agents.size() << " AI agents" << std::endl;
}

void AIAgentManager::stopAllAgents() {
    for (auto& agent : agents) {
        agent->deactivate();
    }
    running = false;
    std::cout << "🤖 Stopped all AI agents" << std::endl;
}

void AIAgentManager::pauseAllAgents() {
    for (auto& agent : agents) {
        agent->updateStatus(AgentStatus::IDLE);
    }
}

void AIAgentManager::resumeAllAgents() {
    for (auto& agent : agents) {
        agent->updateStatus(AgentStatus::ACTIVE);
    }
}

void AIAgentManager::run() {
    if (running) {
        for (auto& agent : agents) {
            if (agent->isAgentActive()) {
                // Execute tasks for active agents
                const auto& tasks = agent->getTasks();
                for (size_t i = 0; i < tasks.size(); ++i) {
                    if (!tasks[i].completed) {
                        agent->executeTask(i);
                        break;
                    }
                }
            }
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
    return static_cast<int>(agents.size());
}

int AIAgentManager::getActiveAgentCount() const {
    int count = 0;
    for (const auto& agent : agents) {
        if (agent->isAgentActive()) {
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
    
    // Simple coordination logic - share information between agents
    for (auto& agent : agents) {
        for (auto& otherAgent : agents) {
            if (agent != otherAgent && agent->isAgentActive() && otherAgent->isAgentActive()) {
                // Share task information between agents
                const auto& tasks = agent->getTasks();
                for (const auto& task : tasks) {
                    if (task.completed) {
                        // Share successful task results
                        AgentTask sharedTask = task;
                        otherAgent->addTask(sharedTask);
                    }
                }
            }
        }
    }
}

void AIAgentManager::shareIntelligence() {
    if (!enableCollaboration) return;
    
    // Share intelligence between agents
    for (auto& agent : agents) {
        for (auto& otherAgent : agents) {
            if (agent != otherAgent && agent->isAgentActive() && otherAgent->isAgentActive()) {
                // Share success rates and strategies
                double successRate = agent->getSuccessRate();
                if (successRate > 0.7) {
                    // High-performing agent shares its success rate
                    otherAgent->setSuccessRate(successRate * 0.9); // Slightly lower for balance
                }
            }
        }
    }
}

void AIAgentManager::monitorPerformance() {
    // Monitor agent performance and adjust strategies if needed
    for (auto& agent : agents) {
        double successRate = agent->getSuccessRate();
        if (successRate < 0.3) {
            // Agent is struggling, boost its success rate
            agent->setSuccessRate(successRate + 0.1);
        }
    }
} 