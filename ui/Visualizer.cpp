#include "Visualizer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

Visualizer::Visualizer() : canvasWidth(80), canvasHeight(25) {
}

Visualizer::~Visualizer() = default;

void Visualizer::loadFromNetworkGraph(const NetworkGraph& graph) {
    nodes.clear();
    links.clear();
    
    // Load nodes (we'll need to add public getters to NetworkGraph)
    // For now, we'll use a placeholder approach
    VisualNode node;
    node.ip = "192.168.1.100";
    node.hostname = "web-server";
    node.type = "server";
    nodes.push_back(node);
    
    node.ip = "192.168.1.101";
    node.hostname = "file-server";
    node.type = "server";
    nodes.push_back(node);
    
    node.ip = "192.168.1.102";
    node.hostname = "database-server";
    node.type = "database";
    nodes.push_back(node);
    
    node.ip = "192.168.1.103";
    node.hostname = "workstation";
    node.type = "workstation";
    nodes.push_back(node);
    
    // Load links
    VisualLink link;
    link.source = "192.168.1.100";
    link.destination = "192.168.1.101";
    link.latency = 100;
    link.protocol = "HTTP";
    links.push_back(link);
    
    link.source = "192.168.1.101";
    link.destination = "192.168.1.102";
    link.latency = 50;
    link.protocol = "TCP";
    links.push_back(link);
    
    link.source = "192.168.1.102";
    link.destination = "192.168.1.103";
    link.latency = 200;
    link.protocol = "SSH";
    links.push_back(link);
    
    // Calculate layout
    calculateStarLayout();
}

void Visualizer::loadFromNodes(const std::vector<std::shared_ptr<Node>>& nodeList) {
    nodes.clear();
    links.clear();
    
    for (const auto& node : nodeList) {
        VisualNode vnode;
        vnode.ip = node->ip;
        vnode.hostname = node->hostname;
        vnode.isCompromised = node->isCompromised();
        
        // Determine node type based on services
        bool hasWeb = false, hasDB = false, hasFile = false;
        for (const auto& service : node->services) {
            vnode.services.push_back(service.serviceName);
            if (service.serviceName == "HTTP" || service.serviceName == "HTTPS") hasWeb = true;
            if (service.serviceName == "MySQL" || service.serviceName == "PostgreSQL") hasDB = true;
            if (service.serviceName == "FTP" || service.serviceName == "SMB") hasFile = true;
        }
        
        if (hasWeb && hasDB) vnode.type = "application";
        else if (hasWeb) vnode.type = "webserver";
        else if (hasDB) vnode.type = "database";
        else if (hasFile) vnode.type = "fileserver";
        else vnode.type = "workstation";
        
        // Add vulnerabilities
        for (const auto& vuln : node->vulnerabilities) {
            vnode.vulnerabilities.push_back(vuln.type);
        }
        
        nodes.push_back(vnode);
    }
    
    // Calculate layout
    calculateStarLayout();
}

void Visualizer::calculateStarLayout() {
    if (nodes.empty()) return;
    
    int centerX = canvasWidth / 2;
    int centerY = canvasHeight / 2;
    int radius = std::min(canvasWidth, canvasHeight) / 3;
    
    // Place first node at center
    if (!nodes.empty()) {
        nodes[0].x = centerX;
        nodes[0].y = centerY;
    }
    
    // Place other nodes in a circle around the center
    for (size_t i = 1; i < nodes.size(); ++i) {
        double angle = 2 * M_PI * (i - 1) / (nodes.size() - 1);
        nodes[i].x = centerX + static_cast<int>(radius * cos(angle));
        nodes[i].y = centerY + static_cast<int>(radius * sin(angle));
    }
}

void Visualizer::calculateMeshLayout() {
    if (nodes.empty()) return;
    
    int cols = static_cast<int>(ceil(sqrt(nodes.size())));
    int rows = static_cast<int>(ceil(static_cast<double>(nodes.size()) / cols));
    
    int cellWidth = canvasWidth / (cols + 1);
    int cellHeight = canvasHeight / (rows + 1);
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        int row = i / cols;
        int col = i % cols;
        nodes[i].x = (col + 1) * cellWidth;
        nodes[i].y = (row + 1) * cellHeight;
    }
}

