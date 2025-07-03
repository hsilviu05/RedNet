#include "Scanner.h"

Scanner::Scanner() {
    // Initialize service database with common ports
    serviceDatabase[22] = {"SSH", "OpenSSH 7.2p2", "SSH-2.0-OpenSSH_7.2p2 Ubuntu-4ubuntu2.2"};
    serviceDatabase[21] = {"FTP", "vsFTPd 2.3.4", "220 (vsFTPd 2.3.4)"};
    serviceDatabase[80] = {"HTTP", "Apache 2.4.41", "HTTP/1.1 200 OK"};
    serviceDatabase[443] = {"HTTPS", "Apache 2.4.41", "HTTP/1.1 200 OK"};
    serviceDatabase[3389] = {"RDP", "Windows RDP", "RDP"};
    serviceDatabase[445] = {"SMB", "Samba 4.7.6", "SMB"};
    serviceDatabase[135] = {"RPC", "Windows RPC", "RPC"};
    serviceDatabase[139] = {"NetBIOS", "Windows NetBIOS", "NetBIOS"};
    serviceDatabase[23] = {"Telnet", "Linux telnetd", "telnet"};
    serviceDatabase[25] = {"SMTP", "Postfix", "220 mail.example.com"};
    
    scanTimeout = 3000;  // 3 seconds default
    verboseMode = false;
}

Scanner::~Scanner() {
    scanResults.clear();
    scannedIPs.clear();
}

void Scanner::scanPort(std::string ip, int port)
{
    ScanResult result;
    result.ip = ip;
    result.port = port;
    
    // Simulate port scanning with random success
    bool isOpen = (rand() % 100) < 70; // 70% chance port is open
    
    if (isOpen) {
        result.isOpen = true;
        
        // Get service info from database
        if (serviceDatabase.find(port) != serviceDatabase.end()) {
            ServiceInfo service = serviceDatabase[port];
            result.service = service.name;
            result.banner = service.banner;
        } else {
            result.service = "Unknown";
            result.banner = "";
        }
        
        // Simulate banner grabbing
        grabBanner(ip, port);
        fingerprintService(ip, port);
    } else {
        result.isOpen = false;
        result.service = "";
        result.banner = "";
    }
    
    scanResults.push_back(result); 
}

void Scanner::scanPortRange(std::string ip, int startPort, int endPort)
{
    for (int port = startPort; port <= endPort; port++) {
        scanPort(ip, port);
        
        // Add small delay for realism
        if (verboseMode) {
            std::cout << "Scanning " << ip << ":" << port << std::endl;
        }
    }
}

void Scanner::scanNetwork(std::string ipRange)
{
    std::vector<std::string> ips = {"192.168.1.1", "192.168.1.2", "192.168.1.10", "192.168.1.100"};
    
    for (std::string ip : ips) {
        scanHost(ip);
    }
}

void Scanner::fingerprintService(std::string ip, int port)
{
    if (verboseMode) {
        std::cout << "Fingerprinting service on " << ip << ":" << port << std::endl;
    }
}

void Scanner::grabBanner(std::string ip, int port)
{
    if (verboseMode) {
        std::cout << "Grabbing banner from " << ip << ":" << port << std::endl;
    }
}

void Scanner::scanHost(std::string ip)
{
    if (scannedIPs.find(ip) != scannedIPs.end()) {
        return; // Already scanned
    }
    
    scannedIPs.insert(ip);
    
    // Scan common ports
    std::vector<int> commonPorts = {21, 22, 23, 25, 80, 443, 135, 139, 445, 3389};
    
    for (int port : commonPorts) {
        scanPort(ip, port);
    }
}

std::vector<ScanResult> Scanner::getResults()
{
    return scanResults;
}

void Scanner::clearResults()
{
    scanResults.clear();
    scannedIPs.clear();
}
void Scanner::setVerbose(bool verbose)
{   
    verboseMode = verbose;
}
void Scanner::setTimeout(int timeout)
{
    scanTimeout = timeout;
}

bool Scanner::isPortOpen(std::string ip, int port)
{
    for (const ScanResult& result : scanResults) {
        if (result.ip == ip && result.port == port && result.isOpen) {
            return true;
        }
    }
    return false;
}
