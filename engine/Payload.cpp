#include "Payload.h"
#include <algorithm>
#include <thread>
#include <regex>

// Base Payload class implementation
Payload::Payload(std::string name, std::string description, PayloadType type, int complexity, double detectionProb, ExecutionLevel requiredLevel)
    : name(name), description(description), type(type), complexity(complexity), detectionProbability(detectionProb), requiredLevel(requiredLevel) {
}

void Payload::addRequiredTool(std::string tool) {
    requiredTools.push_back(tool);
}

void Payload::addDependency(std::string dependency) {
    dependencies.push_back(dependency);
}

bool Payload::requiresLevel(ExecutionLevel level) const {
    return requiredLevel <= level;
}

// ShellPayload implementation
ShellPayload::ShellPayload() 
    : Payload("Shell Access", "Command execution and shell access", SHELL_PAYLOAD, 2, 0.3, USER_LEVEL), 
      maxCommandLength(1024), interactiveMode(false) {
    
    // Add allowed commands
    allowedCommands = {
        "ls", "pwd", "cd", "cat", "grep", "find", "whoami", "id", "uname", "ps",
        "netstat", "ifconfig", "ip", "route", "df", "du", "head", "tail", "wc",
        "echo", "date", "uptime", "free", "top", "kill", "chmod", "chown"
    };
    
    // Add blocked commands
    blockedCommands = {
        "rm", "shutdown", "reboot", "init", "halt", "poweroff", "dd", "mkfs",
        "fdisk", "mount", "umount", "iptables", "firewall-cmd", "systemctl"
    };
}

bool ShellPayload::canExecute(Node& target) {
    // Can execute if target is compromised
    return target.isCompromised();
}

PayloadResult ShellPayload::execute(Node& target) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    result.message = "Shell payload activated on " + target.ip;
    result.executionLevel = USER_LEVEL;
    
    // Simulate shell initialization
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    result.success = true;
    result.output = "Shell access established. Type 'help' for available commands.";
    
    return result;
}

PayloadResult ShellPayload::executeCommand(Node& target, std::string command) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!isCommandAllowed(command)) {
        result.success = false;
        result.errorMessage = "Command not allowed: " + command;
        return result;
    }
    
    if (command.length() > maxCommandLength) {
        result.success = false;
        result.errorMessage = "Command too long";
        return result;
    }
    
    std::string output = simulateCommandOutput(command, target);
    result.commandsExecuted.push_back(command);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    result.success = true;
    result.output = output;
    
    return result;
}

std::string ShellPayload::simulateCommandOutput(std::string command, Node& target) {
    std::string baseCommand = command.substr(0, command.find(' '));
    
    if (baseCommand == "ls") {
        std::string output = "total 4\n";
        for (const auto& file : target.fileSystem) {
            output += "-rw-r--r-- 1 user user 1024 " + file.first + "\n";
        }
        return output;
    }
    else if (baseCommand == "pwd") {
        return "/home/user\n";
    }
    else if (baseCommand == "whoami") {
        return "user\n";
    }
    else if (baseCommand == "id") {
        return "uid=1000(user) gid=1000(user) groups=1000(user)\n";
    }
    else if (baseCommand == "uname") {
        return "Linux\n";
    }
    else if (baseCommand == "cat") {
        std::string filename = command.substr(4);
        if (target.fileExists(filename)) {
            return target.readFile(filename);
        }
        return "cat: " + filename + ": No such file or directory\n";
    }
    else if (baseCommand == "ps") {
        return "  PID TTY          TIME CMD\n 1234 pts/0    00:00:00 bash\n 1235 pts/0    00:00:00 ps\n";
    }
    else if (baseCommand == "netstat") {
        return "Active Internet connections (w/o servers)\nProto Recv-Q Send-Q Local Address           Foreign Address         State\n";
    }
    else if (baseCommand == "ifconfig") {
        return "eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500\n        inet " + target.ip + "  netmask 255.255.255.0  broadcast 192.168.1.255\n";
    }
    else {
        return "Command executed: " + command + "\n";
    }
}

