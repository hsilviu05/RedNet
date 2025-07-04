#ifndef AI_AGENT_MANAGER_H
#define AI_AGENT_MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include "AIAgent.h"

struct SimpleAgent {
    std::string name;
    std::string type;
    std::string state;
};

class AIAgentManager {
private:
    std::vector<std::shared_ptr<AIAgent>> agents;
    std::vector<SimpleAgent> simpleAgents;
    std::map<std::string, std::shared_ptr<AIAgent>> agentMap;
    
    std::thread managerThread;
    std::atomic<bool> running;
    std::mutex agentMutex;
    
    // Configuration
    int maxAgents;
    bool enableCollaboration;
    bool enableLearning;
    
public:
    AIAgentManager();
    ~AIAgentManager();
    
    // Agent management
    void addAgent(std::shared_ptr<AIAgent> agent);
    void removeAgent(const std::string& name);
    std::shared_ptr<AIAgent> getAgent(const std::string& name);
    std::vector<std::shared_ptr<AIAgent>> getAllAgents() const;
    void addAgent(const std::string& name, const std::string& type);
    std::vector<SimpleAgent> getSimpleAgents() const;
    
    // Control methods
    void startAllAgents();
    void stopAllAgents();
    void pauseAllAgents();
    void resumeAllAgents();
    
    // Configuration
    void setMaxAgents(int max) { maxAgents = max; }
    void setCollaboration(bool enable) { enableCollaboration = enable; }
    void setLearning(bool enable) { enableLearning = enable; }
    
    // Runtime
    void run();
    void shutdown();
    
    // Statistics
    int getAgentCount() const;
    int getActiveAgentCount() const;
    double getOverallSuccessRate() const;
    
private:
    void managerLoop();
    void coordinateAgents();
    void shareIntelligence();
    void monitorPerformance();
};

#endif // AI_AGENT_MANAGER_H 