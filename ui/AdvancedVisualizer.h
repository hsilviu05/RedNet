#ifndef ADVANCED_VISUALIZER_H
#define ADVANCED_VISUALIZER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include "../engine/Node.h"
#include "../engine/Exploit.h"
#include "../network/NetworkGraph.h"
#include "../ai/AIAgentManager.h"
#include "../utils/Config.h"

// Visualization types
enum VisualizationType {
    ASCII_2D,           // Simple ASCII representation
    COLOR_2D,           // Colored ASCII with ANSI codes
    GRAPHICAL_2D,       // Simple graphical (if available)
    INTERACTIVE_2D,     // Interactive 2D interface
    SIMPLE_3D,          // Basic 3D representation
    ADVANCED_3D         // Full 3D with rotation/zoom
};

// Color schemes
enum ColorScheme {
    DEFAULT_COLORS,
    DARK_THEME,
    LIGHT_THEME,
    HIGH_CONTRAST,
    COLORBLIND_FRIENDLY,
    CUSTOM_COLORS
};

// Animation types
enum AnimationType {
    NO_ANIMATION,
    FADE_IN,
    SLIDE_IN,
    PULSE,
    FLASH,
    TRAIL_EFFECT
};

// Node states for visualization
enum NodeVisualState {
    NORMAL_NODE,
    DISCOVERED_NODE,
    SCANNED_NODE,
    VULNERABLE_NODE,
    COMPROMISED_NODE,
    TARGET_NODE,
    ATTACK_SOURCE_NODE,
    DETECTED_NODE
};

// Link states for visualization
enum LinkVisualState {
    NORMAL_LINK,
    ACTIVE_LINK,
    ATTACK_PATH_LINK,
    COMPROMISED_LINK,
    BLOCKED_LINK,
    MONITORED_LINK
};

// Attack visualization data
struct AttackVisual {
    std::string attackId;
    std::string sourceNode;
    std::string targetNode;
    std::string exploitType;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool isActive;
    bool wasSuccessful;
    std::vector<std::string> path;
    std::map<std::string, std::string> metadata;
};

// Node visual properties
struct NodeVisual {
    std::string id;
    std::string label;
    std::string ip;
    NodeVisualState state;
    std::vector<std::string> services;
    std::vector<std::string> vulnerabilities;
    std::map<std::string, std::string> properties;
    double x, y, z;  // 3D coordinates
    std::string color;
    std::string shape;
    bool isVisible;
    bool isHighlighted;
    AnimationType animation;
};

// Link visual properties
struct LinkVisual {
    std::string id;
    std::string sourceNode;
    std::string targetNode;
    LinkVisualState state;
    std::string protocol;
    int bandwidth;
    double latency;
    std::string color;
    int thickness;
    bool isVisible;
    bool isHighlighted;
    AnimationType animation;
};

// Visualization configuration
struct VisualConfig {
    VisualizationType type;
    ColorScheme colorScheme;
    bool enableRealTime;
    bool enableAnimations;
    bool enable3D;
    bool showAttackPaths;
    bool showVulnerabilities;
    bool showTraffic;
    bool showServices;
    bool showUsers;
    bool showFiles;
    int updateInterval;
    int animationSpeed;
    std::string customColors;
    bool enableInteractivity;
    bool enableZoom;
    bool enablePan;
    bool enableRotation;
    
    VisualConfig() : type(COLOR_2D), colorScheme(DEFAULT_COLORS), enableRealTime(true),
                    enableAnimations(true), enable3D(false), showAttackPaths(true),
                    showVulnerabilities(true), showTraffic(true), showServices(true),
                    showUsers(false), showFiles(false), updateInterval(100),
                    animationSpeed(1), enableInteractivity(true), enableZoom(true),
                    enablePan(true), enableRotation(false) {}
};

// Base Visualizer class
class BaseVisualizer {
protected:
    std::shared_ptr<NetworkGraph> network;
    std::shared_ptr<ExploitDatabase> exploitDB;
    std::shared_ptr<AIAgentManager> aiManager;
    VisualConfig config;
    
    std::map<std::string, NodeVisual> nodeVisuals;
    std::map<std::string, LinkVisual> linkVisuals;
    std::vector<AttackVisual> activeAttacks;
    
    std::mutex visualMutex;
    bool isRunning;
    std::thread visualThread;
    
public:
    BaseVisualizer();
    BaseVisualizer(std::shared_ptr<NetworkGraph> net, std::shared_ptr<ExploitDatabase> db);
    virtual ~BaseVisualizer();
    
    // Core visualization methods
    virtual void initialize();
    virtual void start();
    virtual void stop();
    virtual void update();
    virtual void render();
    virtual void clear();
    
