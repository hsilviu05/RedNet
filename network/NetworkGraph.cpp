#include "NetworkGraph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <queue>
#include <set>

NetworkConnection::NetworkConnection() : sourceId(0), targetId(0), isActive(false), bandwidth(0.0f) {
}

NetworkConnection::NetworkConnection(int source, int target, const std::string& type, bool active, float bw)
    : sourceId(source), targetId(target), connectionType(type), isActive(active), bandwidth(bw) {
}

std::string NetworkConnection::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"sourceId\":" << sourceId << ",";
    json << "\"targetId\":" << targetId << ",";
    json << "\"type\":\"" << connectionType << "\",";
    json << "\"isActive\":" << (isActive ? "true" : "false") << ",";
    json << "\"bandwidth\":" << bandwidth;
    json << "}";
    return json.str();
}

NetworkGraph::NetworkGraph() : nextNodeId(1) {
}

NetworkGraph::NetworkGraph(const std::string& name) : networkName(name), nextNodeId(1) {
}

NetworkGraph::~NetworkGraph() {
}

void NetworkGraph::setName(const std::string& name) {
    networkName = name;
}

const std::string& NetworkGraph::getName() const {
    return networkName;
}

int NetworkGraph::addNode(std::shared_ptr<Node> node) {
    int nodeId = nextNodeId++;
    // node->setId(nodeId); // This line is commented out as per the edit hint
    nodes.push_back(node);
    adjacencyList[nodeId] = std::vector<int>();
    std::cout << "Added node: " << node->getHostname() << " (ID: " << nodeId << ")" << std::endl;
    return nodeId;
}

void NetworkGraph::removeNode(int nodeId) {
    // Remove node from nodes vector
    nodes.erase(
        std::remove_if(nodes.begin(), nodes.end(),
                      [nodeId](const std::shared_ptr<Node>& node) { return node->getId() == nodeId; }),
        nodes.end()
    );
    
    // Remove connections involving this node
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
                      [nodeId](const NetworkConnection& conn) { 
                          return conn.sourceId == nodeId || conn.targetId == nodeId; 
                      }),
        connections.end()
    );
    
    // Remove from adjacency list
    adjacencyList.erase(nodeId);
    
    // Remove from other nodes' adjacency lists
    for (auto& pair : adjacencyList) {
        pair.second.erase(
            std::remove(pair.second.begin(), pair.second.end(), nodeId),
            pair.second.end()
        );
    }
    
    std::cout << "Removed node with ID: " << nodeId << std::endl;
}

std::shared_ptr<Node> NetworkGraph::getNode(int nodeId) const {
    for (const auto& node : nodes) {
        if (node->getId() == nodeId) {
            return node;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Node>>& NetworkGraph::getAllNodes() const {
    return nodes;
}

void NetworkGraph::addConnection(const NetworkConnection& connection) {
    connections.push_back(connection);
    
    // Update adjacency list
    adjacencyList[connection.sourceId].push_back(connection.targetId);
    adjacencyList[connection.targetId].push_back(connection.sourceId);
    
    std::cout << "Added connection: " << connection.sourceId << " -> " << connection.targetId 
              << " (" << connection.connectionType << ")" << std::endl;
}

void NetworkGraph::removeConnection(int sourceId, int targetId) {
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
                      [sourceId, targetId](const NetworkConnection& conn) { 
                          return (conn.sourceId == sourceId && conn.targetId == targetId) ||
                                 (conn.sourceId == targetId && conn.targetId == sourceId); 
                      }),
        connections.end()
    );
    
    // Update adjacency list
    if (adjacencyList.find(sourceId) != adjacencyList.end()) {
        adjacencyList[sourceId].erase(
            std::remove(adjacencyList[sourceId].begin(), adjacencyList[sourceId].end(), targetId),
            adjacencyList[sourceId].end()
        );
    }
    
    if (adjacencyList.find(targetId) != adjacencyList.end()) {
        adjacencyList[targetId].erase(
            std::remove(adjacencyList[targetId].begin(), adjacencyList[targetId].end(), sourceId),
            adjacencyList[targetId].end()
        );
    }
    
    std::cout << "Removed connection: " << sourceId << " <-> " << targetId << std::endl;
}

const std::vector<NetworkConnection>& NetworkGraph::getAllConnections() const {
    return connections;
}

std::vector<NetworkConnection> NetworkGraph::getConnectionsForNode(int nodeId) const {
    std::vector<NetworkConnection> nodeConnections;
    for (const auto& conn : connections) {
        if (conn.sourceId == nodeId || conn.targetId == nodeId) {
            nodeConnections.push_back(conn);
        }
    }
    return nodeConnections;
}

std::vector<int> NetworkGraph::getNeighbors(int nodeId) const {
    if (adjacencyList.find(nodeId) != adjacencyList.end()) {
        return adjacencyList.at(nodeId);
    }
    return std::vector<int>();
}

std::vector<std::vector<int>> NetworkGraph::findPaths(int sourceId, int targetId) const {
    std::vector<std::vector<int>> paths;
    std::queue<std::vector<int>> queue;
    std::set<int> visited;
    
    queue.push({sourceId});
    visited.insert(sourceId);
    
    while (!queue.empty()) {
        std::vector<int> currentPath = queue.front();
        queue.pop();
        
        int currentNode = currentPath.back();
        
        if (currentNode == targetId) {
            paths.push_back(currentPath);
            continue;
        }
        
        for (int neighbor : getNeighbors(currentNode)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                std::vector<int> newPath = currentPath;
                newPath.push_back(neighbor);
                queue.push(newPath);
            }
        }
    }
    
    return paths;
}

