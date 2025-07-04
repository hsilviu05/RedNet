#include "Node.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>

Node::Node() : id(0), isCompromised(false), isActive(true), securityLevel(SecurityLevel::LOW) {
}

Node::Node(int nodeId, const std::string& ip, const std::string& hostname, NodeType type)
    : id(nodeId), ip(ip), hostname(hostname), type(type), isCompromised(false), 
      isActive(true), securityLevel(SecurityLevel::LOW) {
}

Node::Node(const std::string& ip, OS os) : id(0), ip(ip), hostname(""), type(NodeType::WORKSTATION), isCompromised(false), isActive(true), securityLevel(SecurityLevel::LOW) {}
Node::Node(const std::string& ip, OS os, const std::string& hostname) : id(0), ip(ip), hostname(hostname), type(NodeType::WORKSTATION), isCompromised(false), isActive(true), securityLevel(SecurityLevel::LOW) {}

Node::~Node() {
}

int Node::getId() const {
    return id;
}

const std::string& Node::getIp() const {
    return ip;
}

const std::string& Node::getHostname() const {
    return hostname;
}

NodeType Node::getType() const {
    return type;
}

bool Node::isNodeCompromised() const {
    return isCompromised;
}

bool Node::isNodeActive() const {
    return isActive;
}

SecurityLevel Node::getSecurityLevel() const {
    return securityLevel;
}

const std::vector<Service>& Node::getServices() const {
    return services;
}

const std::vector<User>& Node::getUsers() const {
    return users;
}

const std::vector<File>& Node::getFiles() const {
    return files;
}

void Node::addService(const Service& service) {
    services.push_back(service);
}

void Node::addUser(const User& user) {
    users.push_back(user);
}

void Node::addFile(const File& file) {
    files.push_back(file);
}

void Node::markCompromised() {
    isCompromised = true;
    std::cout << "Node " << hostname << " (" << ip << ") has been compromised!" << std::endl;
}

void Node::setSecurityLevel(SecurityLevel level) {
    securityLevel = level;
}

void Node::setActive(bool active) {
    isActive = active;
}

bool Node::hasService(const std::string& serviceName) const {
    for (const auto& service : services) {
        if (service.serviceName == serviceName && service.isRunning) {
            return true;
        }
    }
    return false;
}

bool Node::hasUser(const std::string& username) const {
    for (const auto& user : users) {
        if (user.username == username) {
            return true;
        }
    }
    return false;
}

bool Node::authenticateUser(const std::string& username, const std::string& password) const {
    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            return true;
        }
    }
    return false;
}

std::string Node::getTypeName() const {
    switch (type) {
        case NodeType::WORKSTATION: return "Workstation";
        case NodeType::SERVER: return "Server";
        case NodeType::ROUTER: return "Router";
        case NodeType::SWITCH: return "Switch";
        case NodeType::FIREWALL: return "Firewall";
        case NodeType::DATABASE: return "Database";
        case NodeType::WEB_SERVER: return "Web Server";
        case NodeType::MAIL_SERVER: return "Mail Server";
        case NodeType::DNS_SERVER: return "DNS Server";
        case NodeType::FILE_SERVER: return "File Server";
        default: return "Unknown";
    }
}

std::string Node::getSecurityLevelName() const {
    switch (securityLevel) {
        case SecurityLevel::LOW: return "Low";
        case SecurityLevel::MEDIUM: return "Medium";
        case SecurityLevel::HIGH: return "High";
        case SecurityLevel::CRITICAL: return "Critical";
        default: return "Unknown";
    }
}

std::vector<Service> Node::getRunningServices() const {
    std::vector<Service> running;
    for (const auto& service : services) {
        if (service.isRunning) {
            running.push_back(service);
        }
    }
    return running;
}

std::vector<File> Node::getSensitiveFiles() const {
    std::vector<File> sensitive;
    for (const auto& file : files) {
        if (file.sensitivity == FileSensitivity::HIGH || file.sensitivity == FileSensitivity::CRITICAL) {
            sensitive.push_back(file);
        }
    }
    return sensitive;
}

void Node::generateDefaultServices() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);
    
    // Add common services based on node type
    switch (type) {
        case NodeType::WORKSTATION:
            if (dist(gen) < 80) addService(Service("SSH", 22, true));
            if (dist(gen) < 60) addService(Service("HTTP", 80, true));
            if (dist(gen) < 40) addService(Service("FTP", 21, true));
            break;
            
        case NodeType::SERVER:
            addService(Service("SSH", 22, true));
            if (dist(gen) < 70) addService(Service("HTTP", 80, true));
            if (dist(gen) < 50) addService(Service("HTTPS", 443, true));
            if (dist(gen) < 30) addService(Service("FTP", 21, true));
            break;
            
        case NodeType::WEB_SERVER:
            addService(Service("HTTP", 80, true));
            addService(Service("HTTPS", 443, true));
            if (dist(gen) < 60) addService(Service("SSH", 22, true));
            break;
            
        case NodeType::DATABASE:
            if (dist(gen) < 70) addService(Service("MySQL", 3306, true));
            if (dist(gen) < 50) addService(Service("PostgreSQL", 5432, true));
            addService(Service("SSH", 22, true));
            break;
            
        case NodeType::MAIL_SERVER:
            addService(Service("SMTP", 25, true));
            addService(Service("POP3", 110, true));
            addService(Service("IMAP", 143, true));
            addService(Service("SSH", 22, true));
            break;
            
        case NodeType::DNS_SERVER:
            addService(Service("DNS", 53, true));
            addService(Service("SSH", 22, true));
            break;
            
        case NodeType::FILE_SERVER:
            addService(Service("FTP", 21, true));
            addService(Service("SMB", 445, true));
            addService(Service("SSH", 22, true));
            break;
            
        default:
            addService(Service("SSH", 22, true));
            break;
    }
}

