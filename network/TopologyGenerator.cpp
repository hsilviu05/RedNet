#include "TopologyGenerator.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

TopologyGenerator::TopologyGenerator() : gen(rd()) {
    initializeDefaultTemplates();
    initializeSubnetConfigs();
}

TopologyGenerator::~TopologyGenerator() {
    templates.clear();
    subnetConfigs.clear();
}

// Template management
void TopologyGenerator::loadDefaultTemplates() {
    initializeDefaultTemplates();
}

void TopologyGenerator::addTemplate(const NetworkTemplate& templ) {
    templates.push_back(templ);
}

NetworkTemplate TopologyGenerator::getTemplate(const std::string& name) {
    for (const auto& templ : templates) {
        if (templ.name == name) {
            return templ;
        }
    }
    return NetworkTemplate();
}

std::vector<NetworkTemplate> TopologyGenerator::getAllTemplates() const {
    return templates;
}

// Topology generation
NetworkGraph TopologyGenerator::generateTopology(TopologyType type, int nodeCount) {
    switch (type) {
        case STAR_TOPOLOGY:
            return generateStarTopology(nodeCount);
        case MESH_TOPOLOGY:
            return generateMeshTopology(nodeCount);
        case TREE_TOPOLOGY:
            return generateTreeTopology(3, nodeCount / 3);
        case RING_TOPOLOGY:
            return generateRingTopology(nodeCount);
        case BUS_TOPOLOGY:
            return generateBusTopology(nodeCount);
        case HYBRID_TOPOLOGY:
            return generateHybridTopology(nodeCount);
        case CORPORATE_NETWORK:
            return generateCorporateNetwork();
        case DATA_CENTER:
            return generateDataCenter();
        case HOME_NETWORK:
            return generateHomeNetwork();
        default:
            return generateStarTopology(nodeCount);
    }
}

NetworkGraph TopologyGenerator::generateTopologyFromTemplate(const std::string& templateName) {
    NetworkTemplate templ = getTemplate(templateName);
    if (templ.name.empty()) {
        return NetworkGraph();
    }
    
    return generateTopology(templ.type, templ.nodeCount);
}

NetworkGraph TopologyGenerator::generateCustomTopology(const std::vector<std::string>& ips, 
                                                       const std::vector<std::pair<std::string, std::string>>& connections) {
    NetworkGraph graph;
    
    // Add nodes
    for (const std::string& ip : ips) {
        auto node = std::make_shared<Node>(ip, LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ip, node);
        networkNode.hostname = "node-" + ip.substr(ip.find_last_of('.') + 1);
        graph.addNode(networkNode);
    }
    
    // Add connections
    for (const auto& connection : connections) {
        NetworkLink link(connection.first, connection.second);
        graph.addLink(link);
    }
    
    return graph;
}

