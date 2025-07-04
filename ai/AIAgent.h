#ifndef AIAGENT_H
#define AIAGENT_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

enum class AgentType {
    SCANNER,
    EXPLOITER,
    PAYLOAD_DELIVERY,
    PERSISTENCE,
    EXFILTRATION,
    COVERAGE
};

enum class AgentStatus {
    IDLE,
    ACTIVE,
    COMPLETED,
    FAILED
};

struct AgentTask {
    std::string description;
    std::string target;
    bool completed;
    std::string result;
};

class AIAgent {
private:
    int id;
    std::string name;
    AgentType type;
    AgentStatus status;
    std::vector<AgentTask> tasks;
    float successRate;
    bool isActive;

public:
    AIAgent();
    AIAgent(int agentId, const std::string& agentName, AgentType agentType);
    ~AIAgent();

    int getId() const;
    virtual std::string getName() const;
    AgentType getType() const;
    AgentStatus getStatus() const;
    virtual bool isAgentActive() const;
    virtual float getSuccessRate() const;

    virtual void activate() {}
    virtual void deactivate() {}
    virtual void addTask(const AgentTask& task);
    virtual bool executeTask(int taskIndex);
    virtual void updateStatus(AgentStatus newStatus);
    
    std::string getTypeName() const;
    std::string getStatusName() const;
    virtual const std::vector<AgentTask>& getTasks() const;
    virtual void setSuccessRate(float rate);
    
    std::string toJson() const;
};

#endif // AIAGENT_H 