# RedNet - AI-Powered Offensive Cybersecurity Simulator

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://github.com/hsilviu05/RedNet)

## 🎯 Overview

RedNet is a sophisticated multi-agent, AI-driven hacking simulation engine where bots learn to exploit, pivot, and escalate in a virtual network. It combines C++, reinforcement learning, simulated networking, and OS-level APIs to create a realistic cybersecurity training environment.

## 🧠 Core Features

### 🔍 Target Scanning Engine
- **Nmap-like simulation**: Port scanning, banner grabbing, fingerprinting
- **Service enumeration**: Automatic detection of running services
- **Network topology discovery**: Map network structure and relationships

### 💣 Exploit Framework
- **Realistic vulnerabilities**: Weak passwords, open services, buffer overflows
- **Exploit chaining**: Combine multiple exploits for complex attack paths
- **Success probability**: Realistic exploit success rates based on target configuration

### 🧬 AI Agents (Reinforcement Learning)
- **Red team agents**: Discover IPs, probe services, select exploits
- **Lateral movement**: Chain exploits to move between systems
- **Stealth techniques**: Learn to avoid detection
- **Q-learning implementation**: Adaptive decision making

### 🖥️ Virtual Network
- **Simulated LAN**: Virtual machines with realistic configurations
- **Topology mapping**: Tree/graph-based network structure
- **Event-based communication**: Realistic network traffic simulation

### 🧪 Shell & Payload Engine
- **Shell access**: Limited command set on vulnerable nodes
- **Data exfiltration**: Extract secrets and sensitive files
- **Persistence mechanisms**: Install backdoors and maintain access
- **Lateral movement**: Propagate through the network

### 📜 Scriptable Missions
- **JSON/YAML configuration**: Define missions, nodes, and objectives
- **Scenario templates**: Pre-built attack scenarios
- **Objective tracking**: Monitor mission progress and success

## 🏗️ Architecture

```
RedNet/
├── main.cpp                 # Main simulation entry point
├── engine/                  # Core simulation components
│   ├── Node.h/cpp          # Virtual machine abstraction
│   ├── Exploit.h/cpp       # Exploit framework and database
│   ├── Scanner.h/cpp       # Network scanning engine
│   └── Payload.h/cpp       # Post-exploitation payloads
├── network/                # Network simulation (planned)
│   ├── NetworkGraph.h      # Network topology management
│   ├── Packet.h           # Network packet simulation
│   └── TopologyGenerator.h # Network generation
├── ai/                     # AI agents (planned)
│   ├── RedAgent.h         # Red team AI agent
│   ├── QTable.h           # Q-learning implementation
│   └── RewardSystem.h     # Reward calculation
├── missions/               # Mission system (planned)
│   ├── mission_001.json   # Mission definitions
│   └── parser.hpp         # Mission parser
├── shell/                  # Virtual shell (planned)
│   ├── CommandInterpreter.h # Command processing
│   └── FileSystemSim.h    # Virtual file system
├── utils/                  # Utilities (planned)
│   ├── Logger.h           # Logging system
│   ├── Timer.h            # Timing utilities
│   └── ConfigLoader.h     # Configuration management
├── ui/                     # User interface (planned)
│   ├── Menu.h             # CLI interface
│   └── NetworkViewer.cpp  # Network visualization
└── tests/                  # Unit tests (planned)
```

## 🚀 Current Implementation Status

### ✅ Completed Components

1. **Node System** - Virtual machine simulation with:
   - Multiple OS types (Linux, Windows, macOS)
   - Service management and configuration
   - Vulnerability tracking
   - File system simulation
   - User account management
   - Node templates (Web server, File server, Workstation, Domain Controller)

2. **Scanner Engine** - Network discovery with:
   - Port scanning simulation
   - Service fingerprinting
   - Banner grabbing
   - Network range scanning
   - Realistic timing and delays

3. **Exploit Framework** - Vulnerability exploitation with:
   - Base exploit interface
   - Brute force attacks
   - Service-specific exploits
   - Buffer overflow simulation
   - Privilege escalation
   - Exploit chaining
   - Exploit database management

