#include "Mission.h"
#include <iostream>
#include <string>
#include <vector>

void printMissionMenu(const std::vector<Mission>& missions) {
    std::cout << "\n🎯 Available Missions:\n";
    std::cout << "========================\n";
    
    for (size_t i = 0; i < missions.size(); ++i) {
        const auto& mission = missions[i];
        std::cout << (i + 1) << ". " << mission.name << " [" << mission.difficulty << "]\n";
        std::cout << "   📝 " << mission.description << "\n";
        std::cout << "   🎯 Points: " << mission.totalPoints << " | ⏱️ Time: " << mission.timeLimit << " min\n";
        std::cout << "   📋 Objectives: " << mission.objectives.size() << "\n\n";
    }
}

void printMissionDetails(const Mission& mission) {
    std::cout << "\n🎯 Mission Details: " << mission.name << "\n";
    std::cout << "=====================================\n";
    std::cout << "📝 Description: " << mission.description << "\n";
    std::cout << "🏷️ Difficulty: " << mission.difficulty << "\n";
    std::cout << "⏱️ Time Limit: " << mission.timeLimit << " minutes\n";
    std::cout << "🎯 Total Points: " << mission.totalPoints << "\n";
    std::cout << "🖥️ Target Nodes: " << mission.nodes.size() << "\n\n";
    
    std::cout << "📋 Objectives:\n";
    for (size_t i = 0; i < mission.objectives.size(); ++i) {
        const auto& obj = mission.objectives[i];
        std::cout << "   " << (i + 1) << ". " << obj.name << " (" << obj.points << " pts)\n";
        std::cout << "      📝 " << obj.description << "\n";
        if (!obj.requirements.empty()) {
            std::cout << "      🔗 Requires: ";
            for (const auto& req : obj.requirements) {
                std::cout << req << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "🖥️ Network Nodes:\n";
    for (const auto& node : mission.nodes) {
        std::cout << "   • " << node.hostname << " (" << node.ip << ") - " << node.os << "\n";
        std::cout << "     Services: ";
        for (const auto& service : node.services) {
            std::cout << service << " ";
        }
        std::cout << "\n";
        if (!node.vulnerabilities.empty()) {
            std::cout << "     Vulnerabilities: ";
            for (const auto& vuln : node.vulnerabilities) {
                std::cout << vuln << " ";
            }
        }
        std::cout << "\n\n";
    }
}

void demonstrateMissionSystem() {
    std::cout << "\n🎯 RedNet Mission System Demo\n";
    std::cout << "=============================\n";
    
    MissionManager missionManager;
    
    // Get available missions
    auto missions = missionManager.getAvailableMissions();
    
    if (missions.empty()) {
        std::cout << "❌ No missions found. Please ensure mission JSON files are in the missions/ directory.\n";
        return;
    }
    
    std::cout << "✅ Loaded " << missions.size() << " missions successfully!\n";
    
    // Display mission menu
    printMissionMenu(missions);
    
    // Demonstrate mission selection
    std::cout << "🎮 Mission System Features:\n";
    std::cout << "===========================\n";
    std::cout << "• Progressive difficulty levels (Beginner → Expert)\n";
    std::cout << "• Prerequisite system for mission unlocking\n";
    std::cout << "• Objective-based progression tracking\n";
    std::cout << "• Point scoring system\n";
    std::cout << "• Time limits for realistic scenarios\n";
    std::cout << "• Comprehensive network environments\n\n";
    
    // Show detailed mission information
    std::cout << "📊 Mission Statistics:\n";
    std::cout << "======================\n";
    
    int totalPoints = 0;
    int totalObjectives = 0;
    std::map<std::string, int> difficultyCount;
    
    for (const auto& mission : missions) {
        totalPoints += mission.totalPoints;
        totalObjectives += mission.objectives.size();
        difficultyCount[mission.difficulty]++;
    }
    
    std::cout << "📈 Total Missions: " << missions.size() << "\n";
    std::cout << "🎯 Total Objectives: " << totalObjectives << "\n";
    std::cout << "🏆 Total Points Available: " << totalPoints << "\n";
    std::cout << "📊 Difficulty Distribution:\n";
    
    for (const auto& [difficulty, count] : difficultyCount) {
        std::cout << "   • " << difficulty << ": " << count << " missions\n";
    }
    
    std::cout << "\n🚀 Mission Progression Path:\n";
    std::cout << "==========================\n";
    std::cout << "1. Mission 001: Network Reconnaissance Basics (Beginner)\n";
    std::cout << "   ↓ (Unlocks after completion)\n";
    std::cout << "2. Mission 002: Initial Access Techniques (Intermediate)\n";
    std::cout << "   ↓ (Unlocks after completion)\n";
    std::cout << "3. Mission 003: Privilege Escalation Mastery (Advanced)\n";
    std::cout << "   ↓ (Unlocks after completion)\n";
    std::cout << "4. Mission 004: Complete Penetration Test (Expert)\n\n";
    
    // Demonstrate mission starting
    std::cout << "🎮 Starting Mission Demo:\n";
    std::cout << "========================\n";
    
    if (missionManager.startMission("mission_001")) {
        std::cout << "✅ Mission started successfully!\n";
        
        auto* currentMission = missionManager.getCurrentMission();
        if (currentMission) {
            std::cout << "📊 Progress: " << missionManager.getMissionProgress() << "%\n";
            std::cout << "🎯 Score: " << missionManager.getTotalScore() << "/" << currentMission->totalPoints << "\n";
            
            // Simulate completing objectives
            std::cout << "\n🎯 Simulating Objective Completion:\n";
            for (const auto& objective : currentMission->objectives) {
                if (missionManager.completeObjective(objective.id)) {
                    std::cout << "✅ Completed: " << objective.name << "\n";
                    std::cout << "📊 Progress: " << missionManager.getMissionProgress() << "%\n";
                    std::cout << "🎯 Score: " << missionManager.getTotalScore() << "/" << currentMission->totalPoints << "\n\n";
                }
            }
            
            if (missionManager.isMissionCompleted()) {
                std::cout << "🎉 Mission completed! Final score: " << missionManager.getTotalScore() << "/" << currentMission->totalPoints << "\n";
            }
        }
    } else {
        std::cout << "❌ Failed to start mission\n";
    }
    
    std::cout << "\n💡 Mission System Benefits:\n";
    std::cout << "==========================\n";
    std::cout << "• Structured learning progression\n";
    std::cout << "• Realistic cybersecurity scenarios\n";
    std::cout << "• Skill-based objective completion\n";
    std::cout << "• Comprehensive network environments\n";
    std::cout << "• Professional penetration testing workflow\n";
    std::cout << "• Educational value for security training\n\n";
    
    std::cout << "🎯 Mission system demonstration completed!\n";
    std::cout << "📚 This system provides a comprehensive framework for learning offensive cybersecurity techniques.\n";
}

// main() function removed to avoid duplicate symbol error.
// If you want to use this as a standalone launcher, restore the main() and build as a separate executable. 