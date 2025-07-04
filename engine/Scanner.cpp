#include "Scanner.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

Scanner::Scanner() : isScanning(false), scanProgress(0.0f) {
    scanResults.clear();
}

Scanner::~Scanner() {
    stopScan();
}

void Scanner::startScan(const std::string& target, ScanType type) {
    if (isScanning) {
        std::cout << "Scan already in progress..." << std::endl;
        return;
    }
    
    isScanning = true;
    scanProgress = 0.0f;
    scanResults.clear();
    currentTarget = target;
    currentScanType = type;
    
    std::cout << "Starting " << getScanTypeName(type) << " scan on " << target << std::endl;
    
    // Start scan in background thread
    scanThread = std::thread(&Scanner::performScan, this);
}

void Scanner::stopScan() {
    if (isScanning) {
        isScanning = false;
        if (scanThread.joinable()) {
            scanThread.join();
        }
        std::cout << "Scan stopped." << std::endl;
    }
}

bool Scanner::isScanActive() const {
    return isScanning;
}

float Scanner::getScanProgress() const {
    return scanProgress;
}

const std::vector<ScanResult>& Scanner::getScanResults() const {
    return scanResults;
}

void Scanner::performScan() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> progressDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> portDist(1, 65535);
    std::uniform_int_distribution<int> delayDist(50, 200);
    
    // Simulate scan progress
    for (int i = 0; i < 100 && isScanning; ++i) {
        scanProgress = static_cast<float>(i) / 100.0f;
        
        // Simulate finding ports/services
        if (i % 10 == 0 && isScanning) {
            ScanResult result;
            result.port = portDist(gen);
            result.service = getServiceName(result.port);
            result.status = (portDist(gen) % 3 == 0) ? "open" : "closed";
            result.vulnerability = (portDist(gen) % 5 == 0) ? "potential" : "none";
            
            scanResults.push_back(result);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delayDist(gen)));
    }
    
    if (isScanning) {
        scanProgress = 1.0f;
        std::cout << "Scan completed. Found " << scanResults.size() << " results." << std::endl;
    }
    
    isScanning = false;
}

std::string Scanner::getScanTypeName(ScanType type) {
    switch (type) {
        case ScanType::QUICK: return "Quick";
        case ScanType::FULL: return "Full";
        case ScanType::STEALTH: return "Stealth";
        case ScanType::VULNERABILITY: return "Vulnerability";
        default: return "Unknown";
    }
}

std::string Scanner::getServiceName(int port) {
    switch (port) {
        case 21: return "FTP";
        case 22: return "SSH";
        case 23: return "Telnet";
        case 25: return "SMTP";
        case 53: return "DNS";
        case 80: return "HTTP";
        case 110: return "POP3";
        case 143: return "IMAP";
        case 443: return "HTTPS";
        case 993: return "IMAPS";
        case 995: return "POP3S";
        case 3306: return "MySQL";
        case 5432: return "PostgreSQL";
        case 8080: return "HTTP-Proxy";
        default: return "Unknown";
    }
}

void Scanner::exportResults(const std::string& filename) {
    // Implementation for exporting scan results
    std::cout << "Exporting scan results to " << filename << std::endl;
}

std::vector<ScanResult> Scanner::filterResults(const std::string& filter) {
    std::vector<ScanResult> filtered;
    for (const auto& result : scanResults) {
        if (result.service.find(filter) != std::string::npos ||
            result.status.find(filter) != std::string::npos ||
            result.vulnerability.find(filter) != std::string::npos) {
            filtered.push_back(result);
        }
    }
    return filtered;
} 

void Scanner::setVerbose(bool verbose) { /* stub */ }
void Scanner::scanHost(const std::string& host) { /* stub */ }
std::vector<ScanResult> Scanner::getResults() const { return scanResults; } 