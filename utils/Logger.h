#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <map>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

enum class LogCategory {
    SYSTEM,
    AGENT,
    NETWORK,
    MISSION,
    SECURITY,
    EXPLOIT,
    SCAN,
    CONFIG
};

struct LogMessage {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    LogCategory category;
    std::string source;      // Source component (e.g., "Agent_1", "Scanner", "System")
    std::string action;      // Action performed (e.g., "Create", "Scan", "Exploit")
    std::string target;      // Target of action (e.g., "192.168.1.10", "Mission_1")
    std::string message;     // Detailed message
    std::map<std::string, std::string> metadata; // Additional context
};

class Logger {
public:
    using LogCallback = std::function<void(const LogMessage&)>;
    
    static Logger& getInstance();
    
    // Core logging methods
    void log(LogLevel level, LogCategory category, const std::string& source,
             const std::string& action, const std::string& target, 
             const std::string& message, const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void debug(LogCategory category, const std::string& source, const std::string& message);
    void info(LogCategory category, const std::string& source, const std::string& message);
    void warning(LogCategory category, const std::string& source, const std::string& message);
    void error(LogCategory category, const std::string& source, const std::string& message);
    void critical(LogCategory category, const std::string& source, const std::string& message);
    
    // Specialized logging methods for different components
    void logAgentAction(const std::string& agentName, const std::string& action, 
                       const std::string& target, const std::string& result);
    void logNetworkEvent(const std::string& event, const std::string& nodeIp, 
                        const std::string& details);
    void logMissionEvent(const std::string& missionId, const std::string& event, 
                        const std::string& details);
    void logSecurityEvent(const std::string& event, const std::string& source, 
                         const std::string& target, LogLevel severity = LogLevel::WARNING);
    void logExploitAttempt(const std::string& agentName, const std::string& targetIp, 
                          const std::string& exploitName, bool success);
    void logScanResult(const std::string& scannerName, const std::string& targetIp, 
                      const std::string& result);
    
    // Log management
    std::vector<LogMessage> getRecentLogs(size_t count = 100) const;
    std::vector<LogMessage> getLogsByCategory(LogCategory category, size_t count = 100) const;
    std::vector<LogMessage> getLogsByLevel(LogLevel level, size_t count = 100) const;
    std::vector<LogMessage> searchLogs(const std::string& query, size_t count = 100) const;
    
    // Real-time callbacks
    void addLogCallback(const LogCallback& callback);
    void removeLogCallback(const LogCallback& callback);
    
    // Configuration
    void setLogLevel(LogLevel minLevel);
    void setMaxLogEntries(size_t maxEntries);
    void enableCategory(LogCategory category, bool enable = true);
    
    // Utility methods
    static std::string levelToString(LogLevel level);
    static std::string categoryToString(LogCategory category);
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    
    // Statistics
    size_t getLogCount() const;
    size_t getLogCountByLevel(LogLevel level) const;
    size_t getLogCountByCategory(LogCategory category) const;
    
private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    mutable std::mutex logMutex;
    std::vector<LogMessage> logs;
    std::vector<LogCallback> callbacks;
    
    LogLevel minLogLevel = LogLevel::DEBUG;
    size_t maxLogEntries = 1000;
    std::map<LogCategory, bool> enabledCategories;
    
    void notifyCallbacks(const LogMessage& message);
    void cleanupOldLogs();
};

// Convenience macros
#define LOG_DEBUG(category, source, message) \
    Logger::getInstance().debug(category, source, message)

#define LOG_INFO(category, source, message) \
    Logger::getInstance().info(category, source, message)

#define LOG_WARNING(category, source, message) \
    Logger::getInstance().warning(category, source, message)

#define LOG_ERROR(category, source, message) \
    Logger::getInstance().error(category, source, message)

#define LOG_CRITICAL(category, source, message) \
    Logger::getInstance().critical(category, source, message)

#define LOG_AGENT_ACTION(agent, action, target, result) \
    Logger::getInstance().logAgentAction(agent, action, target, result)

#define LOG_NETWORK_EVENT(event, nodeIp, details) \
    Logger::getInstance().logNetworkEvent(event, nodeIp, details)

#define LOG_MISSION_EVENT(missionId, event, details) \
    Logger::getInstance().logMissionEvent(missionId, event, details)

#define LOG_SECURITY_EVENT(event, source, target, severity) \
    Logger::getInstance().logSecurityEvent(event, source, target, severity)

#define LOG_EXPLOIT_ATTEMPT(agent, targetIp, exploit, success) \
    Logger::getInstance().logExploitAttempt(agent, targetIp, exploit, success)

#define LOG_SCAN_RESULT(scanner, targetIp, result) \
    Logger::getInstance().logScanResult(scanner, targetIp, result) 