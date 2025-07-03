#include "NetworkGraph.h"
#include "../engine/Node.h"
#include <algorithm>
#include <thread>
#include <sstream>
#include <iomanip>

NetworkGraph::NetworkGraph() 
    : maxHops(15), enableFirewall(true), gen(rd()) {
    defaultGateway = "192.168.1.1";
    dnsServer = "8.8.8.8";
}

NetworkGraph::NetworkGraph(const NetworkGraph& other)
    : maxHops(other.maxHops), enableFirewall(other.enableFirewall), gen(rd()) {
    defaultGateway = other.defaultGateway;
    dnsServer = other.dnsServer;
    nodes = other.nodes;
    adjacencyList = other.adjacencyList;
    links = other.links;
    routingTable = other.routingTable;
    firewallRules = other.firewallRules;
    packetHistory = other.packetHistory;
    packetHandlers = other.packetHandlers;
}

NetworkGraph::~NetworkGraph() {
    nodes.clear();
    links.clear();
    routingTable.clear();
    firewallRules.clear();
    packetHistory.clear();
}

// Node management
void NetworkGraph::addNode(const NetworkNode& node) {
    nodes[node.ip] = node;
    adjacencyList[node.ip] = std::vector<std::string>();
    
    // Add default route if this is the first node
    if (nodes.size() == 1) {
        Route defaultRoute("0.0.0.0/0", defaultGateway, "eth0");
        defaultRoute.isDefault = true;
        routingTable.push_back(defaultRoute);
    }
}

void NetworkGraph::removeNode(const std::string& ip) {
    if (nodes.find(ip) != nodes.end()) {
        nodes.erase(ip);
        adjacencyList.erase(ip);
        
        // Remove all links involving this node
        links.erase(
            std::remove_if(links.begin(), links.end(),
                [&ip](const NetworkLink& link) {
                    return link.sourceIP == ip || link.destIP == ip;
                }),
            links.end()
        );
        
        // Remove from adjacency lists
        for (auto& adj : adjacencyList) {
            adj.second.erase(
                std::remove(adj.second.begin(), adj.second.end(), ip),
                adj.second.end()
            );
        }
    }
}

NetworkNode* NetworkGraph::getNode(const std::string& ip) {
    if (nodes.find(ip) != nodes.end()) {
        return &nodes[ip];
    }
    return nullptr;
}

std::vector<NetworkNode> NetworkGraph::getAllNodes() const {
    std::vector<NetworkNode> result;
    for (const auto& pair : nodes) {
        result.push_back(pair.second);
    }
    return result;
}

bool NetworkGraph::nodeExists(const std::string& ip) const {
    return nodes.find(ip) != nodes.end();
}

// Link management
void NetworkGraph::addLink(const NetworkLink& link) {
    if (nodeExists(link.sourceIP) && nodeExists(link.destIP)) {
        links.push_back(link);
        
        // Add to adjacency list
        adjacencyList[link.sourceIP].push_back(link.destIP);
        adjacencyList[link.destIP].push_back(link.sourceIP);
    }
}

void NetworkGraph::removeLink(const std::string& sourceIP, const std::string& destIP) {
    // Remove from links vector
    links.erase(
        std::remove_if(links.begin(), links.end(),
            [&sourceIP, &destIP](const NetworkLink& link) {
                return (link.sourceIP == sourceIP && link.destIP == destIP) ||
                       (link.sourceIP == destIP && link.destIP == sourceIP);
            }),
        links.end()
    );
    
    // Remove from adjacency lists
    if (adjacencyList.find(sourceIP) != adjacencyList.end()) {
        adjacencyList[sourceIP].erase(
            std::remove(adjacencyList[sourceIP].begin(), adjacencyList[sourceIP].end(), destIP),
            adjacencyList[sourceIP].end()
        );
    }
    
    if (adjacencyList.find(destIP) != adjacencyList.end()) {
        adjacencyList[destIP].erase(
            std::remove(adjacencyList[destIP].begin(), adjacencyList[destIP].end(), sourceIP),
            adjacencyList[destIP].end()
        );
    }
}

