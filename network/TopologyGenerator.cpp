#include "TopologyGenerator.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

TopologyGenerator::TopologyGenerator() : generatorName("DefaultGenerator") {
}

TopologyGenerator::TopologyGenerator(const std::string& name) : generatorName(name) {
}

TopologyGenerator::~TopologyGenerator() {
}

void TopologyGenerator::setName(const std::string& name) {
    generatorName = name;
}

const std::string& TopologyGenerator::getName() const {
    return generatorName;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateTopology(TopologyType type, int numNodes, int numConnections) {
    std::cout << "Generating " << getTopologyTypeName(type) << " topology with " << numNodes << " nodes" << std::endl;
    
    switch (type) {
        case TopologyType::RANDOM:
            return generateRandomTopology(numNodes, numConnections);
        case TopologyType::STAR:
            return generateStarTopology(numNodes);
        case TopologyType::RING:
            return generateRingTopology(numNodes);
        case TopologyType::MESH:
            return generateMeshTopology(numNodes);
        case TopologyType::TREE:
            return generateTreeTopology(numNodes);
        case TopologyType::BUS:
            return generateBusTopology(numNodes);
        case TopologyType::HYBRID:
            return generateHybridTopology(numNodes);
        default:
            return generateRandomTopology(numNodes, numConnections);
    }
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateRandomTopology(int numNodes, int numConnections) {
    auto graph = std::make_shared<NetworkGraph>("Random Topology");
    graph->generateRandomTopology(numNodes, numConnections);
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateStarTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Star Topology");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create central hub
    NodeType hubType = static_cast<NodeType>(nodeTypeDist(gen));
    auto hub = std::make_shared<Node>(1, "192.168.1.1", "hub", hubType);
    hub->generateDefaultServices();
    hub->generateDefaultUsers();
    hub->generateDefaultFiles();
    graph->addNode(hub);
    
    // Create peripheral nodes
    for (int i = 2; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        int nodeId = graph->addNode(node);
        
        // Connect to hub
        graph->addConnection(NetworkConnection(1, nodeId, "Ethernet"));
    }
    
    std::cout << "Generated star topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateRingTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Ring Topology");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create nodes
    for (int i = 1; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        graph->addNode(node);
    }
    
    // Connect nodes in a ring
    for (int i = 1; i <= numNodes; ++i) {
        int next = (i % numNodes) + 1;
        graph->addConnection(NetworkConnection(i, next, "Ethernet"));
    }
    
    std::cout << "Generated ring topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateMeshTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Mesh Topology");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create nodes
    for (int i = 1; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        graph->addNode(node);
    }
    
    // Connect each node to every other node
    for (int i = 1; i <= numNodes; ++i) {
        for (int j = i + 1; j <= numNodes; ++j) {
            graph->addConnection(NetworkConnection(i, j, "Ethernet"));
        }
    }
    
    std::cout << "Generated mesh topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateTreeTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Tree Topology");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create root node
    NodeType rootType = static_cast<NodeType>(nodeTypeDist(gen));
    auto root = std::make_shared<Node>(1, "192.168.1.1", "root", rootType);
    root->generateDefaultServices();
    root->generateDefaultUsers();
    root->generateDefaultFiles();
    graph->addNode(root);
    
    // Create child nodes
    for (int i = 2; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        int nodeId = graph->addNode(node);
        
        // Connect to parent (simple tree structure)
        int parent = (i - 2) / 2 + 1; // Simple parent calculation
        if (parent < i) {
            graph->addConnection(NetworkConnection(parent, nodeId, "Ethernet"));
        }
    }
    
    std::cout << "Generated tree topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateBusTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Bus Topology");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create nodes
    for (int i = 1; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        graph->addNode(node);
    }
    
    // Connect nodes in a bus (linear chain)
    for (int i = 1; i < numNodes; ++i) {
        graph->addConnection(NetworkConnection(i, i + 1, "Bus"));
    }
    
    std::cout << "Generated bus topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::shared_ptr<NetworkGraph> TopologyGenerator::generateHybridTopology(int numNodes) {
    auto graph = std::make_shared<NetworkGraph>("Hybrid Topology");
    
    // Generate a combination of star and ring
    int starNodes = numNodes / 2;
    int ringNodes = numNodes - starNodes;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Create star center
    NodeType hubType = static_cast<NodeType>(nodeTypeDist(gen));
    auto hub = std::make_shared<Node>(1, "192.168.1.1", "hub", hubType);
    hub->generateDefaultServices();
    hub->generateDefaultUsers();
    hub->generateDefaultFiles();
    graph->addNode(hub);
    
    // Create star nodes
    for (int i = 2; i <= starNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "star-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        int nodeId = graph->addNode(node);
        graph->addConnection(NetworkConnection(1, nodeId, "Ethernet"));
    }
    
    // Create ring nodes
    for (int i = starNodes + 1; i <= numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "ring-" + std::to_string(i);
        
        auto node = std::make_shared<Node>(i, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        graph->addNode(node);
    }
    
    // Connect ring nodes
    for (int i = starNodes + 1; i < numNodes; ++i) {
        graph->addConnection(NetworkConnection(i, i + 1, "Ring"));
    }
    if (ringNodes > 1) {
        graph->addConnection(NetworkConnection(starNodes + 1, numNodes, "Ring"));
    }
    
    // Connect hub to ring
    if (ringNodes > 0) {
        graph->addConnection(NetworkConnection(1, starNodes + 1, "Hybrid"));
    }
    
    std::cout << "Generated hybrid topology with " << numNodes << " nodes" << std::endl;
    return graph;
}

std::string TopologyGenerator::getTopologyTypeName(TopologyType type) {
    switch (type) {
        case TopologyType::RANDOM: return "Random";
        case TopologyType::STAR: return "Star";
        case TopologyType::RING: return "Ring";
        case TopologyType::MESH: return "Mesh";
        case TopologyType::TREE: return "Tree";
        case TopologyType::BUS: return "Bus";
        case TopologyType::HYBRID: return "Hybrid";
        default: return "Unknown";
    }
}

void TopologyGenerator::saveTopology(const std::shared_ptr<NetworkGraph>& graph, const std::string& filename) {
    if (graph) {
        graph->saveToFile(filename);
    }
}

std::shared_ptr<NetworkGraph> TopologyGenerator::loadTopology(const std::string& filename) {
    auto graph = std::make_shared<NetworkGraph>("Loaded Topology");
    graph->loadFromFile(filename);
    return graph;
} 