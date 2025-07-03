#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <random>
#include <chrono>
#include <sstream>
#include <algorithm>
#include "Node.h"

enum PayloadType {
    SHELL_PAYLOAD,
    DATA_EXFILTRATION,
    PERSISTENCE,
    RECONNAISSANCE,
    LATERAL_MOVEMENT,
    CUSTOM_PAYLOAD,
    UNKNOWN_PAYLOAD
};

enum ExecutionLevel {
    USER_LEVEL,
    ADMIN_LEVEL,
    ROOT_LEVEL,
    SYSTEM_LEVEL
};

struct PayloadResult {
    bool success;
    std::string output;
    std::string errorMessage;
    std::vector<std::string> filesAccessed;
    std::vector<std::string> commandsExecuted;
    std::vector<std::string> dataCollected;
    int timeTaken; // milliseconds
    ExecutionLevel executionLevel;
    
    PayloadResult() : success(false), timeTaken(0), executionLevel(USER_LEVEL) {}
};

// Forward declaration
class Node;

// Base Payload class
class Payload {
protected:
    std::string name;
    std::string description;
    PayloadType type;
    int complexity; // 1-10 scale
    double detectionProbability;
    ExecutionLevel requiredLevel;
    std::vector<std::string> requiredTools;
    std::vector<std::string> dependencies;
    
public:
    Payload(std::string name, std::string description, PayloadType type, int complexity, double detectionProb, ExecutionLevel requiredLevel);
    virtual ~Payload() = default;
    
    // Virtual methods that must be implemented by derived classes
    virtual bool canExecute(Node& target) = 0;
    virtual PayloadResult execute(Node& target) = 0;
    
    // Getters
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    PayloadType getType() const { return type; }
    int getComplexity() const { return complexity; }
    double getDetectionProbability() const { return detectionProbability; }
    ExecutionLevel getRequiredLevel() const { return requiredLevel; }
    
    // Utility methods
    void addRequiredTool(std::string tool);
    void addDependency(std::string dependency);
    bool requiresLevel(ExecutionLevel level) const;
};

// Shell Payload for command execution
class ShellPayload : public Payload {
private:
    std::vector<std::string> allowedCommands;
    std::vector<std::string> blockedCommands;
    int maxCommandLength;
    bool interactiveMode;
    
public:
    ShellPayload();
    ~ShellPayload() = default;
    
    bool canExecute(Node& target) override;
    PayloadResult execute(Node& target) override;
    
    PayloadResult executeCommand(Node& target, std::string command);
    void addAllowedCommand(std::string command);
    void addBlockedCommand(std::string command);
    void setInteractiveMode(bool interactive);
    void setMaxCommandLength(int length);
    
private:
    std::string processCommand(std::string command, Node& target);
    bool isCommandAllowed(std::string command);
    std::string simulateCommandOutput(std::string command, Node& target);
};

// Data Exfiltration Payload
class DataExfiltrationPayload : public Payload {
private:
    std::vector<std::string> targetFiles;
    std::vector<std::string> targetDirectories;
    std::vector<std::string> fileExtensions;
    int maxFileSize; // bytes
    bool compressData;
    
public:
    DataExfiltrationPayload();
    ~DataExfiltrationPayload() = default;
    
    bool canExecute(Node& target) override;
    PayloadResult execute(Node& target) override;
    
    void addTargetFile(std::string filePath);
    void addTargetDirectory(std::string directory);
    void addFileExtension(std::string extension);
    void setMaxFileSize(int size);
    void setCompressData(bool compress);
    
private:
    std::string readFileContent(Node& target, std::string filePath);
    std::vector<std::string> findFiles(Node& target, std::string directory, std::string extension);
    std::string compressData(std::string data);
};

// Persistence Payload for backdoor installation
class PersistencePayload : public Payload {
private:
    std::vector<std::string> persistenceMethods;
    std::string backdoorCode;
    bool stealthMode;
    std::vector<std::string> cleanupCommands;
    
public:
    PersistencePayload();
    ~PersistencePayload() = default;
    
    bool canExecute(Node& target) override;
    PayloadResult execute(Node& target) override;
    
    void addPersistenceMethod(std::string method);
    void setBackdoorCode(std::string code);
    void setStealthMode(bool stealth);
    void addCleanupCommand(std::string command);
    
private:
    bool installCronJob(Node& target);
    bool modifyStartupScripts(Node& target);
    bool installSSHKey(Node& target);
    bool createHiddenService(Node& target);
    void cleanupEvidence(Node& target);
};

// Reconnaissance Payload for information gathering
class ReconnaissancePayload : public Payload {
private:
    std::vector<std::string> reconModules;
    bool deepScan;
    std::vector<std::string> targetSystems;
    
public:
    ReconnaissancePayload();
    ~ReconnaissancePayload() = default;
    
    bool canExecute(Node& target) override;
    PayloadResult execute(Node& target) override;
    
    void addReconModule(std::string module);
    void setDeepScan(bool deep);
    void addTargetSystem(std::string system);
    
private:
    std::string gatherSystemInfo(Node& target);
    std::string gatherNetworkInfo(Node& target);
    std::string gatherUserInfo(Node& target);
    std::string gatherServiceInfo(Node& target);
    std::string gatherFileSystemInfo(Node& target);
};

// Lateral Movement Payload
class LateralMovementPayload : public Payload {
private:
    std::vector<std::string> movementMethods;
    std::vector<std::string> discoveredTargets;
    bool autoPropagate;
    int maxHops;
    
public:
    LateralMovementPayload();
    ~LateralMovementPayload() = default;
    
    bool canExecute(Node& target) override;
    PayloadResult execute(Node& target) override;
    
    void addMovementMethod(std::string method);
    void setAutoPropagate(bool autoProp);
    void setMaxHops(int hops);
    std::vector<std::string> getDiscoveredTargets() const;
    
private:
    std::vector<std::string> discoverNetworkNeighbors(Node& target);
    bool propagateViaSSH(Node& target, std::string neighbor);
    bool propagateViaSMB(Node& target, std::string neighbor);
    bool propagateViaRDP(Node& target, std::string neighbor);
};

// Payload Chain for chaining multiple payloads
class PayloadChain {
private:
    std::vector<Payload*> payloads;
    std::string name;
    double chainSuccessProbability;
    
public:
    PayloadChain(std::string name);
    ~PayloadChain();
    
    void addPayload(Payload* payload);
    PayloadResult executeChain(Node& target);
    double getChainSuccessProbability() const;
    std::string getName() const { return name; }
    int getPayloadCount() const { return payloads.size(); }
};

// Payload Database to manage all payloads
class PayloadDatabase {
private:
    std::vector<Payload*> payloads;
    std::map<PayloadType, std::vector<Payload*>> payloadsByType;
    std::map<ExecutionLevel, std::vector<Payload*>> payloadsByLevel;
    
public:
    PayloadDatabase();
    ~PayloadDatabase();
    
    void addPayload(Payload* payload);
    std::vector<Payload*> getPayloadsForNode(Node& target);
    std::vector<Payload*> getPayloadsByType(PayloadType type);
    std::vector<Payload*> getPayloadsByLevel(ExecutionLevel level);
    std::vector<Payload*> getAllPayloads() const;
    void loadDefaultPayloads();
    void clearPayloads();
    
    // Utility methods
    Payload* findPayloadByName(std::string name);
    int getPayloadCount() const { return payloads.size(); }
};