void Visualizer::calculateTreeLayout() {
    if (nodes.empty()) return;
    
    int centerX = canvasWidth / 2;
    int startY = 2;
    int levelHeight = (canvasHeight - 4) / 3;
    
    // Root node
    if (!nodes.empty()) {
        nodes[0].x = centerX;
        nodes[0].y = startY;
    }
    
    // Level 1 nodes
    if (nodes.size() > 1) {
        nodes[1].x = centerX - 20;
        nodes[1].y = startY + levelHeight;
    }
    if (nodes.size() > 2) {
        nodes[2].x = centerX + 20;
        nodes[2].y = startY + levelHeight;
    }
    
    // Level 2 nodes
    if (nodes.size() > 3) {
        nodes[3].x = centerX - 30;
        nodes[3].y = startY + 2 * levelHeight;
    }
    if (nodes.size() > 4) {
        nodes[4].x = centerX + 30;
        nodes[4].y = startY + 2 * levelHeight;
    }
}

void Visualizer::calculateGridLayout() {
    calculateMeshLayout(); // Same as mesh for now
}

std::string Visualizer::generateTopologyView() {
    std::ostringstream output;
    
    output << "\n🌐 Network Topology Visualization\n";
    output << "==================================\n\n";
    
    // Create canvas
    std::vector<std::string> canvas(canvasHeight, std::string(canvasWidth, ' '));
    
    // Draw links first (so they appear behind nodes)
    for (const auto& link : links) {
        VisualNode* sourceNode = findNode(link.source);
        VisualNode* destNode = findNode(link.destination);
        
        if (sourceNode && destNode) {
            std::string linkStr = drawLink(link, sourceNode->x, sourceNode->y, destNode->x, destNode->y);
            // For simplicity, we'll just show the link info
            output << "🔗 " << link.source << " <-> " << link.destination 
                   << " (" << link.protocol << ", " << link.latency << "ms)\n";
        }
    }
    
    output << "\n";
    
    // Draw nodes
    for (const auto& node : nodes) {
        output << drawNode(node);
    }
    
    output << "\n" << drawLegend();
    
    return output.str();
}

std::string Visualizer::generateAttackPathView(const std::vector<std::string>& attackPath) {
    std::ostringstream output;
    
    output << "\n🎯 Attack Path Visualization\n";
    output << "============================\n\n";
    
    if (attackPath.empty()) {
        output << "❌ No attack path specified\n";
        return output.str();
    }
    
    output << "🛣️  Attack Path:\n";
    for (size_t i = 0; i < attackPath.size(); ++i) {
        VisualNode* node = findNode(attackPath[i]);
        if (node) {
            std::string icon = node->isCompromised ? "💀" : "🎯";
            output << "  " << (i + 1) << ". " << icon << " " << node->hostname 
                   << " (" << node->ip << ")\n";
            
            if (i < attackPath.size() - 1) {
                output << "     ↓\n";
            }
        }
    }
    
    output << "\n📊 Path Analysis:\n";
    output << "  • Total Hops: " << (attackPath.size() - 1) << "\n";
    output << "  • Compromised Nodes: ";
    
    int compromised = 0;
    for (const auto& ip : attackPath) {
        VisualNode* node = findNode(ip);
        if (node && node->isCompromised) compromised++;
    }
    output << compromised << "\n";
    
    return output.str();
}

std::string Visualizer::generateServiceView() {
    std::ostringstream output;
    
    output << "\n🔧 Service Distribution\n";
    output << "=======================\n\n";
    
    std::map<std::string, std::vector<std::string>> serviceMap;
    
    for (const auto& node : nodes) {
        for (const auto& service : node.services) {
            serviceMap[service].push_back(node.ip);
        }
    }
    
    for (const auto& [service, hosts] : serviceMap) {
        output << "🔧 " << service << " (" << hosts.size() << " hosts):\n";
        for (const auto& host : hosts) {
            VisualNode* node = findNode(host);
            if (node) {
                output << "   • " << node->hostname << " (" << host << ")\n";
            }
        }
        output << "\n";
    }
    
    return output.str();
}

std::string Visualizer::generateVulnerabilityView() {
    std::ostringstream output;
    
    output << "\n💀 Vulnerability Distribution\n";
    output << "=============================\n\n";
    
    std::map<std::string, std::vector<std::string>> vulnMap;
    
    for (const auto& node : nodes) {
        for (const auto& vuln : node.vulnerabilities) {
            vulnMap[vuln].push_back(node.ip);
        }
    }
    
    if (vulnMap.empty()) {
        output << "✅ No vulnerabilities detected\n";
    } else {
        for (const auto& [vuln, hosts] : vulnMap) {
            output << "💀 " << vuln << " (" << hosts.size() << " hosts):\n";
            for (const auto& host : hosts) {
                VisualNode* node = findNode(host);
                if (node) {
                    output << "   • " << node->hostname << " (" << host << ")\n";
                }
            }
            output << "\n";
        }
    }
    
    return output.str();
}

