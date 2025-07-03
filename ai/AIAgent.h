#ifndef AI_AGENT_H
#define AI_AGENT_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <random>
#include <thread>
#include "../engine/Node.h"
#include "../engine/Exploit.h"
#include "../engine/Scanner.h"
#include "../network/NetworkGraph.h"

// AI Agent types
enum AgentType {
    RECON_AGENT,
    EXPLOIT_AGENT,
    PERSISTENCE_AGENT,
    LATERAL_AGENT,
    COORDINATOR_AGENT
};

// AI Agent personality/strategy
enum AgentStrategy {
    STEALTHY,      // Low and slow, avoid detection
    AGGRESSIVE,    // Fast and loud, maximize impact
    METHODICAL,    // Systematic approach, thorough
    OPPORTUNISTIC, // Quick wins, adapt to opportunities
    PERSISTENT     // Keep trying, don't give up
};

// AI Agent state
enum AgentState {
    IDLE,
    DOING_RECONNAISSANCE,
    EXPLOITING,
    PERSISTING,
    MOVING_LATERAL,
    COMPROMISED,
    DETECTED,
    PAUSED,
    ACTIVE
};

// AI Agent decision structure
struct AgentDecision {
    std::string action;
    std::string target;
    std::string reason;
    double confidence;
    int priority;
    std::vector<std::string> prerequisites;
};

// AI Agent memory/learning
struct AgentMemory {
    std::map<std::string, double> exploitSuccessRates;
    std::map<std::string, double> serviceVulnerabilityRates;
    std::vector<std::string> successfulTargets;
    std::vector<std::string> failedTargets;
    std::map<std::string, std::vector<std::string>> attackPaths;
    int totalAttacks;
    int successfulAttacks;
};

// Base AI Agent class
class AIAgent {
protected:
    std::string name;
    AgentType type;
    AgentStrategy strategy;
    AgentState state;
    AgentMemory memory;
    
    std::shared_ptr<NetworkGraph> network;
    std::shared_ptr<ExploitDatabase> exploitDB;
    std::shared_ptr<Scanner> scanner;
    
    std::vector<std::shared_ptr<Node>> discoveredNodes;
    std::vector<std::shared_ptr<Node>> compromisedNodes;
    std::vector<std::string> currentTargets;
    
    std::random_device rd;
    std::mt19937 gen;
    
    // Agent capabilities
    double intelligence;      // 0.0 to 1.0
    double adaptability;      // 0.0 to 1.0
    double persistence;       // 0.0 to 1.0
    double stealth;          // 0.0 to 1.0
    
    // Advanced agent state
    double stealthLevel = 0.5; // 0 = noisy, 1 = stealthy
    std::vector<std::string> knownCredentials;
    std::vector<std::string> knownHosts;
    std::vector<std::string> knownVulnerabilities;
    std::vector<std::string> goals; // e.g., "initial_access", "escalate", "exfiltrate"
    std::string currentGoal;
    int failureCount = 0;
    int detectionCount = 0;
    bool persistent = false;
    
public:
    AIAgent(std::string agentName, AgentType agentType, AgentStrategy agentStrategy);
    virtual ~AIAgent() = default;
    
    // Core AI methods
    virtual AgentDecision makeDecision();
    virtual bool executeDecision(const AgentDecision& decision);
    virtual void updateMemory(const AgentDecision& decision, bool success);
    virtual void analyzeTarget(std::shared_ptr<Node> target);
    
    // Agent lifecycle
    virtual void initialize();
    virtual void run();
    virtual void pause();
    virtual void resume();
    virtual void shutdown();
    
    // Intelligence methods
    virtual std::vector<AgentDecision> generateOptions();
    virtual AgentDecision selectBestOption(const std::vector<AgentDecision>& options);
    virtual double evaluateTarget(std::shared_ptr<Node> target);
    virtual double predictExploitSuccess(std::string exploitName, std::shared_ptr<Node> target);
    
    // Learning methods
    virtual void learnFromSuccess(const AgentDecision& decision, std::shared_ptr<Node> target);
    virtual void learnFromFailure(const AgentDecision& decision, std::shared_ptr<Node> target);
    virtual void adaptStrategy();
    
    // Getters
    std::string getName() const { return name; }
    AgentType getType() const { return type; }
    AgentStrategy getStrategy() const { return strategy; }
    AgentState getState() const { return state; }
    double getIntelligence() const { return intelligence; }
    double getSuccessRate() const;
    
    // Setters
    void setNetwork(std::shared_ptr<NetworkGraph> net) { network = net; }
    void setExploitDB(std::shared_ptr<ExploitDatabase> db) { exploitDB = db; }
    void setScanner(std::shared_ptr<Scanner> scan) { scanner = scan; }
    void setStrategy(AgentStrategy newStrategy) { strategy = newStrategy; }
    void setIntelligence(double intel) { intelligence = intel; }
    
