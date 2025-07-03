#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../network/TopologyGenerator.h"

// Configuration sections
enum ConfigSection {
    NETWORK_CONFIG,
    EXPLOIT_CONFIG,
    SECURITY_CONFIG,
    AI_CONFIG,
    VISUALIZATION_CONFIG,
    GENERAL_CONFIG
};

// Security level
enum SecurityLevel {
    LOW_SECURITY,      // Easy targets, no detection
    MEDIUM_SECURITY,   // Some protection, basic detection
    HIGH_SECURITY,     // Strong protection, advanced detection
    MAXIMUM_SECURITY   // Military grade, AI detection
};

// AI difficulty level
enum AIDifficulty {
    NOVICE_AI,         // Basic decision making
    INTERMEDIATE_AI,   // Improved tactics
    ADVANCED_AI,       // Sophisticated strategies
    EXPERT_AI          // Elite level intelligence
};

// Network Profile Configuration
struct NetworkProfile {
    std::string name;
    std::string description;
    TopologyType type;
    int nodeCount;
    int linkCount;
    double vulnerabilityDensity;  // 0.0 to 1.0
    std::vector<std::string> commonServices;
    std::vector<std::string> commonVulnerabilities;
    std::map<std::string, double> serviceProbabilities;
    std::map<std::string, double> vulnerabilityProbabilities;
    
    NetworkProfile() : type(STAR_TOPOLOGY), nodeCount(5), linkCount(4), vulnerabilityDensity(0.3) {}
};

// Exploit Configuration
struct ExploitConfig {
    double defaultSuccessRate;
    double difficultyMultiplier;
    double timingMultiplier;
    bool enableChaining;
    bool enableAdaptation;
    std::map<std::string, double> exploitSuccessRates;
    std::map<std::string, int> exploitTimeouts;
    std::vector<std::string> enabledExploits;
    std::vector<std::string> disabledExploits;
    
    ExploitConfig() : defaultSuccessRate(0.5), difficultyMultiplier(1.0), 
                     timingMultiplier(1.0), enableChaining(true), enableAdaptation(true) {}
};

// Security Configuration
struct SecurityConfig {
    SecurityLevel level;
    bool enableDetection;
    bool enableResponse;
    double detectionProbability;
    double responseTime;
    std::vector<std::string> detectionMethods;
    std::vector<std::string> responseActions;
    std::map<std::string, double> methodEffectiveness;
    
    SecurityConfig() : level(MEDIUM_SECURITY), enableDetection(true), enableResponse(true),
                      detectionProbability(0.3), responseTime(5.0) {}
};

// AI Configuration
struct AIConfig {
    AIDifficulty difficulty;
    int maxAgents;
    bool enableLearning;
    bool enableAdaptation;
    double intelligenceLevel;
    double aggressionLevel;
    double stealthLevel;
    std::vector<std::string> enabledAgentTypes;
    std::map<std::string, double> agentCapabilities;
    
    AIConfig() : difficulty(INTERMEDIATE_AI), maxAgents(5), enableLearning(true), 
                enableAdaptation(true), intelligenceLevel(0.7), aggressionLevel(0.5), 
                stealthLevel(0.6) {}
};

// Visualization Configuration
struct VisualizationConfig {
    bool enableRealTime;
    bool enable3D;
    bool enableAnimations;
    int updateInterval;
    std::string colorScheme;
    bool showAttackPaths;
    bool showVulnerabilities;
    bool showTraffic;
    std::map<std::string, std::string> nodeColors;
    std::map<std::string, std::string> linkColors;
    
    VisualizationConfig() : enableRealTime(true), enable3D(false), enableAnimations(true),
                           updateInterval(100), colorScheme("default"), showAttackPaths(true),
                           showVulnerabilities(true), showTraffic(true) {}
};

// General Configuration
struct GeneralConfig {
    std::string logLevel;
    std::string logFile;
    bool enableLogging;
    int maxLogSize;
    std::string dataDirectory;
    bool autoSave;
    int autoSaveInterval;
    std::vector<std::string> enabledFeatures;
    
    GeneralConfig() : logLevel("INFO"), logFile("rednet.log"), enableLogging(true),
                     maxLogSize(100), dataDirectory("./data"), autoSave(true), 
                     autoSaveInterval(300) {}
};

// Scenario Template
struct ScenarioTemplate {
    std::string name;
    std::string description;
    std::string difficulty;
    NetworkProfile networkProfile;
    ExploitConfig exploitConfig;
    SecurityConfig securityConfig;
    AIConfig aiConfig;
    std::vector<std::string> objectives;
    std::vector<std::string> constraints;
    std::map<std::string, std::string> customSettings;
    
    ScenarioTemplate() {}
};

// Main Configuration Manager
class ConfigManager {
private:
    std::string configFile;
    std::map<std::string, NetworkProfile> networkProfiles;
    std::map<std::string, ScenarioTemplate> scenarioTemplates;
    
    ExploitConfig exploitConfig;
    SecurityConfig securityConfig;
    AIConfig aiConfig;
    VisualizationConfig visualizationConfig;
    GeneralConfig generalConfig;
    
