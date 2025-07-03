#ifndef REDNET_VISUALIZER_H
#define REDNET_VISUALIZER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../engine/Node.h"
#include "../network/NetworkGraph.h"
#include "../network/TopologyGenerator.h"

struct VisualNode {
    std::string ip;
    std::string hostname;
    std::string type;
    bool isCompromised;
    int x, y;  // Position for ASCII layout
    std::vector<std::string> services;
    std::vector<std::string> vulnerabilities;
    
    VisualNode() : isCompromised(false), x(0), y(0) {}
};

struct VisualLink {
    std::string source;
    std::string destination;
    int latency;
    bool isActive;
    std::string protocol;
    
    VisualLink() : latency(1), isActive(true) {}
};

class Visualizer {
private:
    std::vector<VisualNode> nodes;
    std::vector<VisualLink> links;
    int canvasWidth;
    int canvasHeight;
    
    // Layout algorithms
    void calculateStarLayout();
    void calculateMeshLayout();
    void calculateTreeLayout();
    void calculateGridLayout();
    
    // Drawing helpers
    std::string drawNode(const VisualNode& node);
    std::string drawLink(const VisualLink& link, int startX, int startY, int endX, int endY);
    std::string drawLegend();
    std::string drawAttackPath(const std::vector<std::string>& path);
    
    // Utility methods
    VisualNode* findNode(const std::string& ip);
    std::string getNodeIcon(const std::string& type);
    std::string getStatusColor(bool isCompromised);
    
public:
    Visualizer();
    ~Visualizer();
    
    // Main visualization methods
    void loadFromNetworkGraph(const NetworkGraph& graph);
    void loadFromNodes(const std::vector<std::shared_ptr<Node>>& nodes);
    std::string generateTopologyView();
    std::string generateAttackPathView(const std::vector<std::string>& attackPath);
    std::string generateServiceView();
    std::string generateVulnerabilityView();
    
    // Layout options
    void setLayout(const std::string& layoutType);
    void setCanvasSize(int width, int height);
    
    // Interactive features
    void highlightNode(const std::string& ip);
    void highlightPath(const std::vector<std::string>& path);
    void markCompromised(const std::string& ip);
    
    // Export options
    std::string exportToASCII();
    std::string exportToSimpleGraph();
};

#endif // REDNET_VISUALIZER_H 