    // Configuration
    void setConfig(const VisualConfig& cfg) { config = cfg; }
    VisualConfig getConfig() const { return config; }
    void setNetwork(std::shared_ptr<NetworkGraph> net) { network = net; }
    void setExploitDB(std::shared_ptr<ExploitDatabase> db) { exploitDB = db; }
    void setAIManager(std::shared_ptr<AIAgentManager> manager) { aiManager = manager; }
    
    // Node and link management
    virtual void addNode(std::shared_ptr<Node> node);
    virtual void removeNode(std::string nodeId);
    virtual void updateNode(std::shared_ptr<Node> node);
    virtual void addLink(std::string source, std::string target, std::string protocol = "TCP");
    virtual void removeLink(std::string linkId);
    virtual void updateLink(std::string linkId, LinkVisualState state);
    
    // Attack visualization
    virtual void startAttack(const AttackVisual& attack);
    virtual void updateAttack(const AttackVisual& attack);
    virtual void endAttack(std::string attackId);
    virtual void highlightAttackPath(const std::vector<std::string>& path);
    virtual void clearAttackHighlights();
    
    // State management
    virtual void setNodeState(std::string nodeId, NodeVisualState state);
    virtual void setLinkState(std::string linkId, LinkVisualState state);
    virtual void highlightNode(std::string nodeId, bool highlight = true);
    virtual void highlightLink(std::string linkId, bool highlight = true);
    
    // Utility methods
    virtual std::string getNodeColor(NodeVisualState state);
    virtual std::string getLinkColor(LinkVisualState state);
    virtual std::string getNodeShape(NodeVisualState state);
    virtual void applyColorScheme(ColorScheme scheme);
};

// ASCII 2D Visualizer
class ASCII2DVisualizer : public BaseVisualizer {
private:
    int width, height;
    std::vector<std::vector<char>> displayBuffer;
    std::vector<std::vector<std::string>> colorBuffer;
    
public:
    ASCII2DVisualizer();
    ASCII2DVisualizer(int w, int h);
    
    void initialize() override;
    void render() override;
    void clear() override;
    
    void setDimensions(int w, int h);
    void drawNode(const NodeVisual& node);
    void drawLink(const LinkVisual& link);
    void drawAttackPath(const std::vector<std::string>& path);
    void drawLegend();
    void drawStatusBar();
    
private:
    void clearBuffer();
    void drawToBuffer(int x, int y, char c, std::string color = "");
    bool isValidPosition(int x, int y);
};

// Color 2D Visualizer (Enhanced ASCII with colors)
class Color2DVisualizer : public ASCII2DVisualizer {
private:
    std::map<std::string, std::string> colorMap;
    bool enableColors;
    
public:
    Color2DVisualizer();
    Color2DVisualizer(int w, int h);
    
    void initialize() override;
    void render() override;
    
    void setColorMap(const std::map<std::string, std::string>& colors);
    void enableColorOutput(bool enable) { enableColors = enable; }
    std::string getColorCode(std::string colorName);
    
private:
    void initializeColorMap();
    void drawColoredNode(const NodeVisual& node);
    void drawColoredLink(const LinkVisual& link);
    void drawColoredText(int x, int y, const std::string& text, std::string color);
};

// Interactive 2D Visualizer
class Interactive2DVisualizer : public Color2DVisualizer {
private:
    bool interactiveMode;
    std::string selectedNode;
    std::string selectedLink;
    std::vector<std::string> commandHistory;
    
public:
    Interactive2DVisualizer();
    
    void initialize() override;
    void render() override;
    void handleInput();
    
    void enterInteractiveMode();
    void exitInteractiveMode();
    void selectNode(std::string nodeId);
    void selectLink(std::string linkId);
    void showNodeDetails(std::string nodeId);
    void showLinkDetails(std::string linkId);
    void executeCommand(std::string command);
    
private:
    void drawInteractiveUI();
    void drawNodeDetails(const NodeVisual& node);
    void drawLinkDetails(const LinkVisual& link);
    void drawCommandHelp();
};

// 3D Visualizer (Basic)
class Basic3DVisualizer : public BaseVisualizer {
private:
    std::vector<std::vector<std::vector<char>>> display3D;
    int depth, width, height;
    double rotationX, rotationY, rotationZ;
    
public:
    Basic3DVisualizer();
    Basic3DVisualizer(int w, int h, int d);
    
    void initialize() override;
    void render() override;
    void clear() override;
    
    void setDimensions(int w, int h, int d);
    void setRotation(double x, double y, double z);
    void rotate(double x, double y, double z);
    void draw3DNode(const NodeVisual& node);
    void draw3DLink(const LinkVisual& link);
    void project3DTo2D();
    
private:
    void clear3DBuffer();
    void draw3DLine(int x1, int y1, int z1, int x2, int y2, int z2, char c);
    void draw3DSphere(int x, int y, int z, int radius, char c);
    std::tuple<int, int, int> rotatePoint(int x, int y, int z);
};