bool ShellPayload::isCommandAllowed(std::string command) {
    std::string baseCommand = command.substr(0, command.find(' '));
    
    // Check blocked commands
    for (const std::string& blocked : blockedCommands) {
        if (baseCommand == blocked) {
            return false;
        }
    }
    
    // Check allowed commands
    for (const std::string& allowed : allowedCommands) {
        if (baseCommand == allowed) {
            return true;
        }
    }
    
    return false;
}

void ShellPayload::addAllowedCommand(std::string command) {
    allowedCommands.push_back(command);
}

void ShellPayload::addBlockedCommand(std::string command) {
    blockedCommands.push_back(command);
}

void ShellPayload::setInteractiveMode(bool interactive) {
    interactiveMode = interactive;
}

void ShellPayload::setMaxCommandLength(int length) {
    maxCommandLength = length;
}

// DataExfiltrationPayload implementation
DataExfiltrationPayload::DataExfiltrationPayload()
    : Payload("Data Exfiltration", "Extract sensitive data from target", DATA_EXFILTRATION, 4, 0.6, USER_LEVEL),
      maxFileSize(1048576), compressData(false) {
    
    // Add common target files
    targetFiles = {
        "/etc/passwd", "/etc/shadow", "/etc/hosts", "/etc/resolv.conf",
        "/home/user/.bash_history", "/home/user/.ssh/id_rsa"
    };
    
    // Add target directories
    targetDirectories = {
        "/home/user/documents", "/home/user/desktop", "/var/log",
        "/etc", "/tmp"
    };
    
    // Add file extensions
    fileExtensions = {".txt", ".conf", ".log", ".key", ".pem", ".db", ".sql"};
}

bool DataExfiltrationPayload::canExecute(Node& target) {
    return target.isCompromised();
}

