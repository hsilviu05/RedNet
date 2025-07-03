#include "Node.h"

Node::Node()
{
    ip = "";
    os = UNKNOWN;
    hostname = "";
    compromised = false;
}

Node::Node(std::string ip, OperatingSystem os)
{
    this->ip = ip;
    this->os = os;
    this->hostname = "";
    this->compromised = false;
}

Node::~Node()
{
    vulnerabilities.clear();
    services.clear();
    openPorts.clear();
    fileSystem.clear();
    users.clear();
}

void Node::addVulnerability(Vulnerability vuln)
{
    vulnerabilities.push_back(vuln);
}

bool Node::isServiceRunning(int port)
{
    for (Service service : services) {
        if (service.port == port && service.isRunning) {
            return true;
        }
    }
    return false;
}

std::vector<int> Node::getOpenPorts()
{
    return openPorts;
}

OperatingSystem Node::getOS()
{
    return os;
}

bool Node::hasVulnerability(std::string vulnType)
{
    for (Vulnerability vuln : vulnerabilities) {
        if (vuln.type == vulnType) {
            return true;
        }
    }
    return false;
}

void Node::addService(Service service)
{
    services.push_back(service);
    openPorts.push_back(service.port);
}

void Node::removeService(int port)
{
    for (size_t i = 0; i < services.size(); i++) {
        if (services[i].port == port) {
            services.erase(services.begin() + i);
            break;
        }
    }
    
    // Remove from openPorts as well
    for (size_t i = 0; i < openPorts.size(); i++) {
        if (openPorts[i] == port) {
            openPorts.erase(openPorts.begin() + i);
            break;
        }
    }
}

Service Node::getService(int port)
{
    for (Service service : services) {
        if (service.port == port) {
            return service;
        }
    }
    return Service();
}

std::vector<Service> Node::getAllServices()
{
    return services;
}

void Node::setHostname(std::string name)
{
    hostname = name;
}

std::string Node::getHostname()
{
    return hostname;
}

// Node template methods
void Node::createWebServer() {
    addService(Service(80, "HTTP"));
    addService(Service(443, "HTTPS"));
    addService(Service(22, "SSH"));
    hostname = "webserver-" + ip.substr(ip.find_last_of('.') + 1);
    
    // Add some default files
    addFile("/var/www/html/index.html", "<html><body><h1>Welcome to Web Server</h1></body></html>");
    addFile("/etc/apache2/apache2.conf", "ServerName localhost\nDocumentRoot /var/www/html");
}

void Node::createFileServer() {
    addService(Service(21, "FTP"));
    addService(Service(445, "SMB"));
    addService(Service(22, "SSH"));
    hostname = "fileserver-" + ip.substr(ip.find_last_of('.') + 1);
    
    // Add some default files
    addFile("/home/user/documents/secret.txt", "This is a secret file");
    addFile("/var/ftp/pub/readme.txt", "Welcome to FTP server");
}

void Node::createWorkstation() {
    addService(Service(3389, "RDP"));
    addService(Service(135, "RPC"));
    addService(Service(139, "NetBIOS"));
    hostname = "workstation-" + ip.substr(ip.find_last_of('.') + 1);
    
    // Add user account
    addUser("admin", "password123");
    addUser("user", "user123");
    
    // Add some files
    addFile("/home/user/desktop/notes.txt", "Important meeting notes");
    addFile("/home/user/documents/report.pdf", "Confidential report");
}

void Node::createDomainController() {
    addService(Service(389, "LDAP"));
    addService(Service(636, "LDAPS"));
    addService(Service(88, "Kerberos"));
    addService(Service(135, "RPC"));
    hostname = "dc-" + ip.substr(ip.find_last_of('.') + 1);
    
    // Add domain users
    addUser("administrator", "Admin123!");
    addUser("domain_admin", "Domain123!");
    
    // Add domain files
    addFile("/etc/ldap/slapd.conf", "Database configuration");
    addFile("/var/lib/ldap/domain.ldif", "Domain user database");
}

// File system methods
void Node::addFile(std::string path, std::string content) {
    fileSystem[path] = content;
}

std::string Node::readFile(std::string path) {
    if (fileSystem.find(path) != fileSystem.end()) {
        return fileSystem[path];
    }
    return "";
}

bool Node::fileExists(std::string path) {
    return fileSystem.find(path) != fileSystem.end();
}

// User management
void Node::addUser(std::string username, std::string password) {
    users[username] = password; // In real implementation, hash the password
}

bool Node::authenticateUser(std::string username, std::string password) {
    if (users.find(username) != users.end()) {
        return users[username] == password;
    }
    return false;
}