std::vector<NetworkLink> NetworkGraph::getLinks() const {
    return links;
}

bool NetworkGraph::nodesAreConnected(const std::string& sourceIP, const std::string& destIP) const {
    if (adjacencyList.find(sourceIP) == adjacencyList.end()) {
        return false;
    }
    
    const auto& neighbors = adjacencyList.at(sourceIP);
    return std::find(neighbors.begin(), neighbors.end(), destIP) != neighbors.end();
}

// Routing
std::vector<std::string> NetworkGraph::findPath(const std::string& sourceIP, const std::string& destIP) {
    if (!nodeExists(sourceIP) || !nodeExists(destIP)) {
        return std::vector<std::string>();
    }
    
    // Try BFS first for simple path finding
    auto path = bfs(sourceIP, destIP);
    if (!path.empty()) {
        return path;
    }
    
    // Fall back to Dijkstra for more complex routing
    return dijkstra(sourceIP, destIP);
}

std::vector<std::string> NetworkGraph::bfs(const std::string& startIP, const std::string& targetIP) {
    std::queue<std::string> q;
    std::unordered_map<std::string, std::string> parent;
    std::unordered_set<std::string> visited;
    
    q.push(startIP);
    visited.insert(startIP);
    parent[startIP] = "";
    
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        
        if (current == targetIP) {
            // Reconstruct path
            std::vector<std::string> path;
            std::string node = targetIP;
            while (!node.empty()) {
                path.push_back(node);
                node = parent[node];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        if (adjacencyList.find(current) != adjacencyList.end()) {
            for (const std::string& neighbor : adjacencyList[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }
    }
    
    return std::vector<std::string>();
}

std::vector<std::string> NetworkGraph::dijkstra(const std::string& startIP, const std::string& targetIP) {
    std::map<std::string, int> distances;
    std::map<std::string, std::string> previous;
    std::set<std::pair<int, std::string>> pq;
    
    // Initialize distances
    for (const auto& node : nodes) {
        distances[node.first] = std::numeric_limits<int>::max();
    }
    distances[startIP] = 0;
    pq.insert({0, startIP});
    
    while (!pq.empty()) {
        std::string current = pq.begin()->second;
        pq.erase(pq.begin());
        
        if (current == targetIP) {
            // Reconstruct path
            std::vector<std::string> path;
            std::string node = targetIP;
            while (!node.empty()) {
                path.push_back(node);
                node = previous[node];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        if (adjacencyList.find(current) != adjacencyList.end()) {
            for (const std::string& neighbor : adjacencyList[current]) {
                int weight = 1; // Default weight
                
                // Find link weight
                for (const auto& link : links) {
                    if ((link.sourceIP == current && link.destIP == neighbor) ||
                        (link.sourceIP == neighbor && link.destIP == current)) {
                        weight = link.latency;
                        break;
                    }
                }
                
                int newDist = distances[current] + weight;
                if (newDist < distances[neighbor]) {
                    pq.erase({distances[neighbor], neighbor});
                    distances[neighbor] = newDist;
                    previous[neighbor] = current;
                    pq.insert({newDist, neighbor});
                }
            }
        }
    }
    
    return std::vector<std::string>();
}

std::vector<std::string> NetworkGraph::getReachableNodes(const std::string& sourceIP) {
    std::vector<std::string> reachable;
    std::unordered_set<std::string> visited;
    std::queue<std::string> q;
    
    q.push(sourceIP);
    visited.insert(sourceIP);
    
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        reachable.push_back(current);
        
        if (adjacencyList.find(current) != adjacencyList.end()) {
            for (const std::string& neighbor : adjacencyList[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }
    
    return reachable;
}

void NetworkGraph::addRoute(const Route& route) {
    routingTable.push_back(route);
}

void NetworkGraph::removeRoute(const std::string& destination) {
    routingTable.erase(
        std::remove_if(routingTable.begin(), routingTable.end(),
            [&destination](const Route& route) {
                return route.destination == destination;
            }),
        routingTable.end()
    );
}

std::vector<Route> NetworkGraph::getRoutingTable() const {
    return routingTable;
}

// Packet simulation
bool NetworkGraph::sendPacket(const Packet& packet) {
    if (!nodeExists(packet.sourceIP) || !nodeExists(packet.destIP)) {
        return false;
    }
    
    // Check firewall rules
    if (enableFirewall && !isPacketAllowed(packet)) {
        Packet blockedPacket = packet;
        blockedPacket.status = BLOCKED;
        packetHistory.push_back(blockedPacket);
        logPacketEvent(blockedPacket, "BLOCKED");
        return false;
    }
    
    // Add to packet queue
    packetQueue.push(packet);
    return true;
}

PacketStatus NetworkGraph::routePacket(Packet& packet) {
    if (packet.ttl <= 0) {
        packet.status = DROPPED;
        packetHistory.push_back(packet);
        logPacketEvent(packet, "TTL_EXPIRED");
        return DROPPED;
    }
    
    packet.ttl--;
    
    // Find path to destination
    std::vector<std::string> path = findPath(packet.sourceIP, packet.destIP);
    if (path.empty()) {
        packet.status = DROPPED;
        packetHistory.push_back(packet);
        logPacketEvent(packet, "NO_ROUTE");
        return DROPPED;
    }
    
    // Simulate network conditions
    simulatePacketLoss(packet);
    simulateNetworkDelay(packet);
    
    if (packet.status == DROPPED) {
        packetHistory.push_back(packet);
        logPacketEvent(packet, "PACKET_LOSS");
        return DROPPED;
    }
    
    // Mark as delivered
    packet.status = DELIVERED;
    packetHistory.push_back(packet);
    logPacketEvent(packet, "DELIVERED");
    
    // Notify packet handlers
    for (const auto& handler : packetHandlers) {
        handler(packet);
    }
    
    return DELIVERED;
}

std::vector<Packet> NetworkGraph::getPacketHistory() const {
    return packetHistory;
}

void NetworkGraph::clearPacketHistory() {
    packetHistory.clear();
}

// Firewall
void NetworkGraph::addFirewallRule(const FirewallRule& rule) {
    firewallRules.push_back(rule);
}

void NetworkGraph::removeFirewallRule(int index) {
    if (index >= 0 && index < static_cast<int>(firewallRules.size())) {
        firewallRules.erase(firewallRules.begin() + index);
    }
}

std::vector<FirewallRule> NetworkGraph::getFirewallRules() const {
    return firewallRules;
}

bool NetworkGraph::isPacketAllowed(const Packet& packet) const {
    return checkFirewallRules(packet);
}

bool NetworkGraph::checkFirewallRules(const Packet& packet) const {
    for (const auto& rule : firewallRules) {
        // Check if rule matches packet
        bool matches = true;
        
        if (!rule.sourceIP.empty() && rule.sourceIP != packet.sourceIP) {
            matches = false;
        }
        if (!rule.destIP.empty() && rule.destIP != packet.destIP) {
            matches = false;
        }
        if (rule.sourcePort != 0 && rule.sourcePort != packet.sourcePort) {
            matches = false;
        }
        if (rule.destPort != 0 && rule.destPort != packet.destPort) {
            matches = false;
        }
        if (rule.protocol != packet.protocol) {
            matches = false;
        }
        
        if (matches) {
            return rule.allow;
        }
    }
    
    // Default allow if no rules match
    return true;
}

void NetworkGraph::setFirewallEnabled(bool enabled) {
    enableFirewall = enabled;
}

// Network configuration
void NetworkGraph::setDefaultGateway(const std::string& gateway) {
    defaultGateway = gateway;
}

void NetworkGraph::setDNSServer(const std::string& dns) {
    dnsServer = dns;
}

void NetworkGraph::setMaxHops(int hops) {
    maxHops = hops;
}

std::string NetworkGraph::getDefaultGateway() const {
    return defaultGateway;
}

std::string NetworkGraph::getDNSServer() const {
    return dnsServer;
}

// Network discovery
std::vector<std::string> NetworkGraph::discoverNetwork(const std::string& startIP) {
    return getReachableNodes(startIP);
}

std::vector<std::string> NetworkGraph::getNeighbors(const std::string& ip) const {
    if (adjacencyList.find(ip) != adjacencyList.end()) {
        return adjacencyList.at(ip);
    }
    return std::vector<std::string>();
}

std::string NetworkGraph::getSubnet(const std::string& ip) const {
    // Simple subnet calculation (assuming /24)
    size_t lastDot = ip.find_last_of('.');
    if (lastDot != std::string::npos) {
        return ip.substr(0, lastDot) + ".0/24";
    }
    return ip;
}

// Network statistics
int NetworkGraph::getNodeCount() const {
    return static_cast<int>(nodes.size());
}

int NetworkGraph::getLinkCount() const {
    return static_cast<int>(links.size());
}

int NetworkGraph::getPacketCount() const {
    return static_cast<int>(packetHistory.size());
}

double NetworkGraph::getNetworkUtilization() const {
    if (links.empty()) return 0.0;
    
    int activeLinks = 0;
    for (const auto& link : links) {
        if (link.isActive) activeLinks++;
    }
    
    return static_cast<double>(activeLinks) / links.size();
}

// Event handling
void NetworkGraph::addPacketHandler(std::function<void(const Packet&)> handler) {
    packetHandlers.push_back(handler);
}

void NetworkGraph::removePacketHandler(int index) {
    if (index >= 0 && index < static_cast<int>(packetHandlers.size())) {
        packetHandlers.erase(packetHandlers.begin() + index);
    }
}

// Network simulation
void NetworkGraph::simulateNetworkTraffic() {
    processPacketQueue();
}

void NetworkGraph::updateNetworkState() {
    // Simulate network events
    simulateNetworkTraffic();
}

// Utility methods
std::string NetworkGraph::generateRandomIP() const {
    std::uniform_int_distribution<> dis(1, 254);
    std::stringstream ss;
    ss << "192.168.1." << dis(const_cast<std::mt19937&>(gen));
    return ss.str();
}

bool NetworkGraph::isValidIP(const std::string& ip) const {
    // Simple IP validation
    std::stringstream ss(ip);
    std::string segment;
    int count = 0;
    
    while (std::getline(ss, segment, '.')) {
        if (count >= 4) return false;
        
        try {
            int num = std::stoi(segment);
            if (num < 0 || num > 255) return false;
        } catch (...) {
            return false;
        }
        
        count++;
    }
    
    return count == 4;
}

std::string NetworkGraph::getNetworkAddress(const std::string& ip, int subnetMask) const {
    // Simple network address calculation
    size_t lastDot = ip.find_last_of('.');
    if (lastDot != std::string::npos) {
        return ip.substr(0, lastDot) + ".0";
    }
    return ip;
}

bool NetworkGraph::isInSameSubnet(const std::string& ip1, const std::string& ip2, int subnetMask) const {
    std::string network1 = getNetworkAddress(ip1, subnetMask);
    std::string network2 = getNetworkAddress(ip2, subnetMask);
    return network1 == network2;
}

// Private helper methods
void NetworkGraph::processPacketQueue() {
    while (!packetQueue.empty()) {
        Packet packet = packetQueue.front();
        packetQueue.pop();
        routePacket(packet);
    }
}

void NetworkGraph::simulatePacketLoss(Packet& packet) {
    // 1% packet loss probability
    std::uniform_real_distribution<> dis(0.0, 1.0);
    if (dis(gen) < 0.01) {
        packet.status = DROPPED;
    }
}

void NetworkGraph::simulateNetworkDelay(Packet& packet) {
    // Simulate network delay
    std::uniform_int_distribution<> delayDis(1, 10);
    int delay = delayDis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

std::string NetworkGraph::findNextHop(const std::string& sourceIP, const std::string& destIP) {
    std::vector<std::string> path = findPath(sourceIP, destIP);
    if (path.size() >= 2) {
        return path[1]; // Next hop is the second element
    }
    return destIP; // Direct connection
}

void NetworkGraph::logPacketEvent(const Packet& packet, const std::string& event) {
    // In a real implementation, this would log to a file or console
    // For now, we'll just store the event in packet history
}