void Node::generateDefaultUsers() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);
    
    // Add common users
    addUser(User("root", "root123", UserRole::ADMIN));
    addUser(User("admin", "admin123", UserRole::ADMIN));
    
    if (dist(gen) < 70) {
        addUser(User("user", "user123", UserRole::USER));
    }
    
    if (dist(gen) < 50) {
        addUser(User("guest", "guest", UserRole::GUEST));
    }
    
    if (type == NodeType::WEB_SERVER && dist(gen) < 60) {
        addUser(User("www-data", "www123", UserRole::SERVICE));
    }
    
    if (type == NodeType::DATABASE && dist(gen) < 60) {
        addUser(User("mysql", "mysql123", UserRole::SERVICE));
    }
}

void Node::generateDefaultFiles() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);
    
    // Add common system files
    addFile(File("/etc/passwd", FileSensitivity::HIGH));
    addFile(File("/etc/shadow", FileSensitivity::CRITICAL));
    addFile(File("/etc/hosts", FileSensitivity::MEDIUM));
    
    if (type == NodeType::WEB_SERVER) {
        addFile(File("/var/www/html/index.html", FileSensitivity::LOW));
        addFile(File("/var/www/html/config.php", FileSensitivity::HIGH));
        addFile(File("/var/log/apache2/access.log", FileSensitivity::MEDIUM));
    }
    
    if (type == NodeType::DATABASE) {
        addFile(File("/var/lib/mysql/", FileSensitivity::CRITICAL));
        addFile(File("/etc/mysql/my.cnf", FileSensitivity::HIGH));
    }
    
    if (type == NodeType::FILE_SERVER) {
        addFile(File("/home/shared/", FileSensitivity::MEDIUM));
        addFile(File("/var/ftp/pub/", FileSensitivity::LOW));
    }
}

std::string Node::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"id\":" << id << ",";
    json << "\"ip\":\"" << ip << "\",";
    json << "\"hostname\":\"" << hostname << "\",";
    json << "\"type\":\"" << getTypeName() << "\",";
    json << "\"securityLevel\":\"" << getSecurityLevelName() << "\",";
    json << "\"isCompromised\":" << (isCompromised ? "true" : "false") << ",";
    json << "\"isActive\":" << (isActive ? "true" : "false") << ",";
    json << "\"services\":[";
    for (size_t i = 0; i < services.size(); ++i) {
        json << services[i].toJson();
        if (i < services.size() - 1) json << ",";
    }
    json << "],\"users\":[";
    for (size_t i = 0; i < users.size(); ++i) {
        json << users[i].toJson();
        if (i < users.size() - 1) json << ",";
    }
    json << "],\"files\":[";
    for (size_t i = 0; i < files.size(); ++i) {
        json << files[i].toJson();
        if (i < files.size() - 1) json << ",";
    }
    json << "]";
    json << "}";
    return json.str();
}

void Node::createWebServer() { /* stub */ }
void Node::createWorkstation() { /* stub */ }
void Node::createDomainController() { /* stub */ }
void Node::setVerbose(bool verbose) { /* stub */ }
void Node::createFileServer() { /* stub */ }

// Service implementation
Service::Service() : port(0), isRunning(false) {
}

Service::Service(const std::string& name, int servicePort, bool running)
    : serviceName(name), port(servicePort), isRunning(running) {
}

std::string Service::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"name\":\"" << serviceName << "\",";
    json << "\"port\":" << port << ",";
    json << "\"isRunning\":" << (isRunning ? "true" : "false");
    json << "}";
    return json.str();
}

// User implementation
User::User() : role(UserRole::USER) {
}

User::User(const std::string& name, const std::string& pass, UserRole userRole)
    : username(name), password(pass), role(userRole) {
}

std::string User::getRoleName() const {
    switch (role) {
        case UserRole::ADMIN: return "Admin";
        case UserRole::USER: return "User";
        case UserRole::GUEST: return "Guest";
        case UserRole::SERVICE: return "Service";
        default: return "Unknown";
    }
}

std::string User::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"username\":\"" << username << "\",";
    json << "\"role\":\"" << getRoleName() << "\"";
    json << "}";
    return json.str();
}

// File implementation
File::File() : sensitivity(FileSensitivity::LOW) {
}

File::File(const std::string& path, FileSensitivity sens)
    : filePath(path), sensitivity(sens) {
}

std::string File::getSensitivityName() const {
    switch (sensitivity) {
        case FileSensitivity::LOW: return "Low";
        case FileSensitivity::MEDIUM: return "Medium";
        case FileSensitivity::HIGH: return "High";
        case FileSensitivity::CRITICAL: return "Critical";
        default: return "Unknown";
    }
}

std::string File::toJson() const {
    std::ostringstream json;
    json << "{";
    json << "\"path\":\"" << filePath << "\",";
    json << "\"sensitivity\":\"" << getSensitivityName() << "\"";
    json << "}";
    return json.str();
}