    bool configLoaded;
    
public:
    ConfigManager();
    ConfigManager(std::string configFilePath);
    ~ConfigManager() = default;
    
    // Configuration loading/saving
    bool loadConfig(std::string filePath = "");
    bool saveConfig(std::string filePath = "");
    bool loadDefaultConfig();
    bool exportConfig(std::string filePath);
    bool importConfig(std::string filePath);
    
    // Network profiles
    void addNetworkProfile(const NetworkProfile& profile);
    void removeNetworkProfile(std::string name);
    NetworkProfile getNetworkProfile(std::string name);
    std::vector<std::string> getNetworkProfileNames();
    void setDefaultNetworkProfile(std::string name);
    
    // Scenario templates
    void addScenarioTemplate(const ScenarioTemplate& scenario);
    void removeScenarioTemplate(std::string name);
    ScenarioTemplate getScenarioTemplate(std::string name);
    std::vector<std::string> getScenarioTemplateNames();
    void loadScenario(std::string scenarioName);
    
    // Configuration getters
    ExploitConfig getExploitConfig() const { return exploitConfig; }
    SecurityConfig getSecurityConfig() const { return securityConfig; }
    AIConfig getAIConfig() const { return aiConfig; }
    VisualizationConfig getVisualizationConfig() const { return visualizationConfig; }
    GeneralConfig getGeneralConfig() const { return generalConfig; }
    
    // Configuration setters
    void setExploitConfig(const ExploitConfig& config) { exploitConfig = config; }
    void setSecurityConfig(const SecurityConfig& config) { securityConfig = config; }
    void setAIConfig(const AIConfig& config) { aiConfig = config; }
    void setVisualizationConfig(const VisualizationConfig& config) { visualizationConfig = config; }
    void setGeneralConfig(const GeneralConfig& config) { generalConfig = config; }
    
    // Utility methods
    bool isConfigLoaded() const { return configLoaded; }
    std::string getConfigFile() const { return configFile; }
    void resetToDefaults();
    void validateConfig();
    
    // Environment variables
    void loadFromEnvironment();
    void setEnvironmentVariable(std::string key, std::string value);
    std::string getEnvironmentVariable(std::string key);
    
private:
    bool parseConfigFile(std::ifstream& file);
    bool writeConfigFile(std::ofstream& file);
    void initializeDefaultProfiles();
    void initializeDefaultScenarios();
    std::string configSectionToString(ConfigSection section);
    ConfigSection stringToConfigSection(std::string section);
};

// Configuration Builder for easy setup
class ConfigBuilder {
private:
    ConfigManager config;
    
public:
    ConfigBuilder();
    
    // Network profile builder
    ConfigBuilder& withNetworkProfile(std::string name, TopologyType type, int nodes, int links);
    ConfigBuilder& withService(std::string service, double probability);
    ConfigBuilder& withVulnerability(std::string vuln, double probability);
    ConfigBuilder& withVulnerabilityDensity(double density);
    
    // Exploit configuration builder
    ConfigBuilder& withExploitSuccessRate(double rate);
    ConfigBuilder& withExploitChaining(bool enable);
    ConfigBuilder& withExploitAdaptation(bool enable);
    ConfigBuilder& withEnabledExploit(std::string exploit);
    ConfigBuilder& withDisabledExploit(std::string exploit);
    
    // Security configuration builder
    ConfigBuilder& withSecurityLevel(SecurityLevel level);
    ConfigBuilder& withDetection(bool enable);
    ConfigBuilder& withDetectionProbability(double prob);
    ConfigBuilder& withResponseTime(double time);
    
    // AI configuration builder
    ConfigBuilder& withAIDifficulty(AIDifficulty difficulty);
    ConfigBuilder& withMaxAgents(int max);
    ConfigBuilder& withIntelligenceLevel(double level);
    ConfigBuilder& withAggressionLevel(double level);
    ConfigBuilder& withStealthLevel(double level);
    
    // Visualization configuration builder
    ConfigBuilder& withRealTimeVisualization(bool enable);
    ConfigBuilder& with3DVisualization(bool enable);
    ConfigBuilder& withColorScheme(std::string scheme);
    ConfigBuilder& withUpdateInterval(int interval);
    
    // General configuration builder
    ConfigBuilder& withLogLevel(std::string level);
    ConfigBuilder& withAutoSave(bool enable);
    ConfigBuilder& withDataDirectory(std::string dir);
    
    // Build and save
    ConfigManager build();
    bool buildAndSave(std::string filePath);
};

// Predefined configurations
namespace DefaultConfigs {
    NetworkProfile createCorporateNetwork();
    NetworkProfile createDataCenter();
    NetworkProfile createHomeNetwork();
    NetworkProfile createStarNetwork();
    NetworkProfile createMeshNetwork();
    
    ScenarioTemplate createBeginnerScenario();
    ScenarioTemplate createIntermediateScenario();
    ScenarioTemplate createAdvancedScenario();
    ScenarioTemplate createExpertScenario();
    ScenarioTemplate createCustomScenario(std::string name, std::string description);
}

#endif // CONFIG_H 