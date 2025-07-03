#ifndef NETWORK_GRAPH_H
#define NETWORK_GRAPH_H

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
#include <memory>
#include <functional>

// Forward declarations
class Node;

enum NetworkProtocol {
    TCP,
    UDP_PROTOCOL,
    ICMP,
    HTTP,
    HTTPS,
    FTP,
    SSH,
    SMTP,
    DNS
};

enum PacketStatus {
    PENDING,
    IN_TRANSIT,
    DELIVERED,
    DROPPED,
    BLOCKED
};

struct NetworkNode {
    std::string ip;
    std::string hostname;
    std::string mac;
    std::string subnet;
    std::string gateway;
    std::vector<std::string> dnsServers;
    bool isRouter;
    bool isFirewall;
    std::shared_ptr<Node> nodePtr;
    
    NetworkNode() : isRouter(false), isFirewall(false) {}
    NetworkNode(std::string ip, std::shared_ptr<Node> node) : ip(ip), isRouter(false), isFirewall(false), nodePtr(node) {}
};

struct NetworkLink {
    std::string sourceIP;
    std::string destIP;
    int bandwidth; // Mbps
    int latency; // milliseconds
    bool isActive;
    std::string interface;
    
    NetworkLink() : bandwidth(100), latency(1), isActive(true) {}
    NetworkLink(std::string src, std::string dst) : sourceIP(src), destIP(dst), bandwidth(100), latency(1), isActive(true) {}
};

struct Packet {
    std::string sourceIP;
    std::string destIP;
    int sourcePort;
    int destPort;
    NetworkProtocol protocol;
    std::string payload;
    PacketStatus status;
    int ttl; // Time to live
    std::chrono::steady_clock::time_point timestamp;
    
    Packet() : sourcePort(0), destPort(0), protocol(TCP), status(PENDING), ttl(64) {
        timestamp = std::chrono::steady_clock::now();
    }
    
    Packet(std::string src, std::string dst, NetworkProtocol proto) 
        : sourceIP(src), destIP(dst), sourcePort(0), destPort(0), protocol(proto), status(PENDING), ttl(64) {
        timestamp = std::chrono::steady_clock::now();
    }
};

struct Route {
    std::string destination;
    std::string nextHop;
    std::string interface;
    int metric;
    bool isDefault;
    
    Route() : metric(1), isDefault(false) {}
    Route(std::string dest, std::string hop, std::string iface) 
        : destination(dest), nextHop(hop), interface(iface), metric(1), isDefault(false) {}
};

struct FirewallRule {
    std::string sourceIP;
    std::string destIP;
    int sourcePort;
    int destPort;
    NetworkProtocol protocol;
    bool allow;
    std::string description;
    
    FirewallRule() : sourcePort(0), destPort(0), protocol(TCP), allow(true) {}
};

class NetworkGraph {
private:
    // Network topology
    std::unordered_map<std::string, NetworkNode> nodes;
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;
    std::vector<NetworkLink> links;
    std::vector<Route> routingTable;
    std::vector<FirewallRule> firewallRules;
    
    // Network configuration
    std::string defaultGateway;
    std::string dnsServer;
    int maxHops;
    bool enableFirewall;
    
    // Packet simulation
    std::queue<Packet> packetQueue;
    std::vector<Packet> packetHistory;
    std::random_device rd;
    std::mt19937 gen;
    
    // Network events
    std::vector<std::function<void(const Packet&)>> packetHandlers;
    
public:
    NetworkGraph();
    NetworkGraph(const NetworkGraph& other);
    ~NetworkGraph();
    
    // Node management
    void addNode(const NetworkNode& node);
    void removeNode(const std::string& ip);
    NetworkNode* getNode(const std::string& ip);
    std::vector<NetworkNode> getAllNodes() const;
    bool nodeExists(const std::string& ip) const;
    
    // Link management
    void addLink(const NetworkLink& link);
    void removeLink(const std::string& sourceIP, const std::string& destIP);
    std::vector<NetworkLink> getLinks() const;
    bool nodesAreConnected(const std::string& sourceIP, const std::string& destIP) const;
    
    // Routing
    std::vector<std::string> findPath(const std::string& sourceIP, const std::string& destIP);
    std::vector<std::string> getReachableNodes(const std::string& sourceIP);
    void addRoute(const Route& route);
    void removeRoute(const std::string& destination);
    std::vector<Route> getRoutingTable() const;
    
    // Packet simulation
    bool sendPacket(const Packet& packet);
    PacketStatus routePacket(Packet& packet);
    std::vector<Packet> getPacketHistory() const;
    void clearPacketHistory();
    
    // Firewall
    void addFirewallRule(const FirewallRule& rule);
    void removeFirewallRule(int index);
    std::vector<FirewallRule> getFirewallRules() const;
    bool isPacketAllowed(const Packet& packet) const;
    void setFirewallEnabled(bool enabled);
    
    // Network configuration
    void setDefaultGateway(const std::string& gateway);
    void setDNSServer(const std::string& dns);
    void setMaxHops(int hops);
    std::string getDefaultGateway() const;
    std::string getDNSServer() const;
    
    // Network discovery
    std::vector<std::string> discoverNetwork(const std::string& startIP);
    std::vector<std::string> getNeighbors(const std::string& ip) const;
    std::string getSubnet(const std::string& ip) const;
    
    // Network statistics
    int getNodeCount() const;
    int getLinkCount() const;
    int getPacketCount() const;
    double getNetworkUtilization() const;
    
    // Event handling
    void addPacketHandler(std::function<void(const Packet&)> handler);
    void removePacketHandler(int index);
    
    // Network simulation
    void simulateNetworkTraffic();
    void updateNetworkState();
    
    // Utility methods
    std::string generateRandomIP() const;
    bool isValidIP(const std::string& ip) const;
    std::string getNetworkAddress(const std::string& ip, int subnetMask) const;
    bool isInSameSubnet(const std::string& ip1, const std::string& ip2, int subnetMask) const;
    
private:
    // Internal helper methods
    std::vector<std::string> bfs(const std::string& startIP, const std::string& targetIP);
    std::vector<std::string> dijkstra(const std::string& startIP, const std::string& targetIP);
    void processPacketQueue();
    void simulatePacketLoss(Packet& packet);
    void simulateNetworkDelay(Packet& packet);
    std::string findNextHop(const std::string& sourceIP, const std::string& destIP);
    bool checkFirewallRules(const Packet& packet) const;
    void logPacketEvent(const Packet& packet, const std::string& event);
};

#endif // NETWORK_GRAPH_H
