#include "AIAgent.h"
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <thread>

AIAgent::AIAgent() : id(0), type(AgentType::SCANNER), status(AgentStatus::IDLE), 
                     successRate(0.0f), isActive(false) {
}

AIAgent::AIAgent(int agentId, const std::string& agentName, AgentType agentType)
    : id(agentId), name(agentName), type(agentType), status(AgentStatus::IDLE),
      successRate(0.0f), isActive(false) {
}

AIAgent::~AIAgent() {
}

int AIAgent::getId() const {
    return id;
}

const std::string& AIAgent::getName() const {
    return name;
}

AgentType AIAgent::getType() const {
    return type;
}

AgentStatus AIAgent::getStatus() const {
    return status;
}

bool AIAgent::isAgentActive() const {
    return isActive;
}

float AIAgent::getSuccessRate() const {
    return successRate;
}

void AIAgent::activate() {}
void AIAgent::deactivate() {}
void AIAgent::executeTask(int) {}
void AIAgent::updateStatus(AgentStatus) {}
void AIAgent::setSuccessRate(float) {}
void AIAgent::addTask(const AgentTask&) {}
std::string AIAgent::getName() const { return "StubAgent"; }
std::vector<AgentTask> AIAgent::getTasks() const { return {}; }

std::string AIAgent::getTypeName() const {
    switch (type) {
        case AgentType::SCANNER: return "Scanner";
        case AgentType::EXPLOITER: return "Exploiter";
        case AgentType::PAYLOAD_DELIVERY: return "Payload Delivery";
        case AgentType::PERSISTENCE: return "Persistence";
        case AgentType::EXFILTRATION: return "Exfiltration";
        case AgentType::COVERAGE: return "Coverage";
        default: return "Unknown";
    }
}

std::string AIAgent::getStatusName() const {
    switch (status) {
        case AgentStatus::IDLE: return "Idle";
        case AgentStatus::ACTIVE: return "Active";
        case AgentStatus::COMPLETED: return "Completed";
        case AgentStatus::FAILED: return "Failed";
        default: return "Unknown";
    }
}

const std::vector<AgentTask>& AIAgent::getTasks() const {
    return tasks;
}

void AIAgent::setSuccessRate(float rate) {
    if (rate >= 0.0f && rate <= 1.0f) {
        successRate = rate;
    }
}

std::string AIAgent::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"type\":\"" << getTypeName() << "\",";
    json << "\"status\":\"" << getStatusName() << "\",";
    json << "\"successRate\":" << successRate << ",";
    json << "\"isActive\":" << (isActive ? "true" : "false") << ",";
    json << "\"tasks\":[";
    for (size_t i = 0; i < tasks.size(); ++i) {
        json << "{";
        json << "\"description\":\"" << tasks[i].description << "\",";
        json << "\"target\":\"" << tasks[i].target << "\",";
        json << "\"completed\":" << (tasks[i].completed ? "true" : "false") << ",";
        json << "\"result\":\"" << tasks[i].result << "\"";
        json << "}";
        if (i < tasks.size() - 1) json << ",";
    }
    json << "]";
    json << "}";
    return json.str();
} 