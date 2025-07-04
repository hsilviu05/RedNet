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
    
    // Check if we're running from an app bundle (macOS)
    std::string bundleResourcesPath = "Resources/missions";
    if (std::filesystem::exists(bundleResourcesPath)) {
        missionsDir = bundleResourcesPath;
        std::cout << "📁 Using app bundle missions directory: " << missionsDir << std::endl;
    } else if (std::filesystem::exists("missions")) {
        missionsDir = "missions";
        std::cout << "📁 Using project missions directory: " << missionsDir << std::endl;
    } else {
        std::cout << "❌ No missions directory found!" << std::endl;
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
    // Simple JSON parsing for mission structure
    // In a real implementation, you'd use a proper JSON library like nlohmann/json
    
    // For now, we'll create a basic parser that looks for key patterns
    std::istringstream iss(jsonData);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Parse mission ID
        if (line.find("\"id\"") != std::string::npos) {
            size_t start = line.find("\"id\"") + 6;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                mission.id = line.substr(start, end - start);
            }
        }
        
        // Parse mission name
        if (line.find("\"name\"") != std::string::npos) {
            size_t start = line.find("\"name\"") + 8;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                mission.name = line.substr(start, end - start);
            }
        }
        
        // Parse description
        if (line.find("\"description\"") != std::string::npos) {
            size_t start = line.find("\"description\"") + 15;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                mission.description = line.substr(start, end - start);
            }
        }
        
        // Parse difficulty
        if (line.find("\"difficulty\"") != std::string::npos) {
            size_t start = line.find("\"difficulty\"") + 14;
            size_t end = line.find("\"", start);
            if (end != std::string::npos) {
                mission.difficulty = line.substr(start, end - start);
            }
        }
    }
    
    // Set default values if not found
    if (mission.id.empty()) mission.id = "mission_" + std::to_string(availableMissions.size() + 1);
    if (mission.name.empty()) mission.name = "Untitled Mission";
    if (mission.description.empty()) mission.description = "No description provided";
    if (mission.difficulty.empty()) mission.difficulty = "Beginner";
    
    mission.timeLimit = 0; // No time limit by default
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

bool MissionManager::startMission(const std::string& missionId) {
    // Find the mission
    auto it = std::find_if(availableMissions.begin(), availableMissions.end(),
                          [&missionId](const Mission& m) { return m.id == missionId; });
    
    if (it == availableMissions.end()) {
        std::cout << "❌ Mission not found: " << missionId << std::endl;
        return false;
    }
    
    // Check prerequisites
    if (!checkPrerequisites(missionId)) {
        std::cout << "❌ Prerequisites not met for mission: " << missionId << std::endl;
        return false;
    }
    
    // Start the mission
    currentMission = std::make_unique<Mission>(*it);
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