    // Agent control
    bool isPaused() const { return state == PAUSED; }
    
    // Collaboration
    void shareInfoWith(AIAgent& other);
    void receiveInfo(const std::string& info);
    
    // Stealth
    void setStealthLevel(double s) { stealthLevel = s; }
    double getStealthLevel() const { return stealthLevel; }
    
    // Goal management
    void setGoal(const std::string& goal) { currentGoal = goal; }
    std::string getGoal() const { return currentGoal; }
    void addGoal(const std::string& goal) { goals.push_back(goal); }
    
    // Learning from failure
    void recordFailure();
    
    // Detection
    void recordDetection();
    int getDetectionCount() const { return detectionCount; }
    
    // Persistence
    void enablePersistence() { persistent = true; }
    bool isPersistent() const { return persistent; }
};

// Reconnaissance Agent - Discovers and analyzes targets
class ReconAgent : public AIAgent {
private:
    std::vector<std::string> scanTechniques;
    std::map<std::string, double> techniqueEffectiveness;
    
public:
    ReconAgent(std::string name, AgentStrategy strategy = METHODICAL);
    
    AgentDecision makeDecision() override;
    bool executeDecision(const AgentDecision& decision) override;
    
    void performNetworkDiscovery();
    void performServiceEnumeration(std::shared_ptr<Node> target);
    void performVulnerabilityAssessment(std::shared_ptr<Node> target);
    void performOSFingerprinting(std::shared_ptr<Node> target);
    
    std::vector<std::shared_ptr<Node>> getDiscoveredNodes() const { return discoveredNodes; }
};

// Exploit Agent - Executes attacks and gains access
class ExploitAgent : public AIAgent {
private:
    std::map<std::string, double> exploitPreferences;
    std::vector<std::string> preferredExploits;
    
public:
    ExploitAgent(std::string name, AgentStrategy strategy = AGGRESSIVE);
    
    AgentDecision makeDecision() override;
    bool executeDecision(const AgentDecision& decision) override;
    
    std::vector<Exploit*> selectExploits(std::shared_ptr<Node> target);
    bool executeExploit(Exploit* exploit, std::shared_ptr<Node> target);
    void chainExploits(std::vector<Exploit*> exploits, std::shared_ptr<Node> target);
    
    std::vector<std::shared_ptr<Node>> getCompromisedNodes() const { return compromisedNodes; }
};

// Persistence Agent - Maintains access and evades detection
class PersistenceAgent : public AIAgent {
private:
    std::vector<std::string> persistenceMethods;
    std::map<std::string, double> methodReliability;
    
public:
    PersistenceAgent(std::string name, AgentStrategy strategy = STEALTHY);
    
    AgentDecision makeDecision() override;
    bool executeDecision(const AgentDecision& decision) override;
    
    void installBackdoor(std::shared_ptr<Node> target);
    void createUserAccount(std::shared_ptr<Node> target);
    void modifyStartupScripts(std::shared_ptr<Node> target);
    void evadeDetection(std::shared_ptr<Node> target);
};

// Lateral Movement Agent - Spreads through the network
class LateralAgent : public AIAgent {
private:
    std::vector<std::string> movementTechniques;
    std::map<std::string, double> techniqueSuccessRates;
    
public:
    LateralAgent(std::string name, AgentStrategy strategy = OPPORTUNISTIC);
    
    AgentDecision makeDecision() override;
    bool executeDecision(const AgentDecision& decision) override;
    
    std::vector<std::shared_ptr<Node>> findNextTargets();
    bool moveToTarget(std::shared_ptr<Node> from, std::shared_ptr<Node> to);
    void useCredentials(std::shared_ptr<Node> target, std::string username, std::string password);
    void useExploits(std::shared_ptr<Node> target);
};

// Coordinator Agent - Orchestrates multiple agents
class CoordinatorAgent : public AIAgent {
private:
    std::vector<std::shared_ptr<AIAgent>> subAgents;
    std::map<std::string, std::vector<AgentDecision>> agentTasks;
    
public:
    CoordinatorAgent(std::string name);
    
    AgentDecision makeDecision() override;
    bool executeDecision(const AgentDecision& decision) override;
    
    void addAgent(std::shared_ptr<AIAgent> agent);
    void removeAgent(std::string agentName);
    void assignTask(std::string agentName, const AgentDecision& task);
    void coordinateAttack();
    
    std::vector<std::shared_ptr<AIAgent>> getSubAgents() const { return subAgents; }
};

#endif // AI_AGENT_H 