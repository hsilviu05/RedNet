#ifndef TOPOLOGY_GENERATOR_H
#define TOPOLOGY_GENERATOR_H

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
#include <memory>
#include "NetworkGraph.h"
#include "../engine/Node.h"

enum TopologyType {
    STAR_TOPOLOGY,
    MESH_TOPOLOGY,
    TREE_TOPOLOGY,
    RING_TOPOLOGY,
    BUS_TOPOLOGY,
    HYBRID_TOPOLOGY,
    CORPORATE_NETWORK,
    DATA_CENTER,
    HOME_NETWORK,
    CUSTOM_TOPOLOGY
};

enum NetworkSegment {
    DMZ_SEGMENT,
    INTERNAL_SEGMENT,
    MANAGEMENT_SEGMENT,
    GUEST_SEGMENT,
    DATABASE_SEGMENT,
    STORAGE_SEGMENT
};

struct NetworkTemplate {
    std::string name;
    std::string description;
    TopologyType type;
    int nodeCount;
    std::vector<std::string> subnets;
    std::vector<NetworkSegment> segments;
    std::map<std::string, std::string> nodeTypes;
    std::vector<std::pair<std::string, std::string>> connections;
    
    NetworkTemplate() : type(STAR_TOPOLOGY), nodeCount(0) {}
};

struct SubnetConfig {
    std::string networkAddress;
    int subnetMask;
    std::string gateway;
    std::vector<std::string> dnsServers;
    std::string description;
    
    SubnetConfig() : subnetMask(24) {}
    SubnetConfig(std::string network, int mask) : networkAddress(network), subnetMask(mask) {}
};

class TopologyGenerator {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::vector<NetworkTemplate> templates;
    std::unordered_map<std::string, SubnetConfig> subnetConfigs;
    
public:
    TopologyGenerator();
    ~TopologyGenerator();
    
    // Template management
    void loadDefaultTemplates();
    void addTemplate(const NetworkTemplate& templ);
    NetworkTemplate getTemplate(const std::string& name);
    std::vector<NetworkTemplate> getAllTemplates() const;
    
    // Topology generation
    NetworkGraph generateTopology(TopologyType type, int nodeCount);
    NetworkGraph generateTopologyFromTemplate(const std::string& templateName);
    NetworkGraph generateCustomTopology(const std::vector<std::string>& ips, 
                                       const std::vector<std::pair<std::string, std::string>>& connections);
    
    // Specific topology generators
    NetworkGraph generateStarTopology(int nodeCount);
    NetworkGraph generateMeshTopology(int nodeCount);
    NetworkGraph generateTreeTopology(int levels, int nodesPerLevel);
    NetworkGraph generateRingTopology(int nodeCount);
    NetworkGraph generateBusTopology(int nodeCount);
    NetworkGraph generateHybridTopology(int nodeCount);
    
    // Network templates
    NetworkGraph generateCorporateNetwork();
    NetworkGraph generateDataCenter();
    NetworkGraph generateHomeNetwork();
    
    // Subnet management
    void addSubnetConfig(const std::string& name, const SubnetConfig& config);
    SubnetConfig getSubnetConfig(const std::string& name);
    std::vector<std::string> generateIPRange(const std::string& network, int subnetMask, int count);
    
    // Node creation helpers
    std::shared_ptr<Node> createNodeByType(const std::string& nodeType, const std::string& ip);
    std::vector<std::shared_ptr<Node>> createNodesForTopology(const NetworkTemplate& templ);
    
    // Utility methods
    std::string generateRandomIP(const std::string& network, int subnetMask);
    std::string getNextAvailableIP(const std::string& network, int subnetMask, 
                                  const std::unordered_set<std::string>& usedIPs);
    bool isValidSubnet(const std::string& network, int subnetMask);
    std::string calculateBroadcastAddress(const std::string& network, int subnetMask);
    
private:
    // Internal helper methods
    void initializeDefaultTemplates();
    void initializeSubnetConfigs();
    std::vector<std::string> generateIPSequence(const std::string& startIP, int count);
    std::string incrementIP(const std::string& ip);
    std::string decrementIP(const std::string& ip);
    bool isIPInRange(const std::string& ip, const std::string& network, int subnetMask);
    std::vector<std::pair<std::string, std::string>> generateConnections(TopologyType type, 
                                                                         const std::vector<std::string>& ips);
    void addFirewallRules(NetworkGraph& graph, NetworkSegment segment);
    void configureRouting(NetworkGraph& graph, const std::vector<SubnetConfig>& subnets);
};

#endif // TOPOLOGY_GENERATOR_H
