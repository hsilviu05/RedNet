#ifndef NETWORKGRAPH_H
#define NETWORKGRAPH_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../engine/Node.h"

struct NetworkConnection {
    int sourceId;
    int targetId;
    std::string connectionType;
    bool isActive;
    float bandwidth;
    
    NetworkConnection();
    NetworkConnection(int source, int target, const std::string& type, bool active = true, float bw = 100.0f);
    std::string toJson() const;
};

class NetworkGraph {
private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<NetworkConnection> connections;
    std::map<int, std::vector<int>> adjacencyList;
    std::string networkName;
    int nextNodeId;

public:
    NetworkGraph();
    NetworkGraph(const std::string& name);
    ~NetworkGraph();

    void setName(const std::string& name);
    const std::string& getName() const;
    
    int addNode(std::shared_ptr<Node> node);
    void removeNode(int nodeId);
    std::shared_ptr<Node> getNode(int nodeId) const;
    const std::vector<std::shared_ptr<Node>>& getAllNodes() const;
    
    void addConnection(const NetworkConnection& connection);
    void removeConnection(int sourceId, int targetId);
    const std::vector<NetworkConnection>& getAllConnections() const;
    std::vector<NetworkConnection> getConnectionsForNode(int nodeId) const;
    
    std::vector<int> getNeighbors(int nodeId) const;
    std::vector<std::vector<int>> findPaths(int sourceId, int targetId) const;
    bool isConnected(int sourceId, int targetId) const;
    
    void generateRandomTopology(int numNodes, int numConnections);
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    
    std::string toJson() const;
    void printTopology() const;
};

#endif // NETWORKGRAPH_H 