#include "Logger.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, LogCategory category, const std::string& source,
                const std::string& action, const std::string& target, 
                const std::string& message, const std::map<std::string, std::string>& metadata) {
    
    // Check if this log level is enabled
    if (level < minLogLevel) {
        return;
    }
    
    // Check if this category is enabled
    auto categoryIt = enabledCategories.find(category);
    if (categoryIt != enabledCategories.end() && !categoryIt->second) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    LogMessage logMsg;
    logMsg.timestamp = std::chrono::system_clock::now();
    logMsg.level = level;
    logMsg.category = category;
    logMsg.source = source;
    logMsg.action = action;
    logMsg.target = target;
    logMsg.message = message;
    logMsg.metadata = metadata;
    
    logs.push_back(logMsg);
    
    // Cleanup old logs if we exceed the limit
    cleanupOldLogs();
    
    // Notify callbacks
    notifyCallbacks(logMsg);
}

void Logger::debug(LogCategory category, const std::string& source, const std::string& message) {
    log(LogLevel::DEBUG, category, source, "Debug", "", message);
}

void Logger::info(LogCategory category, const std::string& source, const std::string& message) {
    log(LogLevel::INFO, category, source, "Info", "", message);
}

void Logger::warning(LogCategory category, const std::string& source, const std::string& message) {
    log(LogLevel::WARNING, category, source, "Warning", "", message);
}

void Logger::error(LogCategory category, const std::string& source, const std::string& message) {
    log(LogLevel::ERROR, category, source, "Error", "", message);
}

void Logger::critical(LogCategory category, const std::string& source, const std::string& message) {
    log(LogLevel::CRITICAL, category, source, "Critical", "", message);
}

void Logger::logAgentAction(const std::string& agentName, const std::string& action, 
                           const std::string& target, const std::string& result) {
    LogLevel level = LogLevel::INFO;
    if (result == "Error" || result == "Failed") {
        level = LogLevel::ERROR;
    } else if (result == "Warning") {
        level = LogLevel::WARNING;
    } else if (result == "Success" || result == "Completed") {
        level = LogLevel::INFO;
    }
    
    std::map<std::string, std::string> metadata;
    metadata["result"] = result;
    metadata["agent_type"] = "AI Agent";
    
    log(level, LogCategory::AGENT, agentName, action, target, 
        agentName + " " + action + " on " + target + ": " + result, metadata);
}

void Logger::logNetworkEvent(const std::string& event, const std::string& nodeIp, 
                            const std::string& details) {
    std::map<std::string, std::string> metadata;
    metadata["node_ip"] = nodeIp;
    metadata["event_type"] = "network";
    
    log(LogLevel::INFO, LogCategory::NETWORK, "NetworkManager", event, nodeIp, details, metadata);
}

void Logger::logMissionEvent(const std::string& missionId, const std::string& event, 
                            const std::string& details) {
    std::map<std::string, std::string> metadata;
    metadata["mission_id"] = missionId;
    metadata["event_type"] = "mission";
    
    log(LogLevel::INFO, LogCategory::MISSION, "MissionManager", event, missionId, details, metadata);
}

void Logger::logSecurityEvent(const std::string& event, const std::string& source, 
                             const std::string& target, LogLevel severity) {
    std::map<std::string, std::string> metadata;
    metadata["event_type"] = "security";
    metadata["severity"] = levelToString(severity);
    
    log(severity, LogCategory::SECURITY, source, event, target, 
        "Security event: " + event + " from " + source + " to " + target, metadata);
}

void Logger::logExploitAttempt(const std::string& agentName, const std::string& targetIp, 
                              const std::string& exploitName, bool success) {
    LogLevel level = success ? LogLevel::WARNING : LogLevel::INFO;
    std::string result = success ? "SUCCESS" : "FAILED";
    
    std::map<std::string, std::string> metadata;
    metadata["exploit_name"] = exploitName;
    metadata["success"] = success ? "true" : "false";
    metadata["target_ip"] = targetIp;
    
    log(level, LogCategory::EXPLOIT, agentName, "Exploit", targetIp, 
        "Exploit attempt: " + exploitName + " against " + targetIp + " - " + result, metadata);
}

