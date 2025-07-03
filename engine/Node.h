#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <deque>
#include <list>

enum OperatingSystem {
    LINUX,
    WINDOWS,
    MACOS,
    UNKNOWN
};

struct Service
{
    int port;
    std::string serviceName;
    std::string version;
    std::string settings;
    bool isRunning;
    
    Service() : port(0), isRunning(false) {}
    Service(int p, std::string name) : port(p), serviceName(name), isRunning(true) {}
};

struct Vulnerability
{
    std::string type;
    std::string difficulty;
    std::string service;
    int port;
    int probability;
    std::string tools;
};

class Node {
public:
    Node();
    Node(std::string ip, OperatingSystem os);
    ~Node();
    
    // Node templates
    void createWebServer();
    void createFileServer();
    void createWorkstation();
    void createDomainController();
    
    // File system methods
    void addFile(std::string path, std::string content);
    std::string readFile(std::string path);
    bool fileExists(std::string path);
    
    // User management
    void addUser(std::string username, std::string password);
    bool authenticateUser(std::string username, std::string password);
    
    // Public member variables
    std::string ip;
    int port;
    std::string service;
    OperatingSystem os;
    std::vector<Service> services;
    std::vector<Vulnerability> vulnerabilities;
    std::vector<int> openPorts;
    std::string hostname;
    std::map<std::string, std::string> fileSystem;
    std::map<std::string, std::string> users; // username -> password hash

private:
    bool compromised;
    
    void addVulnerability(Vulnerability vuln);
    bool isServiceRunning(int port);
    std::vector<int> getOpenPorts();
    OperatingSystem getOS();
    bool hasVulnerability(std::string vulnType);
    void addService(Service service);
    void removeService(int port);
    Service getService(int port);
    std::vector<Service> getAllServices();
    void setHostname(std::string name);
    std::string getHostname();
    bool isCompromised();
    void markCompromised();
};


