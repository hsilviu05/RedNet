#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../network/NetworkGraph.h"
#include "../engine/Exploit.h"

class AdvancedVisualizer {
private:
    std::shared_ptr<NetworkGraph> network;
    std::shared_ptr<ExploitDatabase> exploitDB;
public:
    AdvancedVisualizer(std::shared_ptr<NetworkGraph> network, std::shared_ptr<ExploitDatabase> exploitDB);
    ~AdvancedVisualizer();
    void renderNetworkTopology();
    void renderExploitActivity();
    void renderAgentStatus();
    void updateVisualization();
    void exportVisualization(const std::string& format);
}; 