// Real-time Attack Visualizer
class RealTimeAttackVisualizer : public Color2DVisualizer {
private:
    std::chrono::steady_clock::time_point lastUpdate;
    std::vector<AttackVisual> attackHistory;
    std::map<std::string, std::vector<std::string>> attackPaths;
    bool showAttackTrails;
    int trailLength;
    
public:
    RealTimeAttackVisualizer();
    
    void initialize() override;
    void update() override;
    void render() override;
    
    void addAttackTrail(const AttackVisual& attack);
    void removeAttackTrail(std::string attackId);
    void setShowAttackTrails(bool show) { showAttackTrails = show; }
    void setTrailLength(int length) { trailLength = length; }
    
private:
    void drawAttackTrails();
    void drawAttackAnimation(const AttackVisual& attack);
    void updateAttackPositions();
    void fadeOldTrails();
};

// Vulnerability Heatmap Visualizer
class VulnerabilityHeatmapVisualizer : public Color2DVisualizer {
private:
    std::map<std::string, double> vulnerabilityScores;
    std::map<std::string, std::vector<std::string>> nodeVulnerabilities;
    bool showHeatmap;
    std::string heatmapMode; // "vulnerabilities", "services", "users"
    
public:
    VulnerabilityHeatmapVisualizer();
    
    void initialize() override;
    void render() override;
    
    void setVulnerabilityScore(std::string nodeId, double score);
    void setNodeVulnerabilities(std::string nodeId, const std::vector<std::string>& vulns);
    void setShowHeatmap(bool show) { showHeatmap = show; }
    void setHeatmapMode(std::string mode) { heatmapMode = mode; }
    
private:
    void drawHeatmap();
    std::string getHeatmapColor(double score);
    double calculateNodeScore(std::string nodeId);
    void drawHeatmapLegend();
};

// Network Topology Visualizer
class NetworkTopologyVisualizer : public Color2DVisualizer {
private:
    std::string topologyType;
    std::map<std::string, std::vector<std::string>> topologyGroups;
    bool showTopology;
    bool showGroups;
    
public:
    NetworkTopologyVisualizer();
    
    void initialize() override;
    void render() override;
    
    void setTopologyType(std::string type) { topologyType = type; }
    void setTopologyGroups(const std::map<std::string, std::vector<std::string>>& groups);
    void setShowTopology(bool show) { showTopology = show; }
    void setShowGroups(bool show) { showGroups = show; }
    
private:
    void drawTopology();
    void drawGroups();
    void drawTopologyLegend();
    void arrangeNodesByTopology();
};

// Main Advanced Visualizer that combines all features
class AdvancedVisualizer {
private:
    std::unique_ptr<BaseVisualizer> currentVisualizer;
    std::shared_ptr<NetworkGraph> network;
    std::shared_ptr<ExploitDatabase> exploitDB;
    std::shared_ptr<AIAgentManager> aiManager;
    VisualConfig config;
    
    std::map<VisualizationType, std::unique_ptr<BaseVisualizer>> visualizers;
    
public:
    AdvancedVisualizer();
    AdvancedVisualizer(std::shared_ptr<NetworkGraph> net, std::shared_ptr<ExploitDatabase> db);
    ~AdvancedVisualizer();
    
    // Core methods
    void initialize();
    void start();
    void stop();
    void update();
    void render();
    
    // Visualizer management
    void switchVisualizer(VisualizationType type);
    void setConfig(const VisualConfig& cfg);
    VisualConfig getConfig() const { return config; }
    
    // Network integration
    void setNetwork(std::shared_ptr<NetworkGraph> net);
    void setExploitDB(std::shared_ptr<ExploitDatabase> db);
    void setAIManager(std::shared_ptr<AIAgentManager> manager);
    
    // Attack visualization
    void visualizeAttack(const AttackVisual& attack);
    void visualizeExploit(const ExploitResult& result);
    void visualizeAgentAction(const AgentDecision& decision);
    void visualizeNetworkDiscovery(const std::vector<std::shared_ptr<Node>>& nodes);
    
    // Interactive features
    void enableInteractivity(bool enable);
    void setColorScheme(ColorScheme scheme);
    void setAnimationSpeed(int speed);
    void toggleFeature(std::string feature);
    
    // Export/Import
    void exportVisualization(std::string filePath);
    void importVisualization(std::string filePath);
    void saveScreenshot(std::string filePath);
    
private:
    void initializeVisualizers();
    void updateAllVisualizers();
    BaseVisualizer* getCurrentVisualizer();
};

#endif // ADVANCED_VISUALIZER_H 