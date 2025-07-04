#ifndef TOPOLOGYGENERATOR_H
#define TOPOLOGYGENERATOR_H

#include <string>
#include <memory>
#include "NetworkGraph.h"

enum class TopologyType {
    RANDOM,
    STAR,
    RING,
    MESH,
    TREE,
    BUS,
    HYBRID,
    CORPORATE,
    DATA_CENTER,
    HOME
};

class TopologyGenerator {
private:
    std::string generatorName;

public:
    TopologyGenerator();
    TopologyGenerator(const std::string& name);
    ~TopologyGenerator();

    void setName(const std::string& name);
    const std::string& getName() const;
    
    std::shared_ptr<NetworkGraph> generateTopology(TopologyType type, int numNodes, int numConnections = 0);
    std::shared_ptr<NetworkGraph> generateRandomTopology(int numNodes, int numConnections);
    std::shared_ptr<NetworkGraph> generateStarTopology(int numNodes);
    std::shared_ptr<NetworkGraph> generateRingTopology(int numNodes);
    std::shared_ptr<NetworkGraph> generateMeshTopology(int numNodes);
    std::shared_ptr<NetworkGraph> generateTreeTopology(int numNodes);
    std::shared_ptr<NetworkGraph> generateBusTopology(int numNodes);
    std::shared_ptr<NetworkGraph> generateHybridTopology(int numNodes);
    
    static std::string getTopologyTypeName(TopologyType type);
    void saveTopology(const std::shared_ptr<NetworkGraph>& graph, const std::string& filename);
    std::shared_ptr<NetworkGraph> loadTopology(const std::string& filename);
};

#endif // TOPOLOGYGENERATOR_H 