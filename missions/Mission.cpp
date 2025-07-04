#include "Mission.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

MissionManager::MissionManager() {
    // Load available missions from the missions directory
    loadAvailableMissions();
}

MissionManager::~MissionManager() = default;

void MissionManager::loadAvailableMissions() {
    std::string missionsDir = "missions";
    
    // Try multiple possible paths for missions directory
    std::vector<std::string> possiblePaths = {
        "Resources/missions",                    // App bundle relative
        "../Resources/missions",                 // App bundle from MacOS dir
        "missions",                             // Project directory
        "../missions",                          // Project directory one level up
        "build/bin/RedNet.app/Contents/Resources/missions",  // Full path from project root
        "/Users/silviu/RedNet/missions",        // Absolute path to project missions
        "/Users/silviu/RedNet/build/bin/RedNet.app/Contents/Resources/missions"  // Absolute path to app bundle
    };
    
    bool found = false;
    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            missionsDir = path;
            std::cout << "📁 Using missions directory: " << missionsDir << std::endl;
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "❌ No missions directory found! Tried paths:" << std::endl;
        for (const auto& path : possiblePaths) {
            std::cout << "  - " << path << " (exists: " << (std::filesystem::exists(path) ? "yes" : "no") << ")" << std::endl;
        }
        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
        return;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(missionsDir)) {
            if (entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    std::string jsonData = buffer.str();
                    
                    Mission mission;
                    if (parseMissionJson(jsonData, mission)) {
                        availableMissions.push_back(mission);
                        std::cout << "✅ Loaded mission: " << mission.name << std::endl;
                    } else {
                        std::cout << "❌ Failed to parse mission: " << entry.path().filename() << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "⚠️ Warning: Could not load missions directory: " << e.what() << std::endl;
    }
}

bool MissionManager::loadMission(const std::string& missionFile) {
    std::ifstream file(missionFile);
    if (!file.is_open()) {
        std::cout << "❌ Could not open mission file: " << missionFile << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return loadMissionFromJson(buffer.str());
}

bool MissionManager::loadMissionFromJson(const std::string& jsonData) {
    Mission mission;
    if (parseMissionJson(jsonData, mission)) {
        if (validateMission(mission)) {
            currentMission = std::make_unique<Mission>(mission);
            std::cout << "🎯 Mission loaded: " << mission.name << std::endl;
            return true;
        } else {
            std::cout << "❌ Mission validation failed" << std::endl;
        }
    }
    return false;
}

bool MissionManager::parseMissionJson(const std::string& jsonData, Mission& mission) {
    // Enhanced JSON parsing for mission structure
    // This parser handles the root-level mission properties correctly
    
    std::istringstream iss(jsonData);
    std::string line;
    bool inObjectives = false;
    bool inNodes = false;
    int braceDepth = 0;
    
    while (std::getline(iss, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Track brace depth to understand JSON structure
        for (char c : line) {
            if (c == '{') braceDepth++;
            if (c == '}') braceDepth--;
        }
        
        // Check for objectives array start
        if (line.find("\"objectives\"") != std::string::npos && line.find("[") != std::string::npos) {
            inObjectives = true;
            continue;
        }
        
        // Check for nodes array start
        if (line.find("\"nodes\"") != std::string::npos && line.find("[") != std::string::npos) {
            inNodes = true;
            continue;
        }
        
        // Check for array end
        if (line.find("]") != std::string::npos && (inObjectives || inNodes)) {
            inObjectives = false;
            inNodes = false;
            continue;
        }
        
        // Only parse root-level properties (braceDepth == 1 means we're at the root level)
        if (braceDepth == 1 && !inObjectives && !inNodes) {
            // Parse mission ID
            if (line.find("\"id\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find("\"", start) + 1;
                size_t end = line.find("\"", start);
                if (end != std::string::npos) {
                    mission.id = line.substr(start, end - start);
                }
            }
            
            // Parse mission name
            if (line.find("\"name\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find("\"", start) + 1;
                size_t end = line.find("\"", start);
                if (end != std::string::npos) {
                    mission.name = line.substr(start, end - start);
                }
            }
            
            // Parse description
            if (line.find("\"description\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find("\"", start) + 1;
                size_t end = line.find("\"", start);
                if (end != std::string::npos) {
                    mission.description = line.substr(start, end - start);
                }
            }
            
            // Parse difficulty
            if (line.find("\"difficulty\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find("\"", start) + 1;
                size_t end = line.find("\"", start);
                if (end != std::string::npos) {
                    mission.difficulty = line.substr(start, end - start);
                }
            }
            
            // Parse time limit
            if (line.find("\"timeLimit\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find_first_not_of(" \t", start);
                size_t end = line.find_first_of(",}", start);
                if (end != std::string::npos) {
                    try {
                        mission.timeLimit = std::stoi(line.substr(start, end - start));
                    } catch (...) {
                        mission.timeLimit = 0;
                    }
                }
            }
            
            // Parse total points
            if (line.find("\"totalPoints\"") != std::string::npos && line.find(":") != std::string::npos) {
                size_t start = line.find(":") + 1;
                start = line.find_first_not_of(" \t", start);
                size_t end = line.find_first_of(",}", start);
                if (end != std::string::npos) {
                    try {
                        mission.totalPoints = std::stoi(line.substr(start, end - start));
                    } catch (...) {
                        mission.totalPoints = 0;
                    }
                }
            }
        }
        
        // Parse objectives (when we're inside the objectives array)
        if (inObjectives && line.find("\"id\"") != std::string::npos && line.find(":") != std::string::npos) {
            Objective obj;
            
            // Parse objective ID
            size_t start = line.find(":") + 1;
            start = line.find("\"", start) + 1;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                obj.id = line.substr(start, end - start);
            }
            
            // Set default values for objective
            obj.name = "Objective " + std::to_string(mission.objectives.size() + 1);
            obj.description = "Complete this objective";
            obj.type = ObjectiveType::SCAN_NETWORK;
            obj.completed = false;
            obj.points = 10;
            
            mission.objectives.push_back(obj);
        }
    }
    
    // Set default values if not found
    if (mission.id.empty()) mission.id = "mission_" + std::to_string(availableMissions.size() + 1);
    if (mission.name.empty()) mission.name = "Untitled Mission";
    if (mission.description.empty()) mission.description = "No description provided";
    if (mission.difficulty.empty()) mission.difficulty = "Beginner";
    
    // Ensure we have at least one objective
    if (mission.objectives.empty()) {
        Objective defaultObj;
        defaultObj.id = "default_objective";
        defaultObj.name = "Complete Mission";
        defaultObj.description = "Complete the mission objectives";
        defaultObj.type = ObjectiveType::SCAN_NETWORK;
        defaultObj.completed = false;
        defaultObj.points = 100;
        mission.objectives.push_back(defaultObj);
    }
    
    // Set mission type based on difficulty
    if (mission.difficulty == "Beginner") {
        mission.type = MissionType::RECONNAISSANCE;
    } else if (mission.difficulty == "Intermediate") {
        mission.type = MissionType::INITIAL_ACCESS;
    } else if (mission.difficulty == "Advanced") {
        mission.type = MissionType::PRIVILEGE_ESCALATION;
    } else {
        mission.type = MissionType::FULL_PENTEST;
    }
    
    mission.completed = false;
    
    return true;
}

bool MissionManager::validateMission(const Mission& mission) {
    if (mission.id.empty() || mission.name.empty()) {
        std::cout << "❌ Mission must have ID and name" << std::endl;
        return false;
    }
    
    if (mission.objectives.empty()) {
        std::cout << "❌ Mission must have at least one objective" << std::endl;
        return false;
    }
    
    for (const auto& objective : mission.objectives) {
        if (!validateObjective(objective)) {
            return false;
        }
    }
    
    calculateTotalPoints(const_cast<Mission&>(mission));
    return true;
}

bool MissionManager::validateObjective(const Objective& objective) {
    if (objective.id.empty() || objective.name.empty()) {
        std::cout << "❌ Objective must have ID and name" << std::endl;
        return false;
    }
    
    if (objective.points < 0) {
        std::cout << "❌ Objective points cannot be negative" << std::endl;
        return false;
    }
    
    return true;
}

void MissionManager::calculateTotalPoints(Mission& mission) {
    mission.totalPoints = 0;
    for (const auto& objective : mission.objectives) {
        mission.totalPoints += objective.points;
    }
}

std::vector<Mission> MissionManager::getAvailableMissions() {
    return availableMissions;
}

Mission* MissionManager::getCurrentMission() {
    return currentMission.get();
}

bool MissionManager::loadMissionById(const std::string& missionId) {
    // Find the mission in available missions
    auto it = std::find_if(availableMissions.begin(), availableMissions.end(),
                          [&missionId](const Mission& m) { return m.id == missionId; });
    
    if (it == availableMissions.end()) {
        std::cout << "❌ Mission not found: " << missionId << std::endl;
        return false;
    }
    
    // Load the mission as current mission
    currentMission = std::make_unique<Mission>(*it);
    std::cout << "📋 Mission loaded: " << currentMission->name << std::endl;
    std::cout << "📝 Description: " << currentMission->description << std::endl;
    std::cout << "🎯 Objectives: " << currentMission->objectives.size() << std::endl;
    std::cout << "🏆 Total points: " << currentMission->totalPoints << std::endl;
    
    return true;
}

bool MissionManager::startMission(const std::string& missionId) {
    // First load the mission if not already loaded
    if (!currentMission || currentMission->id != missionId) {
        if (!loadMissionById(missionId)) {
            return false;
        }
    }
    
    // Check prerequisites
    if (!checkPrerequisites(missionId)) {
        std::cout << "❌ Prerequisites not met for mission: " << missionId << std::endl;
        return false;
    }
    
    // Start the mission
    std::cout << "🚀 Starting mission: " << currentMission->name << std::endl;
    std::cout << "📋 Objectives: " << currentMission->objectives.size() << std::endl;
    std::cout << "🎯 Total points: " << currentMission->totalPoints << std::endl;
    
    return true;
}

bool MissionManager::completeObjective(const std::string& objectiveId) {
    if (!currentMission) return false;
    
    for (auto& objective : currentMission->objectives) {
        if (objective.id == objectiveId) {
            objective.completed = true;
            std::cout << "✅ Objective completed: " << objective.name << " (+" << objective.points << " points)" << std::endl;
            
            // Check if mission is completed
            if (isMissionCompleted()) {
                std::cout << "🎉 Mission completed: " << currentMission->name << std::endl;
                std::cout << "🏆 Final score: " << getTotalScore() << "/" << currentMission->totalPoints << std::endl;
            }
            
            return true;
        }
    }
    
    std::cout << "❌ Objective not found: " << objectiveId << std::endl;
    return false;
}

bool MissionManager::isMissionCompleted() {
    if (!currentMission) return false;
    
    for (const auto& objective : currentMission->objectives) {
        if (!objective.completed) return false;
    }
    
    currentMission->completed = true;
    return true;
}

int MissionManager::getMissionProgress() {
    if (!currentMission || currentMission->objectives.empty()) return 0;
    
    int completedObjectives = 0;
    for (const auto& objective : currentMission->objectives) {
        if (objective.completed) completedObjectives++;
    }
    
    return (completedObjectives * 100) / currentMission->objectives.size();
}

int MissionManager::getTotalScore() {
    if (!currentMission) return 0;
    
    int totalScore = 0;
    for (const auto& objective : currentMission->objectives) {
        if (objective.completed) totalScore += objective.points;
    }
    
    return totalScore;
}

bool MissionManager::checkPrerequisites(const std::string& missionId) {
    // Find the mission
    auto it = std::find_if(availableMissions.begin(), availableMissions.end(),
                          [&missionId](const Mission& m) { return m.id == missionId; });
    
    if (it == availableMissions.end()) return false;
    
    // Check if all prerequisites are completed
    for (const auto& prereq : it->prerequisites) {
        if (completedMissions.find(prereq) == completedMissions.end() || !completedMissions[prereq]) {
            return false;
        }
    }
    
    return true;
}

void MissionManager::saveMissionState() {
    // In a real implementation, this would save to a file
    std::cout << "💾 Mission state saved" << std::endl;
}

void MissionManager::loadMissionState() {
    // In a real implementation, this would load from a file
    std::cout << "📂 Mission state loaded" << std::endl;
}

void MissionManager::resetMission() {
    if (currentMission) {
        for (auto& objective : currentMission->objectives) {
            objective.completed = false;
        }
        currentMission->completed = false;
        std::cout << "🔄 Mission reset" << std::endl;
    }
} 