#include "Payload.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

Payload::Payload() : id(0), successRate(0.0f), isActive(false) {
}

Payload::Payload(int payloadId, const std::string& name, const std::string& description, 
                 PayloadType type, float successRate)
    : id(payloadId), name(name), description(description), type(type), 
      successRate(successRate), isActive(false) {
}

Payload::~Payload() {
}

int Payload::getId() const {
    return id;
}

const std::string& Payload::getName() const {
    return name;
}

const std::string& Payload::getDescription() const {
    return description;
}

PayloadType Payload::getType() const {
    return type;
}

float Payload::getSuccessRate() const {
    return successRate;
}

bool Payload::isPayloadActive() const {
    return isActive;
}

void Payload::activate() {
    isActive = true;
    std::cout << "Payload '" << name << "' activated." << std::endl;
}

void Payload::deactivate() {
    isActive = false;
    std::cout << "Payload '" << name << "' deactivated." << std::endl;
}

bool Payload::execute(const std::string& target) {
    if (!isActive) {
        std::cout << "Cannot execute inactive payload." << std::endl;
        return false;
    }
    
    std::cout << "Executing payload '" << name << "' on target: " << target << std::endl;
    
    // Simulate execution with success rate
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    bool success = dist(gen) <= successRate;
    
    if (success) {
        std::cout << "Payload execution successful!" << std::endl;
        executionHistory.push_back({target, true, "Success"});
    } else {
        std::cout << "Payload execution failed." << std::endl;
        executionHistory.push_back({target, false, "Failed"});
    }
    
    return success;
}

const std::vector<ExecutionRecord>& Payload::getExecutionHistory() const {
    return executionHistory;
}

std::string Payload::getTypeName() const {
    switch (type) {
        case PayloadType::EXPLOIT: return "Exploit";
        case PayloadType::BACKDOOR: return "Backdoor";
        case PayloadType::KEYLOGGER: return "Keylogger";
        case PayloadType::RANSOMWARE: return "Ransomware";
        case PayloadType::SPYWARE: return "Spyware";
        case PayloadType::ROOTKIT: return "Rootkit";
        default: return "Unknown";
    }
}

void Payload::setSuccessRate(float rate) {
    if (rate >= 0.0f && rate <= 1.0f) {
        successRate = rate;
    }
}

void Payload::addExecutionRecord(const std::string& target, bool success, const std::string& details) {
    executionHistory.push_back({target, success, details});
}

std::string Payload::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"name\":\"" << name << "\",";
    json << "\"description\":\"" << description << "\",";
    json << "\"type\":\"" << getTypeName() << "\",";
    json << "\"successRate\":" << successRate << ",";
    json << "\"isActive\":" << (isActive ? "true" : "false");
    json << "}";
    return json.str();
}

PayloadManager::PayloadManager() {
    loadDefaultPayloads();
}

PayloadManager::~PayloadManager() {
}

void PayloadManager::addPayload(const Payload& payload) {
    payloads.push_back(payload);
    std::cout << "Added payload: " << payload.getName() << std::endl;
}

void PayloadManager::removePayload(int id) {
    payloads.erase(
        std::remove_if(payloads.begin(), payloads.end(),
                      [id](const Payload& p) { return p.getId() == id; }),
        payloads.end()
    );
}

Payload* PayloadManager::getPayload(int id) {
    for (auto& payload : payloads) {
        if (payload.getId() == id) {
            return &payload;
        }
    }
    return nullptr;
}

const std::vector<Payload>& PayloadManager::getAllPayloads() const {
    return payloads;
}

std::vector<Payload> PayloadManager::getPayloadsByType(PayloadType type) {
    std::vector<Payload> filtered;
    for (const auto& payload : payloads) {
        if (payload.getType() == type) {
            filtered.push_back(payload);
        }
    }
    return filtered;
}

void PayloadManager::loadDefaultPayloads() {
    // Add some default payloads
    addPayload(Payload(1, "SSH Brute Force", "Attempts to crack SSH passwords", PayloadType::EXPLOIT, 0.3f));
    addPayload(Payload(2, "SQL Injection", "SQL injection attack vector", PayloadType::EXPLOIT, 0.4f));
    addPayload(Payload(3, "Reverse Shell", "Creates a reverse shell connection", PayloadType::BACKDOOR, 0.6f));
    addPayload(Payload(4, "Keylogger Module", "Captures keystrokes", PayloadType::KEYLOGGER, 0.7f));
    addPayload(Payload(5, "Data Exfiltration", "Steals sensitive data", PayloadType::SPYWARE, 0.5f));
    addPayload(Payload(6, "Privilege Escalation", "Elevates user privileges", PayloadType::ROOTKIT, 0.4f));
}

void PayloadManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "[\n";
        for (size_t i = 0; i < payloads.size(); ++i) {
            file << payloads[i].toJson();
            if (i < payloads.size() - 1) file << ",";
            file << "\n";
        }
        file << "]\n";
        file.close();
        std::cout << "Payloads saved to " << filename << std::endl;
    }
}

void PayloadManager::loadFromFile(const std::string& filename) {
    // Implementation for loading payloads from file
    std::cout << "Loading payloads from " << filename << std::endl;
} 