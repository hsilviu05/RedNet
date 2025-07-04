#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>

enum class ScanType {
    QUICK,
    FULL,
    STEALTH,
    VULNERABILITY
};

struct ScanResult {
    int port;
    std::string service;
    std::string status;
    std::string vulnerability;
    bool isOpen = false;
    std::string banner;
};

class Scanner {
private:
    std::atomic<bool> isScanning;
    std::atomic<float> scanProgress;
    std::vector<ScanResult> scanResults;
    std::string currentTarget;
    ScanType currentScanType;
    std::thread scanThread;

    void performScan();
    std::string getServiceName(int port);

public:
    Scanner();
    ~Scanner();

    void startScan(const std::string& target, ScanType type);
    void stopScan();
    bool isScanActive() const;
    float getScanProgress() const;
    const std::vector<ScanResult>& getScanResults() const;
    
    static std::string getScanTypeName(ScanType type);
    void exportResults(const std::string& filename);
    std::vector<ScanResult> filterResults(const std::string& filter);
    void setVerbose(bool verbose);
    void scanHost(const std::string& host);
    std::vector<ScanResult> getResults() const;
};

#endif // SCANNER_H 