// Specific topology generators
NetworkGraph TopologyGenerator::generateStarTopology(int nodeCount) {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, nodeCount);
    
    // Create central hub (first IP)
    auto hubNode = std::make_shared<Node>(ips[0], LINUX);
    hubNode->createWorkstation();
    
    NetworkNode hub(ips[0], hubNode);
    hub.hostname = "hub";
    hub.isRouter = true;
    graph.addNode(hub);
    
    // Create spoke nodes
    for (int i = 1; i < nodeCount; i++) {
        auto node = std::make_shared<Node>(ips[i], LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ips[i], node);
        networkNode.hostname = "spoke-" + std::to_string(i);
        graph.addNode(networkNode);
        
        // Connect to hub
        NetworkLink link(ips[i], ips[0]);
        graph.addLink(link);
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateMeshTopology(int nodeCount) {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, nodeCount);
    
    // Create all nodes
    for (int i = 0; i < nodeCount; i++) {
        auto node = std::make_shared<Node>(ips[i], LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ips[i], node);
        networkNode.hostname = "mesh-" + std::to_string(i);
        graph.addNode(networkNode);
    }
    
    // Connect every node to every other node
    for (int i = 0; i < nodeCount; i++) {
        for (int j = i + 1; j < nodeCount; j++) {
            NetworkLink link(ips[i], ips[j]);
            graph.addLink(link);
        }
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateTreeTopology(int levels, int nodesPerLevel) {
    NetworkGraph graph;
    
    std::vector<std::string> ips;
    int totalNodes = 0;
    
    // Calculate total nodes
    for (int i = 0; i < levels; i++) {
        totalNodes += nodesPerLevel * (1 << i);
    }
    
    // Generate IPs
    ips = generateIPRange("192.168.1.0", 24, totalNodes);
    
    int ipIndex = 0;
    
    // Create root node
    auto rootNode = std::make_shared<Node>(ips[ipIndex], LINUX);
    rootNode->createWorkstation();
    
    NetworkNode root(ips[ipIndex], rootNode);
    root.hostname = "root";
    root.isRouter = true;
    graph.addNode(root);
    ipIndex++;
    
    // Create tree levels
    for (int level = 1; level < levels; level++) {
        int nodesInLevel = nodesPerLevel * (1 << (level - 1));
        int parentNodes = nodesPerLevel * (1 << (level - 2));
        
        for (int node = 0; node < nodesInLevel; node++) {
            auto treeNode = std::make_shared<Node>(ips[ipIndex], LINUX);
            treeNode->createWorkstation();
            
            NetworkNode networkNode(ips[ipIndex], treeNode);
            networkNode.hostname = "level" + std::to_string(level) + "-" + std::to_string(node);
            graph.addNode(networkNode);
            
            // Connect to parent
            int parentIndex = (node / 2) + (ipIndex - nodesInLevel);
            if (parentIndex < ipIndex) {
                NetworkLink link(ips[ipIndex], ips[parentIndex]);
                graph.addLink(link);
            }
            
            ipIndex++;
        }
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateRingTopology(int nodeCount) {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, nodeCount);
    
    // Create all nodes
    for (int i = 0; i < nodeCount; i++) {
        auto node = std::make_shared<Node>(ips[i], LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ips[i], node);
        networkNode.hostname = "ring-" + std::to_string(i);
        graph.addNode(networkNode);
    }
    
    // Connect nodes in a ring
    for (int i = 0; i < nodeCount; i++) {
        int next = (i + 1) % nodeCount;
        NetworkLink link(ips[i], ips[next]);
        graph.addLink(link);
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateBusTopology(int nodeCount) {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, nodeCount);
    
    // Create all nodes
    for (int i = 0; i < nodeCount; i++) {
        auto node = std::make_shared<Node>(ips[i], LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ips[i], node);
        networkNode.hostname = "bus-" + std::to_string(i);
        graph.addNode(networkNode);
    }
    
    // Connect nodes in a bus (linear chain)
    for (int i = 0; i < nodeCount - 1; i++) {
        NetworkLink link(ips[i], ips[i + 1]);
        graph.addLink(link);
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateHybridTopology(int nodeCount) {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, nodeCount);
    
    // Create all nodes
    for (int i = 0; i < nodeCount; i++) {
        auto node = std::make_shared<Node>(ips[i], LINUX);
        node->createWorkstation();
        
        NetworkNode networkNode(ips[i], node);
        networkNode.hostname = "hybrid-" + std::to_string(i);
        graph.addNode(networkNode);
    }
    
    // Create a hybrid topology (star + some mesh connections)
    // Connect first node as hub
    for (int i = 1; i < nodeCount; i++) {
        NetworkLink link(ips[i], ips[0]);
        graph.addLink(link);
    }
    
    // Add some additional connections for redundancy
    for (int i = 1; i < nodeCount - 1; i += 2) {
        NetworkLink link(ips[i], ips[i + 1]);
        graph.addLink(link);
    }
    
    return graph;
}

// Network templates
NetworkGraph TopologyGenerator::generateCorporateNetwork() {
    NetworkGraph graph;
    
    // Create subnets
    std::vector<std::string> dmzIPs = generateIPRange("10.0.1.0", 24, 5);
    std::vector<std::string> internalIPs = generateIPRange("10.0.2.0", 24, 10);
    std::vector<std::string> managementIPs = generateIPRange("10.0.3.0", 24, 3);
    
    // DMZ segment
    auto firewall = std::make_shared<Node>(dmzIPs[0], LINUX);
    NetworkNode fwNode(dmzIPs[0], firewall);
    fwNode.hostname = "firewall";
    fwNode.isFirewall = true;
    graph.addNode(fwNode);
    
    auto webServer = std::make_shared<Node>(dmzIPs[1], LINUX);
    webServer->createWebServer();
    NetworkNode webNode(dmzIPs[1], webServer);
    webNode.hostname = "webserver";
    graph.addNode(webNode);
    
    // Internal segment
    for (int i = 0; i < 5; i++) {
        auto workstation = std::make_shared<Node>(internalIPs[i], WINDOWS);
        workstation->createWorkstation();
        NetworkNode wsNode(internalIPs[i], workstation);
        wsNode.hostname = "workstation-" + std::to_string(i);
        graph.addNode(wsNode);
    }
    
    auto fileServer = std::make_shared<Node>(internalIPs[5], LINUX);
    fileServer->createFileServer();
    NetworkNode fsNode(internalIPs[5], fileServer);
    fsNode.hostname = "fileserver";
    graph.addNode(fsNode);
    
    // Management segment
    auto domainController = std::make_shared<Node>(managementIPs[0], WINDOWS);
    domainController->createDomainController();
    NetworkNode dcNode(managementIPs[0], domainController);
    dcNode.hostname = "domain-controller";
    graph.addNode(dcNode);
    
    // Connect segments
    NetworkLink fwWeb(dmzIPs[0], dmzIPs[1]);
    NetworkLink fwInternal(dmzIPs[0], internalIPs[0]);
    NetworkLink internalMgmt(internalIPs[0], managementIPs[0]);
    
    graph.addLink(fwWeb);
    graph.addLink(fwInternal);
    graph.addLink(internalMgmt);
    
    // Add firewall rules
    FirewallRule rule1;
    rule1.sourceIP = "0.0.0.0";
    rule1.destIP = dmzIPs[1];
    rule1.destPort = 80;
    rule1.protocol = HTTP;
    rule1.allow = true;
    rule1.description = "Allow HTTP to web server";
    graph.addFirewallRule(rule1);
    
    return graph;
}

NetworkGraph TopologyGenerator::generateDataCenter() {
    NetworkGraph graph;
    
    // Create subnets
    std::vector<std::string> webIPs = generateIPRange("10.1.1.0", 24, 5);
    std::vector<std::string> appIPs = generateIPRange("10.1.2.0", 24, 5);
    std::vector<std::string> dbIPs = generateIPRange("10.1.3.0", 24, 3);
    std::vector<std::string> storageIPs = generateIPRange("10.1.4.0", 24, 2);
    
    // Web tier
    for (int i = 0; i < 5; i++) {
        auto webServer = std::make_shared<Node>(webIPs[i], LINUX);
        webServer->createWebServer();
        NetworkNode webNode(webIPs[i], webServer);
        webNode.hostname = "web-" + std::to_string(i);
        graph.addNode(webNode);
    }
    
    // Application tier
    for (int i = 0; i < 5; i++) {
        auto appServer = std::make_shared<Node>(appIPs[i], LINUX);
        appServer->createWebServer();
        NetworkNode appNode(appIPs[i], appServer);
        appNode.hostname = "app-" + std::to_string(i);
        graph.addNode(appNode);
    }
    
    // Database tier
    for (int i = 0; i < 3; i++) {
        auto dbServer = std::make_shared<Node>(dbIPs[i], LINUX);
        dbServer->createWorkstation();
        NetworkNode dbNode(dbIPs[i], dbServer);
        dbNode.hostname = "db-" + std::to_string(i);
        graph.addNode(dbNode);
    }
    
    // Storage tier
    for (int i = 0; i < 2; i++) {
        auto storageServer = std::make_shared<Node>(storageIPs[i], LINUX);
        storageServer->createFileServer();
        NetworkNode storageNode(storageIPs[i], storageServer);
        storageNode.hostname = "storage-" + std::to_string(i);
        graph.addNode(storageNode);
    }
    
    // Connect tiers
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            NetworkLink link(webIPs[i], appIPs[j]);
            graph.addLink(link);
        }
    }
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            NetworkLink link(appIPs[i], dbIPs[j]);
            graph.addLink(link);
        }
    }
    
    return graph;
}

NetworkGraph TopologyGenerator::generateHomeNetwork() {
    NetworkGraph graph;
    
    // Generate IPs
    std::vector<std::string> ips = generateIPRange("192.168.1.0", 24, 6);
    
    // Router
    auto router = std::make_shared<Node>(ips[0], LINUX);
    NetworkNode routerNode(ips[0], router);
    routerNode.hostname = "router";
    routerNode.isRouter = true;
    graph.addNode(routerNode);
    
    // Devices
    auto laptop = std::make_shared<Node>(ips[1], WINDOWS);
    laptop->createWorkstation();
    NetworkNode laptopNode(ips[1], laptop);
    laptopNode.hostname = "laptop";
    graph.addNode(laptopNode);
    
    auto desktop = std::make_shared<Node>(ips[2], WINDOWS);
    desktop->createWorkstation();
    NetworkNode desktopNode(ips[2], desktop);
    desktopNode.hostname = "desktop";
    graph.addNode(desktopNode);
    
    auto phone = std::make_shared<Node>(ips[3], LINUX);
    phone->createWorkstation();
    NetworkNode phoneNode(ips[3], phone);
    phoneNode.hostname = "phone";
    graph.addNode(phoneNode);
    
    auto tv = std::make_shared<Node>(ips[4], LINUX);
    tv->createWorkstation();
    NetworkNode tvNode(ips[4], tv);
    tvNode.hostname = "tv";
    graph.addNode(tvNode);
    
    auto nas = std::make_shared<Node>(ips[5], LINUX);
    nas->createFileServer();
    NetworkNode nasNode(ips[5], nas);
    nasNode.hostname = "nas";
    graph.addNode(nasNode);
    
    // Connect all devices to router
    for (int i = 1; i < 6; i++) {
        NetworkLink link(ips[i], ips[0]);
        graph.addLink(link);
    }
    
    return graph;
}

// Subnet management
void TopologyGenerator::addSubnetConfig(const std::string& name, const SubnetConfig& config) {
    subnetConfigs[name] = config;
}

SubnetConfig TopologyGenerator::getSubnetConfig(const std::string& name) {
    if (subnetConfigs.find(name) != subnetConfigs.end()) {
        return subnetConfigs[name];
    }
    return SubnetConfig();
}

std::vector<std::string> TopologyGenerator::generateIPRange(const std::string& network, int subnetMask, int count) {
    std::vector<std::string> ips;
    std::string baseIP = network.substr(0, network.find_last_of('.'));
    
    for (int i = 1; i <= count; i++) {
        std::stringstream ss;
        ss << baseIP << "." << i;
        ips.push_back(ss.str());
    }
    
    return ips;
}

// Node creation helpers
std::shared_ptr<Node> TopologyGenerator::createNodeByType(const std::string& nodeType, const std::string& ip) {
    auto node = std::make_shared<Node>(ip, LINUX);
    
    if (nodeType == "webserver") {
        node->createWebServer();
    } else if (nodeType == "fileserver") {
        node->createFileServer();
    } else if (nodeType == "workstation") {
        node->createWorkstation();
    } else if (nodeType == "domain_controller") {
        node->createDomainController();
    } else {
        node->createWorkstation(); // Default
    }
    
    return node;
}

std::vector<std::shared_ptr<Node>> TopologyGenerator::createNodesForTopology(const NetworkTemplate& templ) {
    std::vector<std::shared_ptr<Node>> nodes;
    
    for (const auto& nodeType : templ.nodeTypes) {
        std::string ip = nodeType.first;
        std::string type = nodeType.second;
        nodes.push_back(createNodeByType(type, ip));
    }
    
    return nodes;
}

// Utility methods
std::string TopologyGenerator::generateRandomIP(const std::string& network, int subnetMask) {
    std::uniform_int_distribution<> dis(1, 254);
    std::string baseIP = network.substr(0, network.find_last_of('.'));
    std::stringstream ss;
    ss << baseIP << "." << dis(gen);
    return ss.str();
}

std::string TopologyGenerator::getNextAvailableIP(const std::string& network, int subnetMask, 
                                                 const std::unordered_set<std::string>& usedIPs) {
    std::string baseIP = network.substr(0, network.find_last_of('.'));
    
    for (int i = 1; i <= 254; i++) {
        std::stringstream ss;
        ss << baseIP << "." << i;
        std::string ip = ss.str();
        
        if (usedIPs.find(ip) == usedIPs.end()) {
            return ip;
        }
    }
    
    return "";
}

bool TopologyGenerator::isValidSubnet(const std::string& network, int subnetMask) {
    if (subnetMask < 0 || subnetMask > 32) {
        return false;
    }
    
    // Simple validation - check if it's a valid IP format
    std::stringstream ss(network);
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

std::string TopologyGenerator::calculateBroadcastAddress(const std::string& network, int subnetMask) {
    // Simple broadcast calculation (assuming /24)
    size_t lastDot = network.find_last_of('.');
    if (lastDot != std::string::npos) {
        return network.substr(0, lastDot) + ".255";
    }
    return network;
}

// Private helper methods
void TopologyGenerator::initializeDefaultTemplates() {
    // Corporate network template
    NetworkTemplate corporate;
    corporate.name = "corporate";
    corporate.description = "Standard corporate network with DMZ, internal, and management segments";
    corporate.type = CORPORATE_NETWORK;
    corporate.nodeCount = 15;
    corporate.subnets = {"10.0.1.0/24", "10.0.2.0/24", "10.0.3.0/24"};
    corporate.segments = {DMZ_SEGMENT, INTERNAL_SEGMENT, MANAGEMENT_SEGMENT};
    templates.push_back(corporate);
    
    // Data center template
    NetworkTemplate datacenter;
    datacenter.name = "datacenter";
    datacenter.description = "Multi-tier data center with web, app, database, and storage tiers";
    datacenter.type = DATA_CENTER;
    datacenter.nodeCount = 15;
    datacenter.subnets = {"10.1.1.0/24", "10.1.2.0/24", "10.1.3.0/24", "10.1.4.0/24"};
    datacenter.segments = {DMZ_SEGMENT, INTERNAL_SEGMENT, DATABASE_SEGMENT, STORAGE_SEGMENT};
    templates.push_back(datacenter);
    
    // Home network template
    NetworkTemplate home;
    home.name = "home";
    home.description = "Simple home network with router and connected devices";
    home.type = HOME_NETWORK;
    home.nodeCount = 6;
    home.subnets = {"192.168.1.0/24"};
    home.segments = {INTERNAL_SEGMENT};
    templates.push_back(home);
}

void TopologyGenerator::initializeSubnetConfigs() {
    // Default subnet configurations
    SubnetConfig dmz("10.0.1.0", 24);
    dmz.gateway = "10.0.1.1";
    dmz.dnsServers = {"8.8.8.8", "8.8.4.4"};
    dmz.description = "DMZ subnet for public-facing services";
    subnetConfigs["dmz"] = dmz;
    
    SubnetConfig internal("10.0.2.0", 24);
    internal.gateway = "10.0.2.1";
    internal.dnsServers = {"10.0.3.10"};
    internal.description = "Internal subnet for workstations and servers";
    subnetConfigs["internal"] = internal;
    
    SubnetConfig management("10.0.3.0", 24);
    management.gateway = "10.0.3.1";
    management.dnsServers = {"10.0.3.10"};
    management.description = "Management subnet for administrative access";
    subnetConfigs["management"] = management;
}