void Logger::logScanResult(const std::string& scannerName, const std::string& targetIp, 
                          const std::string& result) {
    std::map<std::string, std::string> metadata;
    metadata["target_ip"] = targetIp;
    metadata["scan_type"] = "network";
    
    log(LogLevel::INFO, LogCategory::SCAN, scannerName, "Scan", targetIp, 
        "Scan result for " + targetIp + ": " + result, metadata);
}

std::vector<LogMessage> Logger::getRecentLogs(size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (logs.size() <= count) {
        return logs;
    }
    
    return std::vector<LogMessage>(logs.end() - count, logs.end());
}

std::vector<LogMessage> Logger::getLogsByCategory(LogCategory category, size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogMessage> filtered;
    for (auto it = logs.rbegin(); it != logs.rend() && filtered.size() < count; ++it) {
        if (it->category == category) {
            filtered.push_back(*it);
        }
    }
    
    std::reverse(filtered.begin(), filtered.end());
    return filtered;
}

std::vector<LogMessage> Logger::getLogsByLevel(LogLevel level, size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogMessage> filtered;
    for (auto it = logs.rbegin(); it != logs.rend() && filtered.size() < count; ++it) {
        if (it->level == level) {
            filtered.push_back(*it);
        }
    }
    
    std::reverse(filtered.begin(), filtered.end());
    return filtered;
}

std::vector<LogMessage> Logger::searchLogs(const std::string& query, size_t count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::vector<LogMessage> filtered;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (auto it = logs.rbegin(); it != logs.rend() && filtered.size() < count; ++it) {
        std::string searchText = it->source + " " + it->action + " " + it->target + " " + it->message;
        std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
        
        if (searchText.find(lowerQuery) != std::string::npos) {
            filtered.push_back(*it);
        }
    }
    
    std::reverse(filtered.begin(), filtered.end());
    return filtered;
}

void Logger::addLogCallback(const LogCallback& callback) {
    std::lock_guard<std::mutex> lock(logMutex);
    callbacks.push_back(callback);
}

void Logger::removeLogCallback(const LogCallback& callback) {
    std::lock_guard<std::mutex> lock(logMutex);
    // Note: This is a simplified removal that won't work with lambdas
    // In a real implementation, you'd need a callback ID system
}

void Logger::setLogLevel(LogLevel minLevel) {
    std::lock_guard<std::mutex> lock(logMutex);
    minLogLevel = minLevel;
}

void Logger::setMaxLogEntries(size_t maxEntries) {
    std::lock_guard<std::mutex> lock(logMutex);
    maxLogEntries = maxEntries;
    cleanupOldLogs();
}

void Logger::enableCategory(LogCategory category, bool enable) {
    std::lock_guard<std::mutex> lock(logMutex);
    enabledCategories[category] = enable;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::categoryToString(LogCategory category) {
    switch (category) {
        case LogCategory::SYSTEM: return "SYSTEM";
        case LogCategory::AGENT: return "AGENT";
        case LogCategory::NETWORK: return "NETWORK";
        case LogCategory::MISSION: return "MISSION";
        case LogCategory::SECURITY: return "SECURITY";
        case LogCategory::EXPLOIT: return "EXPLOIT";
        case LogCategory::SCAN: return "SCAN";
        case LogCategory::CONFIG: return "CONFIG";
        default: return "UNKNOWN";
    }
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

size_t Logger::getLogCount() const {
    std::lock_guard<std::mutex> lock(logMutex);
    return logs.size();
}

size_t Logger::getLogCountByLevel(LogLevel level) const {
    std::lock_guard<std::mutex> lock(logMutex);
    return std::count_if(logs.begin(), logs.end(), 
                        [level](const LogMessage& msg) { return msg.level == level; });
}

size_t Logger::getLogCountByCategory(LogCategory category) const {
    std::lock_guard<std::mutex> lock(logMutex);
    return std::count_if(logs.begin(), logs.end(), 
                        [category](const LogMessage& msg) { return msg.category == category; });
}

void Logger::notifyCallbacks(const LogMessage& message) {
    for (const auto& callback : callbacks) {
        try {
            callback(message);
        } catch (...) {
            // Ignore callback errors to prevent logging system from failing
        }
    }
}

void Logger::cleanupOldLogs() {
    if (logs.size() > maxLogEntries) {
        size_t removeCount = logs.size() - maxLogEntries;
        logs.erase(logs.begin(), logs.begin() + removeCount);
    }
} 