4. **Payload System** - Post-exploitation capabilities:
   - Shell access with command restrictions
   - Data exfiltration from files and directories
   - Persistence mechanisms (cron jobs, SSH keys, hidden services)
   - Reconnaissance and information gathering
   - Lateral movement through networks
   - Payload chaining

### 🔄 In Progress
- Network topology simulation
- AI agent implementation
- Mission system
- User interface

### 📋 Planned Features
- Blue team agents (defenders)
- Advanced AI algorithms
- Real-time visualization
- Multi-player scenarios
- Export/import capabilities

## 🛠️ Building RedNet

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- Git

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/hsilviu05/RedNet.git
cd RedNet

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)

# Run the simulator
./RedNet
```

### Development Setup

```bash
# Install development dependencies
sudo apt-get install build-essential cmake git

# Clone and setup
git clone https://github.com/hsilviu05/RedNet.git
cd RedNet
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## 🎮 Usage Examples

### Basic Network Scanning
```cpp
#include "engine/Scanner.h"
#include "engine/Node.h"

int main() {
    // Create a virtual machine
    Node target("192.168.1.100", LINUX);
    target.createWebServer();
    
    // Scan the target
    Scanner scanner;
    scanner.scanHost("192.168.1.100");
    
    // Get scan results
    auto results = scanner.getResults();
    return 0;
}
```

### Exploit Execution
```cpp
#include "engine/Exploit.h"
#include "engine/Node.h"

int main() {
    // Create target and exploit database
    Node target("192.168.1.100", LINUX);
    target.createWorkstation();
    
    ExploitDatabase exploits;
    auto applicableExploits = exploits.getExploitsForNode(target);
    
    // Execute exploits
    for (auto exploit : applicableExploits) {
        ExploitResult result = exploit->execute(target);
        if (result.success) {
            std::cout << "Exploit successful: " << result.message << std::endl;
        }
    }
    return 0;
}
```

### Payload Execution
```cpp
#include "engine/Payload.h"
#include "engine/Node.h"

int main() {
    // Create compromised target
    Node target("192.168.1.100", LINUX);
    target.createFileServer();
    target.markCompromised();
    
    // Execute payloads
    PayloadDatabase payloads;
    auto shellPayload = payloads.findPayloadByName("Shell Access");
    
    if (shellPayload) {
        PayloadResult result = shellPayload->execute(target);
        if (result.success) {
            std::cout << "Shell access established" << std::endl;
        }
    }
    return 0;
}
```

## 🔧 Configuration

### Node Templates
RedNet includes several pre-configured node templates:

- **Web Server**: HTTP, HTTPS, SSH services
- **File Server**: FTP, SMB, SSH services
- **Workstation**: RDP, RPC, NetBIOS services
- **Domain Controller**: LDAP, Kerberos, RPC services

### Exploit Database
The exploit database includes:
- Brute force attacks
- Service-specific vulnerabilities
- Buffer overflow exploits
- Privilege escalation methods

### Payload Types
Available payload types:
- Shell access and command execution
- Data exfiltration
- Persistence mechanisms
- Reconnaissance
- Lateral movement

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Workflow
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🛡️ Security Notice

**⚠️ IMPORTANT**: RedNet is designed for educational and research purposes only. It simulates cybersecurity attacks in a controlled, virtual environment. Do not use this software against real systems without explicit permission.

## 🙏 Acknowledgments

- Inspired by real-world penetration testing tools
- Built with modern C++ practices
- Designed for cybersecurity education and research

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/hsilviu05/RedNet/issues)
- **Discussions**: [GitHub Discussions](https://github.com/hsilviu05/RedNet/discussions)
- **Wiki**: [Project Wiki](https://github.com/hsilviu05/RedNet/wiki)

---

**RedNet** - Empowering cybersecurity education through AI-driven simulation.

*Built with ❤️ for the cybersecurity community* 