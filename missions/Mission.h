#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

enum class MissionType {
    RECONNAISSANCE,
    INITIAL_ACCESS,
    PERSISTENCE,
    PRIVILEGE_ESCALATION,
    LATERAL_MOVEMENT,
    DATA_EXFILTRATION,
    FULL_PENTEST
};

enum class ObjectiveType {
    SCAN_NETWORK,
    EXPLOIT_SERVICE,
    GAIN_ACCESS,
    ESCALATE_PRIVILEGES,
    MOVE_LATERALLY,
    EXFILTRATE_DATA,
    MAINTAIN_PERSISTENCE,
    COVER_TRACKS
};

struct Objective {
    std::string id;
    std::string name;
    std::string description;
    ObjectiveType type;
    std::vector<std::string> requirements;
    std::vector<std::string> targetNodes;
    std::map<std::string, std::string> parameters;
    bool completed = false;
    int points = 10;
};

struct MissionNode {
    std::string ip;
    std::string hostname;
    std::string os;
    std::vector<std::string> services;
    std::vector<std::string> vulnerabilities;
    std::map<std::string, std::string> configuration;
};

struct Mission {
    std::string id;
    std::string name;
    std::string description;
    MissionType type;
    std::string difficulty; // "Beginner", "Intermediate", "Advanced", "Expert"
    int timeLimit; // in minutes, 0 for no limit
    std::vector<Objective> objectives;
    std::vector<MissionNode> nodes;
    std::map<std::string, std::string> environment;
    std::vector<std::string> prerequisites;
    int totalPoints = 0;
    bool completed = false;
};

class MissionManager {
public:
    MissionManager();
    ~MissionManager();
    
    // Mission loading and management
    bool loadMission(const std::string& missionFile);
    bool loadMissionFromJson(const std::string& jsonData);
    std::vector<Mission> getAvailableMissions();
    Mission* getCurrentMission();
    
    // Mission loading helpers
    void loadAvailableMissions();
    
    // Mission execution
    bool loadMissionById(const std::string& missionId);
    bool startMission(const std::string& missionId);
    bool completeObjective(const std::string& objectiveId);
    bool isMissionCompleted();
    int getMissionProgress();
    int getTotalScore();
    
    // Mission validation
    bool validateMission(const Mission& mission);
    bool checkPrerequisites(const std::string& missionId);
    
    // Mission state
    void saveMissionState();
    void loadMissionState();
    void resetMission();
    
private:
    std::vector<Mission> availableMissions;
    std::unique_ptr<Mission> currentMission;
    std::map<std::string, bool> completedMissions;
    std::map<std::string, int> missionScores;
    
    // Helper methods
    bool parseMissionJson(const std::string& jsonData, Mission& mission);
    bool validateObjective(const Objective& objective);
    void calculateTotalPoints(Mission& mission);
}; 