PayloadResult DataExfiltrationPayload::execute(Node& target) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    result.message = "Starting data exfiltration from " + target.ip;
    
    // Exfiltrate target files
    for (const std::string& file : targetFiles) {
        if (target.fileExists(file)) {
            std::string content = readFileContent(target, file);
            if (!content.empty()) {
                result.filesAccessed.push_back(file);
                result.dataCollected.push_back(content);
            }
        }
    }
    
    // Find and exfiltrate files by extension
    for (const std::string& dir : targetDirectories) {
        for (const std::string& ext : fileExtensions) {
            std::vector<std::string> files = findFiles(target, dir, ext);
            for (const std::string& file : files) {
                std::string content = readFileContent(target, file);
                if (!content.empty()) {
                    result.filesAccessed.push_back(file);
                    result.dataCollected.push_back(content);
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    if (!result.filesAccessed.empty()) {
        result.success = true;
        result.output = "Successfully exfiltrated " + std::to_string(result.filesAccessed.size()) + " files";
    } else {
        result.success = false;
        result.errorMessage = "No files found for exfiltration";
    }
    
    return result;
}

std::string DataExfiltrationPayload::readFileContent(Node& target, std::string filePath) {
    if (target.fileExists(filePath)) {
        std::string content = target.readFile(filePath);
        if (content.length() > maxFileSize) {
            content = content.substr(0, maxFileSize) + "\n[TRUNCATED]";
        }
        return content;
    }
    return "";
}

std::vector<std::string> DataExfiltrationPayload::findFiles(Node& target, std::string directory, std::string extension) {
    std::vector<std::string> foundFiles;
    
    for (const auto& file : target.fileSystem) {
        if (file.first.find(directory) == 0 && file.first.find(extension) != std::string::npos) {
            foundFiles.push_back(file.first);
        }
    }
    
    return foundFiles;
}

void DataExfiltrationPayload::addTargetFile(std::string filePath) {
    targetFiles.push_back(filePath);
}

void DataExfiltrationPayload::addTargetDirectory(std::string directory) {
    targetDirectories.push_back(directory);
}

void DataExfiltrationPayload::addFileExtension(std::string extension) {
    fileExtensions.push_back(extension);
}

void DataExfiltrationPayload::setMaxFileSize(int size) {
    maxFileSize = size;
}

void DataExfiltrationPayload::setCompressData(bool compress) {
    compressData = compress;
}

// PersistencePayload implementation
PersistencePayload::PersistencePayload()
    : Payload("Persistence", "Install backdoor for persistent access", PERSISTENCE, 6, 0.7, ADMIN_LEVEL),
      stealthMode(true) {
    
    persistenceMethods = {
        "cron_job", "startup_script", "ssh_key", "hidden_service"
    };
    
    backdoorCode = "#!/bin/bash\nwhile true; do nc -l -p 4444 -e /bin/bash; done";
    
    cleanupCommands = {
        "rm -f /tmp/.backdoor",
        "crontab -r",
        "rm -f ~/.ssh/authorized_keys"
    };
}

bool PersistencePayload::canExecute(Node& target) {
    return target.isCompromised();
}

PayloadResult PersistencePayload::execute(Node& target) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    result.message = "Installing persistence mechanisms on " + target.ip;
    
    bool success = false;
    
    // Try different persistence methods
    if (installCronJob(target)) {
        result.commandsExecuted.push_back("cron_job_installed");
        success = true;
    }
    
    if (installSSHKey(target)) {
        result.commandsExecuted.push_back("ssh_key_installed");
        success = true;
    }
    
    if (createHiddenService(target)) {
        result.commandsExecuted.push_back("hidden_service_created");
        success = true;
    }
    
    if (stealthMode) {
        cleanupEvidence(target);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    if (success) {
        result.success = true;
        result.output = "Persistence mechanisms installed successfully";
    } else {
        result.success = false;
        result.errorMessage = "Failed to install persistence mechanisms";
    }
    
    return result;
}

bool PersistencePayload::installCronJob(Node& target) {
    // Simulate cron job installation
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return true;
}

bool PersistencePayload::installSSHKey(Node& target) {
    // Simulate SSH key installation
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return true;
}

bool PersistencePayload::createHiddenService(Node& target) {
    // Simulate hidden service creation
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    return true;
}

void PersistencePayload::cleanupEvidence(Node& target) {
    // Simulate evidence cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void PersistencePayload::addPersistenceMethod(std::string method) {
    persistenceMethods.push_back(method);
}

void PersistencePayload::setBackdoorCode(std::string code) {
    backdoorCode = code;
}

void PersistencePayload::setStealthMode(bool stealth) {
    stealthMode = stealth;
}

void PersistencePayload::addCleanupCommand(std::string command) {
    cleanupCommands.push_back(command);
}

// ReconnaissancePayload implementation
ReconnaissancePayload::ReconnaissancePayload()
    : Payload("Reconnaissance", "Gather system and network information", RECONNAISSANCE, 3, 0.2, USER_LEVEL),
      deepScan(false) {
    
    reconModules = {
        "system_info", "network_info", "user_info", "service_info", "filesystem_info"
    };
}

bool ReconnaissancePayload::canExecute(Node& target) {
    return target.isCompromised();
}

PayloadResult ReconnaissancePayload::execute(Node& target) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    result.message = "Starting reconnaissance on " + target.ip;
    
    std::string reconData;
    
    // Gather different types of information
    reconData += gatherSystemInfo(target);
    reconData += gatherNetworkInfo(target);
    reconData += gatherUserInfo(target);
    reconData += gatherServiceInfo(target);
    
    if (deepScan) {
        reconData += gatherFileSystemInfo(target);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    result.success = true;
    result.output = reconData;
    result.dataCollected.push_back(reconData);
    
    return result;
}

std::string ReconnaissancePayload::gatherSystemInfo(Node& target) {
    std::string info = "=== SYSTEM INFORMATION ===\n";
    info += "Hostname: " + target.hostname + "\n";
    info += "IP Address: " + target.ip + "\n";
    info += "OS: " + (target.os == LINUX ? "Linux" : target.os == WINDOWS ? "Windows" : "Unknown") + "\n";
    info += "Services: " + std::to_string(target.services.size()) + "\n";
    info += "Users: " + std::to_string(target.users.size()) + "\n";
    info += "Files: " + std::to_string(target.fileSystem.size()) + "\n";
    return info;
}

std::string ReconnaissancePayload::gatherNetworkInfo(Node& target) {
    std::string info = "=== NETWORK INFORMATION ===\n";
    info += "IP: " + target.ip + "\n";
    info += "Open Ports: ";
    for (int port : target.openPorts) {
        info += std::to_string(port) + " ";
    }
    info += "\n";
    return info;
}

std::string ReconnaissancePayload::gatherUserInfo(Node& target) {
    std::string info = "=== USER INFORMATION ===\n";
    for (const auto& user : target.users) {
        info += "User: " + user.first + "\n";
    }
    return info;
}

std::string ReconnaissancePayload::gatherServiceInfo(Node& target) {
    std::string info = "=== SERVICE INFORMATION ===\n";
    for (const Service& service : target.services) {
        info += "Service: " + service.serviceName + " (Port: " + std::to_string(service.port) + ")\n";
    }
    return info;
}

std::string ReconnaissancePayload::gatherFileSystemInfo(Node& target) {
    std::string info = "=== FILESYSTEM INFORMATION ===\n";
    for (const auto& file : target.fileSystem) {
        info += "File: " + file.first + "\n";
    }
    return info;
}

void ReconnaissancePayload::addReconModule(std::string module) {
    reconModules.push_back(module);
}

void ReconnaissancePayload::setDeepScan(bool deep) {
    deepScan = deep;
}

void ReconnaissancePayload::addTargetSystem(std::string system) {
    targetSystems.push_back(system);
}

// LateralMovementPayload implementation
LateralMovementPayload::LateralMovementPayload()
    : Payload("Lateral Movement", "Move laterally through the network", LATERAL_MOVEMENT, 7, 0.8, USER_LEVEL),
      autoPropagate(false), maxHops(3) {
    
    movementMethods = {
        "ssh_propagation", "smb_propagation", "rdp_propagation", "service_exploitation"
    };
}

bool LateralMovementPayload::canExecute(Node& target) {
    return target.isCompromised();
}

PayloadResult LateralMovementPayload::execute(Node& target) {
    PayloadResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    result.message = "Starting lateral movement from " + target.ip;
    
    // Discover network neighbors
    std::vector<std::string> neighbors = discoverNetworkNeighbors(target);
    discoveredTargets = neighbors;
    
    if (autoPropagate) {
        for (const std::string& neighbor : neighbors) {
            if (propagateViaSSH(target, neighbor)) {
                result.commandsExecuted.push_back("ssh_propagated_to_" + neighbor);
            }
            if (propagateViaSMB(target, neighbor)) {
                result.commandsExecuted.push_back("smb_propagated_to_" + neighbor);
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    result.success = true;
    result.output = "Discovered " + std::to_string(neighbors.size()) + " potential targets";
    result.dataCollected = neighbors;
    
    return result;
}

std::vector<std::string> LateralMovementPayload::discoverNetworkNeighbors(Node& target) {
    std::vector<std::string> neighbors;
    
    // Simulate network discovery
    std::string baseIP = target.ip.substr(0, target.ip.find_last_of('.'));
    for (int i = 1; i <= 10; i++) {
        std::string neighborIP = baseIP + "." + std::to_string(i);
        if (neighborIP != target.ip) {
            neighbors.push_back(neighborIP);
        }
    }
    
    return neighbors;
}

bool LateralMovementPayload::propagateViaSSH(Node& target, std::string neighbor) {
    // Simulate SSH propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return (rand() % 100) < 30; // 30% success rate
}

bool LateralMovementPayload::propagateViaSMB(Node& target, std::string neighbor) {
    // Simulate SMB propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    return (rand() % 100) < 40; // 40% success rate
}

bool LateralMovementPayload::propagateViaRDP(Node& target, std::string neighbor) {
    // Simulate RDP propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    return (rand() % 100) < 25; // 25% success rate
}

void LateralMovementPayload::addMovementMethod(std::string method) {
    movementMethods.push_back(method);
}

void LateralMovementPayload::setAutoPropagate(bool autoProp) {
    autoPropagate = autoProp;
}

void LateralMovementPayload::setMaxHops(int hops) {
    maxHops = hops;
}

std::vector<std::string> LateralMovementPayload::getDiscoveredTargets() const {
    return discoveredTargets;
}

// PayloadChain implementation
PayloadChain::PayloadChain(std::string name) : name(name), chainSuccessProbability(1.0) {
}

PayloadChain::~PayloadChain() {
    // Don't delete payloads as they're managed by PayloadDatabase
}

void PayloadChain::addPayload(Payload* payload) {
    payloads.push_back(payload);
    // Recalculate chain success probability
    chainSuccessProbability *= (1.0 - payload->getDetectionProbability());
}

PayloadResult PayloadChain::executeChain(Node& target) {
    PayloadResult result;
    result.message = "Executing payload chain: " + name;
    
    for (Payload* payload : payloads) {
        if (!payload->canExecute(target)) {
            result.success = false;
            result.errorMessage = "Payload " + payload->getName() + " cannot be executed on target";
            return result;
        }
        
        PayloadResult payloadResult = payload->execute(target);
        if (!payloadResult.success) {
            result.success = false;
            result.errorMessage = "Chain failed at payload: " + payload->getName() + " - " + payloadResult.errorMessage;
            return result;
        }
        
        // Update result with successful payload
        result.filesAccessed.insert(result.filesAccessed.end(), payloadResult.filesAccessed.begin(), payloadResult.filesAccessed.end());
        result.commandsExecuted.insert(result.commandsExecuted.end(), payloadResult.commandsExecuted.begin(), payloadResult.commandsExecuted.end());
        result.dataCollected.insert(result.dataCollected.end(), payloadResult.dataCollected.begin(), payloadResult.dataCollected.end());
        result.timeTaken += payloadResult.timeTaken;
    }
    
    result.success = true;
    result.message = "Payload chain completed successfully";
    return result;
}

double PayloadChain::getChainSuccessProbability() const {
    return chainSuccessProbability;
}

// PayloadDatabase implementation
PayloadDatabase::PayloadDatabase() {
    loadDefaultPayloads();
}

PayloadDatabase::~PayloadDatabase() {
    clearPayloads();
}

void PayloadDatabase::addPayload(Payload* payload) {
    payloads.push_back(payload);
    payloadsByType[payload->getType()].push_back(payload);
    payloadsByLevel[payload->getRequiredLevel()].push_back(payload);
}

std::vector<Payload*> PayloadDatabase::getPayloadsForNode(Node& target) {
    std::vector<Payload*> applicablePayloads;
    
    for (Payload* payload : payloads) {
        if (payload->canExecute(target)) {
            applicablePayloads.push_back(payload);
        }
    }
    
    return applicablePayloads;
}

std::vector<Payload*> PayloadDatabase::getPayloadsByType(PayloadType type) {
    if (payloadsByType.find(type) != payloadsByType.end()) {
        return payloadsByType[type];
    }
    return std::vector<Payload*>();
}

std::vector<Payload*> PayloadDatabase::getPayloadsByLevel(ExecutionLevel level) {
    if (payloadsByLevel.find(level) != payloadsByLevel.end()) {
        return payloadsByLevel[level];
    }
    return std::vector<Payload*>();
}

std::vector<Payload*> PayloadDatabase::getAllPayloads() const {
    return payloads;
}

Payload* PayloadDatabase::findPayloadByName(std::string name) {
    for (Payload* payload : payloads) {
        if (payload->getName() == name) {
            return payload;
        }
    }
    return nullptr;
}

void PayloadDatabase::loadDefaultPayloads() {
    // Add shell payloads
    addPayload(new ShellPayload());
    
    // Add data exfiltration payloads
    addPayload(new DataExfiltrationPayload());
    
    // Add persistence payloads
    addPayload(new PersistencePayload());
    
    // Add reconnaissance payloads
    addPayload(new ReconnaissancePayload());
    
    // Add lateral movement payloads
    addPayload(new LateralMovementPayload());
}

void PayloadDatabase::clearPayloads() {
    for (Payload* payload : payloads) {
        delete payload;
    }
    payloads.clear();
    payloadsByType.clear();
    payloadsByLevel.clear();
}