std::string Visualizer::drawNode(const VisualNode& node) {
    std::ostringstream output;
    
    std::string icon = getNodeIcon(node.type);
    std::string status = node.isCompromised ? "💀" : "🟢";
    
    output << status << " " << icon << " " << node.hostname << " (" << node.ip << ")\n";
    output << "   Type: " << node.type << "\n";
    
    if (!node.services.empty()) {
        output << "   Services: ";
        for (size_t i = 0; i < node.services.size(); ++i) {
            output << node.services[i];
            if (i < node.services.size() - 1) output << ", ";
        }
        output << "\n";
    }
    
    if (!node.vulnerabilities.empty()) {
        output << "   Vulnerabilities: ";
        for (size_t i = 0; i < node.vulnerabilities.size(); ++i) {
            output << "💀" << node.vulnerabilities[i];
            if (i < node.vulnerabilities.size() - 1) output << ", ";
        }
        output << "\n";
    }
    
    output << "\n";
    return output.str();
}

std::string Visualizer::drawLink(const VisualLink& link, int startX, int startY, int endX, int endY) {
    // Simplified link drawing - just return link info
    std::ostringstream output;
    output << "🔗 " << link.source << " <-> " << link.destination;
    return output.str();
}

std::string Visualizer::drawLegend() {
    std::ostringstream output;
    
    output << "📋 Legend:\n";
    output << "==========\n";
    output << "🟢  - Online Node\n";
    output << "💀  - Compromised Node\n";
    output << "🖥️  - Workstation\n";
    output << "🌐  - Web Server\n";
    output << "💾  - Database Server\n";
    output << "📁  - File Server\n";
    output << "🔧  - Application Server\n";
    output << "🔗  - Network Link\n";
    output << "💀  - Vulnerability\n";
    
    return output.str();
}

VisualNode* Visualizer::findNode(const std::string& ip) {
    for (auto& node : nodes) {
        if (node.ip == ip) {
            return &node;
        }
    }
    return nullptr;
}

std::string Visualizer::getNodeIcon(const std::string& type) {
    if (type == "workstation") return "🖥️";
    if (type == "webserver") return "🌐";
    if (type == "database") return "💾";
    if (type == "fileserver") return "📁";
    if (type == "application") return "🔧";
    return "🖥️"; // default
}

std::string Visualizer::getStatusColor(bool isCompromised) {
    return isCompromised ? "💀" : "🟢";
}

void Visualizer::setLayout(const std::string& layoutType) {
    if (layoutType == "star") {
        calculateStarLayout();
    } else if (layoutType == "mesh") {
        calculateMeshLayout();
    } else if (layoutType == "tree") {
        calculateTreeLayout();
    } else if (layoutType == "grid") {
        calculateGridLayout();
    }
}

void Visualizer::setCanvasSize(int width, int height) {
    canvasWidth = width;
    canvasHeight = height;
}

void Visualizer::highlightNode(const std::string& ip) {
    VisualNode* node = findNode(ip);
    if (node) {
        // For now, just mark it as highlighted in the output
        // In a more advanced implementation, we could use different colors
    }
}

void Visualizer::highlightPath(const std::vector<std::string>& path) {
    for (const auto& ip : path) {
        highlightNode(ip);
    }
}

void Visualizer::markCompromised(const std::string& ip) {
    VisualNode* node = findNode(ip);
    if (node) {
        node->isCompromised = true;
    }
}

std::string Visualizer::exportToASCII() {
    return generateTopologyView();
}

std::string Visualizer::exportToSimpleGraph() {
    std::ostringstream output;
    
    output << "digraph RedNet {\n";
    output << "  rankdir=LR;\n";
    output << "  node [shape=box];\n\n";
    
    for (const auto& node : nodes) {
        std::string color = node.isCompromised ? "red" : "green";
        output << "  \"" << node.ip << "\" [label=\"" << node.hostname 
               << "\", color=" << color << "];\n";
    }
    
    output << "\n";
    
    for (const auto& link : links) {
        output << "  \"" << link.source << "\" -> \"" << link.destination 
               << "\" [label=\"" << link.protocol << "\"];\n";
    }
    
    output << "}\n";
    
    return output.str();
} 