bool NetworkGraph::isConnected(int sourceId, int targetId) const {
    auto paths = findPaths(sourceId, targetId);
    return !paths.empty();
}

void NetworkGraph::generateRandomTopology(int numNodes, int numConnections) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> nodeTypeDist(0, 9);
    std::uniform_int_distribution<int> ipDist(1, 254);
    
    // Clear existing topology
    nodes.clear();
    connections.clear();
    adjacencyList.clear();
    nextNodeId = 1;
    
    // Generate nodes
    for (int i = 0; i < numNodes; ++i) {
        NodeType type = static_cast<NodeType>(nodeTypeDist(gen));
        std::string ip = "192.168.1." + std::to_string(ipDist(gen));
        std::string hostname = "node-" + std::to_string(i + 1);
        
        auto node = std::make_shared<Node>(nextNodeId, ip, hostname, type);
        node->generateDefaultServices();
        node->generateDefaultUsers();
        node->generateDefaultFiles();
        
        addNode(node);
    }
    
    // Generate connections
    std::uniform_int_distribution<int> nodeDist(0, numNodes - 1);
    int connectionsCreated = 0;
    
    while (connectionsCreated < numConnections) {
        int sourceIdx = nodeDist(gen);
        int targetIdx = nodeDist(gen);
        
        if (sourceIdx != targetIdx) {
            int sourceId = nodes[sourceIdx]->getId();
            int targetId = nodes[targetIdx]->getId();
            
            // Check if connection already exists
            bool exists = false;
            for (const auto& conn : connections) {
                if ((conn.sourceId == sourceId && conn.targetId == targetId) ||
                    (conn.sourceId == targetId && conn.targetId == sourceId)) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                std::string connectionTypes[] = {"Ethernet", "WiFi", "Fiber", "Copper"};
                std::uniform_int_distribution<int> typeDist(0, 3);
                std::string type = connectionTypes[typeDist(gen)];
                
                addConnection(NetworkConnection(sourceId, targetId, type));
                connectionsCreated++;
            }
        }
    }
    
    std::cout << "Generated random topology with " << numNodes << " nodes and " 
              << numConnections << " connections." << std::endl;
}

void NetworkGraph::loadFromFile(const std::string& filename) {
    std::cout << "Loading network topology from " << filename << std::endl;
    // Implementation for loading from file
}

void NetworkGraph::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << toJson();
        file.close();
        std::cout << "Network topology saved to " << filename << std::endl;
    }
}

std::string NetworkGraph::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"name\":\"" << networkName << "\",";
    json << "\"nodes\":[";
    for (size_t i = 0; i < nodes.size(); ++i) {
        json << nodes[i]->toJson();
        if (i < nodes.size() - 1) json << ",";
    }
    json << "],\"connections\":[";
    for (size_t i = 0; i < connections.size(); ++i) {
        json << connections[i].toJson();
        if (i < connections.size() - 1) json << ",";
    }
    json << "]";
    json << "}";
    return json.str();
}

void NetworkGraph::printTopology() const {
    std::cout << "\n=== Network Topology: " << networkName << " ===" << std::endl;
    std::cout << "Nodes (" << nodes.size() << "):" << std::endl;
    
    for (const auto& node : nodes) {
        std::cout << "  " << node->getId() << ": " << node->getHostname() 
                  << " (" << node->getIp() << ") - " << node->getTypeName() << std::endl;
    }
    
    std::cout << "\nConnections (" << connections.size() << "):" << std::endl;
    for (const auto& conn : connections) {
        std::cout << "  " << conn.sourceId << " <-> " << conn.targetId 
                  << " (" << conn.connectionType << ")" << std::endl;
    }
    std::cout << "=====================================" << std::endl;
} 