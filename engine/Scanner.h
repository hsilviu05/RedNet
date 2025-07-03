#ifndef SCANNER_H
#define SCANNER_H

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

struct ScanResult {
    std::string ip;
    int port;
    std::string service;
    std::string banner;
    bool isOpen;
    
    ScanResult() : port(0), isOpen(false) {}
    ScanResult(std::string ip, int port, std::string service) 
        : ip(ip), port(port), service(service), isOpen(true) {}
};

enum ScanType {
    TCP_CONNECT,
    TCP_SYN,
    UDP
};

struct ServiceInfo {
    std::string name;
    std::string version;
    std::string banner;
};

class Scanner {
    public:
        Scanner();
        ~Scanner();
        std::map<int, ServiceInfo> serviceDatabase;
        std::vector<ScanResult> scanResults;
        std::unordered_set<std::string> scannedIPs;
        int scanTimeout;
        bool verboseMode;

    public:
        void scanPort(std::string ip, int port);
        void scanPortRange(std::string ip, int startPort, int endPort);
        void scanNetwork(std::string ipRange);
        void fingerprintService(std::string ip, int port);
        void grabBanner(std::string ip, int port);
        void scanHost(std::string ip);
        std::vector<ScanResult> getResults();
        void clearResults();
        void setVerbose(bool verbose);
        void setTimeout(int timeout);
        bool isPortOpen(std::string ip, int port);
};



#endif // SCANNER_H