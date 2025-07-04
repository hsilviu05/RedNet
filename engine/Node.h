#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

enum class NodeType {
    WORKSTATION,
    SERVER,
    ROUTER,
    SWITCH,
    FIREWALL,
    DATABASE,
    WEB_SERVER,
    MAIL_SERVER,
    DNS_SERVER,
    FILE_SERVER
};

enum class SecurityLevel {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum class UserRole {
    ADMIN,
    USER,
    GUEST,
    SERVICE
};

enum class FileSensitivity {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

enum OS {
    LINUX,
    WINDOWS
};

constexpr int USER_ACCESS = 1;
constexpr int ADMIN_ACCESS = 2;
constexpr int ROOT_ACCESS = 3;

struct Service {
    std::string serviceName;
    int port;
    bool isRunning;
    
    Service();
    Service(const std::string& name, int servicePort, bool running);
    std::string toJson() const;
};

struct User {
    std::string username;
    std::string password;
    UserRole role;
    
    User();
    User(const std::string& name, const std::string& pass, UserRole userRole);
    std::string getRoleName() const;
    std::string toJson() const;
};

struct File {
    std::string filePath;
    FileSensitivity sensitivity;
    
    File();
    File(const std::string& path, FileSensitivity sens);
    std::string getSensitivityName() const;
    std::string toJson() const;
};

class Node {
private:
    int id;
    std::string ip;
    std::string hostname;
    NodeType type;
    bool isCompromised;
    bool isActive;
    SecurityLevel securityLevel;
    std::vector<Service> services;
    std::vector<User> users;
    std::vector<File> files;

public:
    Node();
    Node(int nodeId, const std::string& ip, const std::string& hostname, NodeType type);
    Node(const std::string& ip, OS os);
    Node(const std::string& ip, OS os, const std::string& hostname);
    ~Node();

    int getId() const;
    const std::string& getIp() const;
    const std::string& getHostname() const;
    NodeType getType() const;
    bool isNodeCompromised() const;
    bool isNodeActive() const;
    SecurityLevel getSecurityLevel() const;
    
    const std::vector<Service>& getServices() const;
    const std::vector<User>& getUsers() const;
    const std::vector<File>& getFiles() const;

    void addService(const Service& service);
    void addUser(const User& user);
    void addFile(const File& file);
    
    void markCompromised();
    void setSecurityLevel(SecurityLevel level);
    void setActive(bool active);
    
    bool hasService(const std::string& serviceName) const;
    bool hasUser(const std::string& username) const;
    bool authenticateUser(const std::string& username, const std::string& password) const;
    
    std::string getTypeName() const;
    std::string getSecurityLevelName() const;
    std::vector<Service> getRunningServices() const;
    std::vector<File> getSensitiveFiles() const;
    
    void generateDefaultServices();
    void generateDefaultUsers();
    void generateDefaultFiles();
    
    void createWebServer();
    void createWorkstation();
    void createDomainController();
    void setVerbose(bool verbose);
    void createFileServer();
    
    std::string toJson() const;
};

#endif // NODE_H 