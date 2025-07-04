#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// ConfigManager implementation
ConfigManager::ConfigManager() : configLoaded(false) {
    configFile = "rednet.conf";
    initializeDefaultProfiles();
    initializeDefaultScenarios();
}

ConfigManager::ConfigManager(std::string configFilePath) : configFile(configFilePath), configLoaded(false) {
    initializeDefaultProfiles();
    initializeDefaultScenarios();
    loadConfig(configFilePath);
}

bool ConfigManager::loadConfig(std::string filePath) {
    if (filePath.empty()) {
        filePath = configFile;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "⚠️  Config file not found: " << filePath << std::endl;
        std::cout << "📝 Loading default configuration..." << std::endl;
        loadDefaultConfig();
        return false;
    }
    
    bool success = parseConfigFile(file);
    file.close();
    
    if (success) {
        configLoaded = true;
        std::cout << "✅ Configuration loaded from: " << filePath << std::endl;
    } else {
        std::cout << "❌ Failed to parse config file: " << filePath << std::endl;
        loadDefaultConfig();
    }
    
    return success;
}

bool ConfigManager::saveConfig(std::string filePath) {
    if (filePath.empty()) {
        filePath = configFile;
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cout << "❌ Cannot create config file: " << filePath << std::endl;
        return false;
    }
    
    bool success = writeConfigFile(file);
    file.close();
    
    if (success) {
        std::cout << "✅ Configuration saved to: " << filePath << std::endl;
    } else {
        std::cout << "❌ Failed to write config file: " << filePath << std::endl;
    }
    
    return success;
}

bool ConfigManager::loadDefaultConfig() {
    // Set default values
    exploitConfig = ExploitConfig();
    securityConfig = SecurityConfig();
    aiConfig = AIConfig();
    visualizationConfig = VisualizationConfig();
    generalConfig = GeneralConfig();
    
    configLoaded = true;
    std::cout << "✅ Default configuration loaded" << std::endl;
    return true;
}

bool ConfigManager::exportConfig(std::string filePath) {
    return saveConfig(filePath);
}

bool ConfigManager::importConfig(std::string filePath) {
    return loadConfig(filePath);
}

void ConfigManager::addNetworkProfile(const NetworkProfile& profile) {
    networkProfiles[profile.name] = profile;
    std::cout << "📊 Added network profile: " << profile.name << std::endl;
}

void ConfigManager::removeNetworkProfile(std::string name) {
    if (networkProfiles.erase(name) > 0) {
        std::cout << "🗑️  Removed network profile: " << name << std::endl;
    }
}

NetworkProfile ConfigManager::getNetworkProfile(std::string name) {
    auto it = networkProfiles.find(name);
    if (it != networkProfiles.end()) {
        return it->second;
    }
    
    // Return default profile if not found
    NetworkProfile defaultProfile;
    defaultProfile.name = "default";
    defaultProfile.description = "Default network profile";
    return defaultProfile;
}

