# RedNet Mission System

The RedNet Mission System provides structured, educational cybersecurity scenarios that guide users through various offensive security techniques in a safe, controlled environment.

## 🎯 Mission Overview

The mission system consists of progressive scenarios that teach cybersecurity concepts from basic reconnaissance to advanced penetration testing techniques.

### Available Missions

#### 1. **Mission 001: Network Reconnaissance Basics** (Beginner)
- **Objective**: Learn fundamental network discovery and mapping techniques
- **Skills**: Network scanning, port enumeration, service fingerprinting
- **Duration**: 30 minutes
- **Points**: 100

#### 2. **Mission 002: Initial Access Techniques** (Intermediate)
- **Objective**: Master techniques for gaining initial access to target systems
- **Skills**: Vulnerability exploitation, brute force attacks, web application testing
- **Duration**: 45 minutes
- **Points**: 110
- **Prerequisites**: Mission 001

#### 3. **Mission 003: Privilege Escalation Mastery** (Advanced)
- **Objective**: Learn advanced privilege escalation techniques for both Windows and Linux
- **Skills**: System enumeration, privilege escalation, credential harvesting
- **Duration**: 60 minutes
- **Points**: 130
- **Prerequisites**: Mission 001, Mission 002

#### 4. **Mission 004: Complete Penetration Test** (Expert)
- **Objective**: Execute a full penetration test against a complex enterprise network
- **Skills**: Complete attack lifecycle, lateral movement, data exfiltration
- **Duration**: 120 minutes
- **Points**: 200
- **Prerequisites**: Mission 001, Mission 002, Mission 003

## 🏗️ Mission Structure

Each mission is defined in JSON format with the following components:

### Mission Properties
- **id**: Unique mission identifier
- **name**: Mission title
- **description**: Detailed mission description
- **type**: Mission category (RECONNAISSANCE, INITIAL_ACCESS, etc.)
- **difficulty**: Skill level required (Beginner, Intermediate, Advanced, Expert)
- **timeLimit**: Maximum time allowed in minutes
- **prerequisites**: List of missions that must be completed first

### Objectives
Each mission contains multiple objectives that must be completed in sequence:

- **id**: Unique objective identifier
- **name**: Objective title
- **description**: Detailed objective description
- **type**: Objective category (SCAN_NETWORK, EXPLOIT_SERVICE, etc.)
- **requirements**: List of objectives that must be completed first
- **targetNodes**: IP addresses of target systems
- **parameters**: Configuration parameters for the objective
- **points**: Points awarded for completion

### Network Nodes
Each mission defines a realistic network environment:

- **ip**: Node IP address
- **hostname**: System hostname
- **os**: Operating system (Linux, Windows)
- **services**: Running services (SSH, HTTP, FTP, etc.)
- **vulnerabilities**: Known vulnerabilities on the system
- **configuration**: Additional system configuration details

## 🚀 Using the Mission System

### Starting a Mission
```cpp
#include "missions/Mission.h"

MissionManager missionManager;

// Start a specific mission
if (missionManager.startMission("mission_001")) {
    std::cout << "Mission started successfully!" << std::endl;
}
```

### Checking Mission Progress
```cpp
auto* currentMission = missionManager.getCurrentMission();
if (currentMission) {
    int progress = missionManager.getMissionProgress();
    int score = missionManager.getTotalScore();
    
    std::cout << "Progress: " << progress << "%" << std::endl;
    std::cout << "Score: " << score << "/" << currentMission->totalPoints << std::endl;
}
```

### Completing Objectives
```cpp
// Complete an objective
if (missionManager.completeObjective("obj_001_scan_network")) {
    std::cout << "Objective completed!" << std::endl;
}

// Check if mission is complete
if (missionManager.isMissionCompleted()) {
    std::cout << "Mission completed!" << std::endl;
}
```

### Getting Available Missions
```cpp
auto missions = missionManager.getAvailableMissions();
for (const auto& mission : missions) {
    std::cout << "Mission: " << mission.name << " [" << mission.difficulty << "]" << std::endl;
}
```

## 📋 Mission Types

### RECONNAISSANCE
- Network discovery and mapping
- Service enumeration
- Information gathering
- OSINT techniques

### INITIAL_ACCESS
- Vulnerability exploitation
- Brute force attacks
- Web application testing
- Social engineering simulation

### PERSISTENCE
- Backdoor installation
- Scheduled task creation
- Registry modifications
- Service creation

### PRIVILEGE_ESCALATION
- Linux privilege escalation
- Windows privilege escalation
- Credential harvesting
- Kernel exploitation

### LATERAL_MOVEMENT
- Pass-the-hash attacks
- Credential reuse
- Service exploitation
- Network pivoting

### DATA_EXFILTRATION
- File extraction
- Database dumping
- Credential harvesting
- Encrypted exfiltration

### FULL_PENTEST
- Complete attack lifecycle
- Multiple attack vectors
- Advanced techniques
- Professional reporting

## 🎮 Educational Benefits

### Structured Learning
- Progressive difficulty levels
- Skill-based progression
- Comprehensive coverage of techniques

### Realistic Scenarios
- Authentic network environments
- Real-world vulnerabilities
- Professional penetration testing workflow

### Skill Development
- Hands-on practice
- Technique mastery
- Problem-solving skills

### Assessment
- Point-based scoring
- Progress tracking
- Performance metrics

## 🔧 Customizing Missions

### Creating New Missions
1. Create a new JSON file in the `missions/` directory
2. Follow the mission structure format
3. Define objectives, nodes, and parameters
4. Set appropriate difficulty and prerequisites

### Modifying Existing Missions
- Edit the JSON files to customize scenarios
- Adjust difficulty levels and time limits
- Add or remove objectives
- Modify network topologies

### Mission Parameters
- **network_range**: Target network range
- **scan_type**: Type of scanning to perform
- **ports**: Port ranges to scan
- **attack_type**: Specific attack method
- **payload**: Exploit payload to use

## 📊 Mission Statistics

- **Total Missions**: 4
- **Total Objectives**: 22
- **Total Points Available**: 540
- **Difficulty Levels**: 4 (Beginner to Expert)
- **Network Environments**: 4 unique scenarios

## 🛡️ Safety and Ethics

- All missions are educational simulations
- No real systems are targeted
- Designed for security training and research
- Follow responsible disclosure practices
- Educational use only

## 📚 Additional Resources

- RedNet Documentation: See main README.md
- Mission Examples: Review existing JSON files
- Best Practices: Follow cybersecurity guidelines
- Legal Compliance: Ensure proper authorization

---

**Note**: This mission system is designed for educational purposes only. Always ensure you have proper authorization before testing security techniques on any real systems. 