#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <string>
#include <vector>
#include "Node.h"

enum class PayloadType {
    EXPLOIT,
    BACKDOOR,
    KEYLOGGER,
    RANSOMWARE,
    SPYWARE,
    ROOTKIT
};

struct ExecutionRecord {
    std::string target;
    bool success;
    std::string details;
};

struct PayloadResult {
    bool success;
    std::string output;
};

class ShellPayload {
public:
    PayloadResult executeCommand(const Node& target, const std::string& command) { return {true, "stub output"}; }
};

class Payload {
private:
    int id;
    std::string name;
    std::string description;
    PayloadType type;
    float successRate;
    bool isActive;
    std::vector<ExecutionRecord> executionHistory;

public:
    Payload();
    Payload(int payloadId, const std::string& name, const std::string& description, 
            PayloadType type, float successRate);
    ~Payload();

    int getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    PayloadType getType() const;
    float getSuccessRate() const;
    bool isPayloadActive() const;

    void activate();
    void deactivate();
    bool execute(const std::string& target);
    
    const std::vector<ExecutionRecord>& getExecutionHistory() const;
    std::string getTypeName() const;
    void setSuccessRate(float rate);
    void addExecutionRecord(const std::string& target, bool success, const std::string& details);
    std::string toJson() const;
};

class PayloadManager {
private:
    std::vector<Payload> payloads;
    void loadDefaultPayloads();

public:
    PayloadManager();
    ~PayloadManager();

    void addPayload(const Payload& payload);
    void removePayload(int id);
    Payload* getPayload(int id);
    const std::vector<Payload>& getAllPayloads() const;
    std::vector<Payload> getPayloadsByType(PayloadType type);
    
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
};

class PayloadDatabase {
public:
    void* findPayloadByName(const std::string&) { return nullptr; }
};

#endif // PAYLOAD_H 