std::vector<std::string> ConfigManager::getNetworkProfileNames() {
    std::vector<std::string> names;
    for (const auto& pair : networkProfiles) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::setDefaultNetworkProfile(std::string name) {
    auto it = networkProfiles.find(name);
    if (it != networkProfiles.end()) {
        std::cout << "📊 Set default network profile: " << name << std::endl;
    } else {
        std::cout << "❌ Network profile not found: " << name << std::endl;
    }
}

void ConfigManager::addScenarioTemplate(const ScenarioTemplate& scenario) {
    scenarioTemplates[scenario.name] = scenario;
    std::cout << "🎯 Added scenario template: " << scenario.name << std::endl;
}

void ConfigManager::removeScenarioTemplate(std::string name) {
    if (scenarioTemplates.erase(name) > 0) {
        std::cout << "🗑️  Removed scenario template: " << name << std::endl;
    }
}

ScenarioTemplate ConfigManager::getScenarioTemplate(std::string name) {
    auto it = scenarioTemplates.find(name);
    if (it != scenarioTemplates.end()) {
        return it->second;
    }
    
    // Return default scenario if not found
    ScenarioTemplate defaultScenario;
    defaultScenario.name = "default";
    defaultScenario.description = "Default scenario";
    return defaultScenario;
}

std::vector<std::string> ConfigManager::getScenarioTemplateNames() {
    std::vector<std::string> names;
    for (const auto& pair : scenarioTemplates) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::loadScenario(std::string scenarioName) {
    auto scenario = getScenarioTemplate(scenarioName);
    
    // Apply scenario configuration
    exploitConfig = scenario.exploitConfig;
    securityConfig = scenario.securityConfig;
    aiConfig = scenario.aiConfig;
    
    std::cout << "🎯 Loaded scenario: " << scenarioName << std::endl;
    std::cout << "  📝 " << scenario.description << std::endl;
    std::cout << "  🎯 Difficulty: " << scenario.difficulty << std::endl;
}

void ConfigManager::resetToDefaults() {
    loadDefaultConfig();
    std::cout << "🔄 Configuration reset to defaults" << std::endl;
}

void ConfigManager::validateConfig() {
    std::cout << "🔍 Validating configuration..." << std::endl;
    
    // Validate exploit config
    if (exploitConfig.defaultSuccessRate < 0.0 || exploitConfig.defaultSuccessRate > 1.0) {
        std::cout << "⚠️  Invalid exploit success rate, resetting to 0.5" << std::endl;
        exploitConfig.defaultSuccessRate = 0.5;
    }
    
    // Validate AI config
    if (aiConfig.maxAgents < 1) {
        std::cout << "⚠️  Invalid max agents, resetting to 5" << std::endl;
        aiConfig.maxAgents = 5;
    }
    
    // Validate visualization config
    if (visualizationConfig.updateInterval < 10) {
        std::cout << "⚠️  Invalid update interval, resetting to 100" << std::endl;
        visualizationConfig.updateInterval = 100;
    }
    
    std::cout << "✅ Configuration validation complete" << std::endl;
}

void ConfigManager::loadFromEnvironment() {
    // Load configuration from environment variables
    std::cout << "🌍 Loading configuration from environment variables..." << std::endl;
    
    // Example environment variables
    const char* logLevel = std::getenv("REDNET_LOG_LEVEL");
    if (logLevel) {
        generalConfig.logLevel = logLevel;
    }
    
    const char* dataDir = std::getenv("REDNET_DATA_DIR");
    if (dataDir) {
        generalConfig.dataDirectory = dataDir;
    }
    
    const char* maxAgents = std::getenv("REDNET_MAX_AGENTS");
    if (maxAgents) {
        aiConfig.maxAgents = std::stoi(maxAgents);
    }
}

void ConfigManager::setEnvironmentVariable(std::string key, std::string value) {
    // Note: This is a simplified implementation
    // In a real implementation, you would use platform-specific APIs
    std::cout << "🌍 Set environment variable: " << key << "=" << value << std::endl;
}

std::string ConfigManager::getEnvironmentVariable(std::string key) {
    const char* value = std::getenv(key.c_str());
    return value ? value : "";
}

bool ConfigManager::parseConfigFile(std::ifstream& file) {
    std::string line;
    ConfigSection currentSection = GENERAL_CONFIG;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse section headers
        if (line[0] == '[' && line[line.length()-1] == ']') {
            std::string sectionName = line.substr(1, line.length()-2);
            currentSection = stringToConfigSection(sectionName);
            continue;
        }
        
        // Parse key-value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Apply configuration based on section
            switch (currentSection) {
                case EXPLOIT_CONFIG:
                    if (key == "default_success_rate") {
                        exploitConfig.defaultSuccessRate = std::stod(value);
                    } else if (key == "enable_chaining") {
                        exploitConfig.enableChaining = (value == "true");
                    }
                    break;
                    
                case SECURITY_CONFIG:
                    if (key == "detection_probability") {
                        securityConfig.detectionProbability = std::stod(value);
                    } else if (key == "enable_detection") {
                        securityConfig.enableDetection = (value == "true");
                    }
                    break;
                    
                case AI_CONFIG:
                    if (key == "max_agents") {
                        aiConfig.maxAgents = std::stoi(value);
                    } else if (key == "intelligence_level") {
                        aiConfig.intelligenceLevel = std::stod(value);
                    }
                    break;
                    
                case VISUALIZATION_CONFIG:
                    if (key == "update_interval") {
                        visualizationConfig.updateInterval = std::stoi(value);
                    } else if (key == "enable_real_time") {
                        visualizationConfig.enableRealTime = (value == "true");
                    }
                    break;
                    
                case GENERAL_CONFIG:
                    if (key == "log_level") {
                        generalConfig.logLevel = value;
                    } else if (key == "data_directory") {
                        generalConfig.dataDirectory = value;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return true;
}

bool ConfigManager::writeConfigFile(std::ofstream& file) {
    // Write exploit configuration
    file << "[EXPLOIT_CONFIG]\n";
    file << "default_success_rate=" << exploitConfig.defaultSuccessRate << "\n";
    file << "enable_chaining=" << (exploitConfig.enableChaining ? "true" : "false") << "\n";
    file << "enable_adaptation=" << (exploitConfig.enableAdaptation ? "true" : "false") << "\n";
    file << "\n";
    
    // Write security configuration
    file << "[SECURITY_CONFIG]\n";
    file << "detection_probability=" << securityConfig.detectionProbability << "\n";
    file << "enable_detection=" << (securityConfig.enableDetection ? "true" : "false") << "\n";
    file << "enable_response=" << (securityConfig.enableResponse ? "true" : "false") << "\n";
    file << "\n";
    
    // Write AI configuration
    file << "[AI_CONFIG]\n";
    file << "max_agents=" << aiConfig.maxAgents << "\n";
    file << "intelligence_level=" << aiConfig.intelligenceLevel << "\n";
    file << "enable_learning=" << (aiConfig.enableLearning ? "true" : "false") << "\n";
    file << "\n";
    
    // Write visualization configuration
    file << "[VISUALIZATION_CONFIG]\n";
    file << "update_interval=" << visualizationConfig.updateInterval << "\n";
    file << "enable_real_time=" << (visualizationConfig.enableRealTime ? "true" : "false") << "\n";
    file << "enable_animations=" << (visualizationConfig.enableAnimations ? "true" : "false") << "\n";
    file << "\n";
    
    // Write general configuration
    file << "[GENERAL_CONFIG]\n";
    file << "log_level=" << generalConfig.logLevel << "\n";
    file << "data_directory=" << generalConfig.dataDirectory << "\n";
    file << "auto_save=" << (generalConfig.autoSave ? "true" : "false") << "\n";
    
    return true;
}

void ConfigManager::initializeDefaultProfiles() {
    // Corporate Network Profile
    NetworkProfile corporate;
    corporate.name = "corporate";
    corporate.description = "Typical corporate network with multiple departments";
    corporate.type = TopologyType::CORPORATE;
    corporate.nodeCount = 20;
    corporate.linkCount = 25;
    corporate.vulnerabilityDensity = 0.4;
    corporate.commonServices = {"HTTP", "HTTPS", "SSH", "RDP", "SMB", "FTP"};
    corporate.commonVulnerabilities = {"weak_passwords", "outdated_software", "misconfigured_services"};
    corporate.serviceProbabilities["HTTP"] = 0.8;
    corporate.serviceProbabilities["SSH"] = 0.6;
    corporate.serviceProbabilities["RDP"] = 0.4;
    networkProfiles["corporate"] = corporate;
    
    // Data Center Profile
    NetworkProfile datacenter;
    datacenter.name = "datacenter";
    datacenter.description = "High-performance data center environment";
    datacenter.type = TopologyType::DATA_CENTER;
    datacenter.nodeCount = 50;
    datacenter.linkCount = 100;
    datacenter.vulnerabilityDensity = 0.2;
    datacenter.commonServices = {"SSH", "HTTP", "HTTPS", "Database", "Storage"};
    datacenter.commonVulnerabilities = {"privilege_escalation", "service_misconfiguration"};
    datacenter.serviceProbabilities["SSH"] = 0.9;
    datacenter.serviceProbabilities["Database"] = 0.7;
    networkProfiles["datacenter"] = datacenter;
    
    // Home Network Profile
    NetworkProfile home;
    home.name = "home";
    home.description = "Small home network with basic devices";
    home.type = TopologyType::HOME;
    home.nodeCount = 5;
    home.linkCount = 4;
    home.vulnerabilityDensity = 0.6;
    home.commonServices = {"HTTP", "WiFi", "IoT"};
    home.commonVulnerabilities = {"default_passwords", "unpatched_devices", "weak_encryption"};
    home.serviceProbabilities["HTTP"] = 0.9;
    home.serviceProbabilities["WiFi"] = 0.8;
    networkProfiles["home"] = home;
}

void ConfigManager::initializeDefaultScenarios() {
    // Beginner Scenario
    ScenarioTemplate beginner;
    beginner.name = "beginner";
    beginner.description = "Simple network for learning basic concepts";
    beginner.difficulty = "easy";
    beginner.networkProfile = networkProfiles["home"];
    beginner.exploitConfig.defaultSuccessRate = 0.7;
    beginner.securityConfig.level = 0; // LOW_SECURITY
    beginner.aiConfig.difficulty = static_cast<AIDifficulty>(0); // NOVICE_AI
    beginner.objectives = {"Compromise one device", "Learn basic scanning"};
    scenarioTemplates["beginner"] = beginner;
    
    // Intermediate Scenario
    ScenarioTemplate intermediate;
    intermediate.name = "intermediate";
    intermediate.description = "Corporate network with moderate security";
    intermediate.difficulty = "medium";
    intermediate.networkProfile = networkProfiles["corporate"];
    intermediate.exploitConfig.defaultSuccessRate = 0.5;
    intermediate.securityConfig.level = 1; // MEDIUM_SECURITY
    intermediate.aiConfig.difficulty = static_cast<AIDifficulty>(1); // INTERMEDIATE_AI
    intermediate.objectives = {"Compromise multiple devices", "Perform lateral movement"};
    scenarioTemplates["intermediate"] = intermediate;
    
    // Advanced Scenario
    ScenarioTemplate advanced;
    advanced.name = "advanced";
    advanced.description = "High-security data center environment";
    advanced.difficulty = "hard";
    advanced.networkProfile = networkProfiles["datacenter"];
    advanced.exploitConfig.defaultSuccessRate = 0.3;
    advanced.securityConfig.level = 2; // HIGH_SECURITY
    advanced.aiConfig.difficulty = static_cast<AIDifficulty>(2); // ADVANCED_AI
    advanced.objectives = {"Compromise critical systems", "Evade detection"};
    scenarioTemplates["advanced"] = advanced;
}

std::string ConfigManager::configSectionToString(ConfigSection section) {
    switch (section) {
        case NETWORK_CONFIG: return "NETWORK_CONFIG";
        case EXPLOIT_CONFIG: return "EXPLOIT_CONFIG";
        case SECURITY_CONFIG: return "SECURITY_CONFIG";
        case AI_CONFIG: return "AI_CONFIG";
        case VISUALIZATION_CONFIG: return "VISUALIZATION_CONFIG";
        case GENERAL_CONFIG: return "GENERAL_CONFIG";
        default: return "UNKNOWN";
    }
}

ConfigSection ConfigManager::stringToConfigSection(std::string section) {
    if (section == "NETWORK_CONFIG") return NETWORK_CONFIG;
    if (section == "EXPLOIT_CONFIG") return EXPLOIT_CONFIG;
    if (section == "SECURITY_CONFIG") return SECURITY_CONFIG;
    if (section == "AI_CONFIG") return AI_CONFIG;
    if (section == "VISUALIZATION_CONFIG") return VISUALIZATION_CONFIG;
    if (section == "GENERAL_CONFIG") return GENERAL_CONFIG;
    return GENERAL_CONFIG;
}

// ConfigBuilder implementation
ConfigBuilder::ConfigBuilder() {
}

ConfigBuilder& ConfigBuilder::withNetworkProfile(std::string name, TopologyType type, int nodes, int links) {
    NetworkProfile profile;
    profile.name = name;
    profile.type = type;
    profile.nodeCount = nodes;
    profile.linkCount = links;
    config.addNetworkProfile(profile);
    return *this;
}

ConfigBuilder& ConfigBuilder::withService(std::string service, double probability) {
    // TODO: Add service to current profile
    return *this;
}

ConfigBuilder& ConfigBuilder::withVulnerability(std::string vuln, double probability) {
    // TODO: Add vulnerability to current profile
    return *this;
}

ConfigBuilder& ConfigBuilder::withVulnerabilityDensity(double density) {
    // TODO: Set vulnerability density for current profile
    return *this;
}

ConfigBuilder& ConfigBuilder::withExploitSuccessRate(double rate) {
    ExploitConfig exploitConfig = config.getExploitConfig();
    exploitConfig.defaultSuccessRate = rate;
    config.setExploitConfig(exploitConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withExploitChaining(bool enable) {
    ExploitConfig exploitConfig = config.getExploitConfig();
    exploitConfig.enableChaining = enable;
    config.setExploitConfig(exploitConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withExploitAdaptation(bool enable) {
    ExploitConfig exploitConfig = config.getExploitConfig();
    exploitConfig.enableAdaptation = enable;
    config.setExploitConfig(exploitConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withEnabledExploit(std::string exploit) {
    config.getExploitConfig().enabledExploits.push_back(exploit);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDisabledExploit(std::string exploit) {
    config.getExploitConfig().disabledExploits.push_back(exploit);
    return *this;
}

ConfigBuilder& ConfigBuilder::withSecurityLevel(int level) {
    SecurityConfig securityConfig = config.getSecurityConfig();
    securityConfig.level = level;
    config.setSecurityConfig(securityConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDetection(bool enable) {
    SecurityConfig securityConfig = config.getSecurityConfig();
    securityConfig.enableDetection = enable;
    config.setSecurityConfig(securityConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDetectionProbability(double prob) {
    SecurityConfig securityConfig = config.getSecurityConfig();
    securityConfig.detectionProbability = prob;
    config.setSecurityConfig(securityConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withResponseTime(double time) {
    SecurityConfig securityConfig = config.getSecurityConfig();
    securityConfig.responseTime = time;
    config.setSecurityConfig(securityConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withAIDifficulty(AIDifficulty difficulty) {
    AIConfig aiConfig = config.getAIConfig();
    aiConfig.difficulty = difficulty;
    config.setAIConfig(aiConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withMaxAgents(int max) {
    AIConfig aiConfig = config.getAIConfig();
    aiConfig.maxAgents = max;
    config.setAIConfig(aiConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withIntelligenceLevel(double level) {
    AIConfig aiConfig = config.getAIConfig();
    aiConfig.intelligenceLevel = level;
    config.setAIConfig(aiConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withAggressionLevel(double level) {
    AIConfig aiConfig = config.getAIConfig();
    aiConfig.aggressionLevel = level;
    config.setAIConfig(aiConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withStealthLevel(double level) {
    AIConfig aiConfig = config.getAIConfig();
    aiConfig.stealthLevel = level;
    config.setAIConfig(aiConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withRealTimeVisualization(bool enable) {
    VisualizationConfig visConfig = config.getVisualizationConfig();
    visConfig.enableRealTime = enable;
    config.setVisualizationConfig(visConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::with3DVisualization(bool enable) {
    VisualizationConfig visConfig = config.getVisualizationConfig();
    visConfig.enable3D = enable;
    config.setVisualizationConfig(visConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withColorScheme(std::string scheme) {
    config.getVisualizationConfig().colorScheme = scheme;
    return *this;
}

ConfigBuilder& ConfigBuilder::withUpdateInterval(int interval) {
    VisualizationConfig visConfig = config.getVisualizationConfig();
    visConfig.updateInterval = interval;
    config.setVisualizationConfig(visConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withLogLevel(std::string level) {
    config.getGeneralConfig().logLevel = level;
    return *this;
}

ConfigBuilder& ConfigBuilder::withAutoSave(bool enable) {
    GeneralConfig genConfig = config.getGeneralConfig();
    genConfig.autoSave = enable;
    config.setGeneralConfig(genConfig);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDataDirectory(std::string dir) {
    config.getGeneralConfig().dataDirectory = dir;
    return *this;
}

ConfigManager ConfigBuilder::build() {
    return config;
}

bool ConfigBuilder::buildAndSave(std::string filePath) {
    return config.saveConfig(filePath);
}

// DefaultConfigs namespace implementation
namespace DefaultConfigs {
    NetworkProfile createCorporateNetwork() {
        NetworkProfile profile;
        profile.name = "corporate";
        profile.description = "Typical corporate network";
        profile.type = TopologyType::CORPORATE;
        profile.nodeCount = 20;
        profile.linkCount = 25;
        profile.vulnerabilityDensity = 0.4;
        return profile;
    }
    
    NetworkProfile createDataCenter() {
        NetworkProfile profile;
        profile.name = "datacenter";
        profile.description = "High-performance data center";
        profile.type = TopologyType::DATA_CENTER;
        profile.nodeCount = 50;
        profile.linkCount = 100;
        profile.vulnerabilityDensity = 0.2;
        return profile;
    }
    
    NetworkProfile createHomeNetwork() {
        NetworkProfile profile;
        profile.name = "home";
        profile.description = "Small home network";
        profile.type = TopologyType::HOME;
        profile.nodeCount = 5;
        profile.linkCount = 4;
        profile.vulnerabilityDensity = 0.6;
        return profile;
    }
    
    NetworkProfile createStarNetwork() {
        NetworkProfile profile;
        profile.name = "star";
        profile.description = "Star topology network";
        profile.type = TopologyType::STAR;
        profile.nodeCount = 5;
        profile.linkCount = 4;
        profile.vulnerabilityDensity = 0.3;
        return profile;
    }
    
    NetworkProfile createMeshNetwork() {
        NetworkProfile profile;
        profile.name = "mesh";
        profile.description = "Mesh topology network";
        profile.type = TopologyType::MESH;
        profile.nodeCount = 6;
        profile.linkCount = 15;
        profile.vulnerabilityDensity = 0.4;
        return profile;
    }
    
    ScenarioTemplate createBeginnerScenario() {
        ScenarioTemplate scenario;
        scenario.name = "beginner";
        scenario.description = "Simple scenario for learning";
        scenario.difficulty = "easy";
        scenario.networkProfile = createHomeNetwork();
        scenario.exploitConfig.defaultSuccessRate = 0.7;
        scenario.securityConfig.level = 0; // LOW_SECURITY
        scenario.aiConfig.difficulty = static_cast<AIDifficulty>(0); // NOVICE_AI
        return scenario;
    }
    
    ScenarioTemplate createIntermediateScenario() {
        ScenarioTemplate scenario;
        scenario.name = "intermediate";
        scenario.description = "Moderate difficulty scenario";
        scenario.difficulty = "medium";
        scenario.networkProfile = createCorporateNetwork();
        scenario.exploitConfig.defaultSuccessRate = 0.5;
        scenario.securityConfig.level = 1; // MEDIUM_SECURITY
        scenario.aiConfig.difficulty = static_cast<AIDifficulty>(1); // INTERMEDIATE_AI
        return scenario;
    }
    
    ScenarioTemplate createAdvancedScenario() {
        ScenarioTemplate scenario;
        scenario.name = "advanced";
        scenario.description = "High difficulty scenario";
        scenario.difficulty = "hard";
        scenario.networkProfile = createDataCenter();
        scenario.exploitConfig.defaultSuccessRate = 0.3;
        scenario.securityConfig.level = 2; // HIGH_SECURITY
        scenario.aiConfig.difficulty = static_cast<AIDifficulty>(2); // ADVANCED_AI
        return scenario;
    }
    
    ScenarioTemplate createExpertScenario() {
        ScenarioTemplate scenario;
        scenario.name = "expert";
        scenario.description = "Expert level scenario";
        scenario.difficulty = "expert";
        scenario.networkProfile = createDataCenter();
        scenario.exploitConfig.defaultSuccessRate = 0.2;
        scenario.securityConfig.level = 3; // MAXIMUM_SECURITY
        scenario.aiConfig.difficulty = static_cast<AIDifficulty>(3); // EXPERT_AI
        return scenario;
    }
    
    ScenarioTemplate createCustomScenario(std::string name, std::string description) {
        ScenarioTemplate scenario;
        scenario.name = name;
        scenario.description = description;
        scenario.difficulty = "custom";
        scenario.networkProfile = createCorporateNetwork();
        scenario.exploitConfig.defaultSuccessRate = 0.5;
        scenario.securityConfig.level = 1; // MEDIUM_SECURITY
        scenario.aiConfig.difficulty = static_cast<AIDifficulty>(1); // INTERMEDIATE_AI
        return scenario;
    }
} 