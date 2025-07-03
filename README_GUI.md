# RedNet macOS-Style GUI

## Overview

RedNet now features a modern, macOS-style graphical user interface that provides an intuitive and visually appealing way to interact with the AI Cyber Range Simulator. The interface is designed to resemble macOS System Preferences with a sidebar navigation layout and native macOS design language.

## Features

### 🎨 Modern macOS Design
- **Sidebar Navigation**: Left sidebar with categorized sections and colorful SF Symbol-style icons
- **Native Styling**: Soft light gray and white color palette with subtle shadows and rounded corners (12-16px)
- **Translucent Elements**: Subtle blur effects and translucent backgrounds
- **Blue Accent**: macOS-style blue accent color for selections and highlights
- **Auto Dark Mode**: Automatically adapts to macOS light/dark mode preferences

### 🧭 Navigation Structure
The sidebar is organized into five main categories:

1. **AI Agents** 🤖
   - Overview: Agent statistics and performance metrics
   - Configuration: Agent settings and parameters
   - Performance: Real-time performance monitoring

2. **Network** 🌐
   - Topology: Network structure visualization
   - Scanning: Network discovery and scanning tools
   - Monitoring: Real-time network monitoring

3. **Security** 🔒
   - Detection: Intrusion detection system
   - Response: Automated response mechanisms
   - Threats: Threat feed and analysis

4. **System** ⚙️
   - General: System configuration
   - Performance: System metrics and monitoring
   - Logs: System and application logs

5. **Simulation** 🎮
   - Controls: Simulation start/stop/pause controls
   - Statistics: Real-time simulation statistics
   - Export: Data export and reporting

### 🎚️ Interactive Elements
- **Toggle Switches**: Native macOS-style toggle controls
- **Sliders**: Smooth, responsive sliders with visual feedback
- **Dropdowns**: Clean, minimal dropdown menus
- **Tables**: Sortable tables with alternating row colors
- **Progress Bars**: Visual progress indicators
- **Buttons**: Rounded, modern button designs

### 📊 Real-time Monitoring
- **Agent Status**: Live monitoring of AI agent activities
- **Network Topology**: Dynamic network visualization
- **Security Alerts**: Real-time threat detection and alerts
- **System Metrics**: CPU, memory, and network usage
- **Simulation Statistics**: Live simulation data and metrics

## Getting Started

### Prerequisites
- macOS 10.15 (Catalina) or later
- OpenGL-compatible graphics card
- CMake 3.10 or later
- C++17 compatible compiler

### Building the GUI

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd RedNet
   ```

2. **Build the project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the GUI**:
   ```bash
   ./bin/RedNet
   ```

### Command Line Options
- `--gui` or `-g`: Run GUI dashboard (default)
- `--shell` or `-s`: Run command-line shell
- `--help` or `-h`: Show help message

## Usage Guide

### Basic Navigation
1. **Select a Category**: Click on any category in the left sidebar
2. **Explore Sub-sections**: Expand categories to see detailed options
3. **Interact with Controls**: Use buttons, sliders, and toggles to control the simulation
4. **Monitor Progress**: Watch real-time updates in the various monitoring panels

### AI Agent Management
1. **Add Agents**: Use the "Add Recon Agent" or "Add Exploit Agent" buttons
2. **Monitor Performance**: View agent statistics in the overview section
3. **Control Agents**: Start, stop, or pause individual agents
4. **View Details**: Check agent intelligence, success rates, and current activities

### Network Operations
1. **Generate Topology**: Create new network topologies with different configurations
2. **Scan Network**: Perform network discovery and vulnerability scanning
3. **Monitor Traffic**: Watch real-time network traffic and packet flow
4. **View Nodes**: Examine individual network nodes and their services

### Security Monitoring
1. **View Threats**: Monitor the threat feed for security alerts
2. **Configure Detection**: Adjust intrusion detection sensitivity
3. **Review Alerts**: Check recent security events and their status
4. **Set Response**: Configure automated response mechanisms

### Simulation Control
1. **Start Simulation**: Launch the full cyber range simulation
2. **Pause/Resume**: Control simulation execution
3. **Monitor Progress**: Track simulation statistics and metrics
4. **Export Data**: Save simulation results and logs

## Keyboard Shortcuts

- **Cmd+Q**: Quit RedNet
- **Cmd+S**: Toggle sidebar visibility
- **Cmd+M**: Minimize window
- **Cmd+W**: Close window

## Technical Details

### Architecture
- **Framework**: Dear ImGui (Immediate Mode GUI)
- **Rendering**: OpenGL 3.3+
- **Window Management**: GLFW 3.4+
- **Fonts**: SF Pro (macOS system font) + FontAwesome icons
- **Styling**: Custom macOS-style theme with auto dark mode support

### Performance
- **60 FPS**: Smooth, responsive interface
- **Real-time Updates**: Live data updates without blocking
- **Memory Efficient**: Minimal memory footprint
- **GPU Accelerated**: Hardware-accelerated rendering

### Customization
The GUI can be customized by modifying:
- `ui/desktop/ImGuiDashboard.cpp`: Main interface logic
- `ui/desktop/ImGuiDashboard.h`: Interface structure and data
- Color schemes and styling in the `applyMacOSStyle()` function
- Sidebar categories in the `initializeSidebarCategories()` function

## Troubleshooting

### Common Issues

1. **Font Not Loading**
   - Ensure FontAwesome font file is present in `fonts/fa-solid-900.ttf`
   - Check file permissions and paths

2. **OpenGL Errors**
   - Verify OpenGL 3.3+ support
   - Update graphics drivers if necessary

3. **Window Not Displaying**
   - Check GLFW installation
   - Verify display permissions on macOS

4. **Performance Issues**
   - Reduce window size if needed
   - Close other GPU-intensive applications
   - Check system resource usage

### Debug Mode
Enable debug mode in the System Configuration section to get detailed logging information.

## Contributing

To contribute to the GUI development:

1. Follow the existing code style and patterns
2. Test on both light and dark mode
3. Ensure accessibility compliance
4. Add appropriate error handling
5. Update documentation for new features

## License

This GUI is part of the RedNet AI Cyber Range Simulator and is for educational use only.

---

*RedNet GUI - Modern macOS-style interface for AI-powered cybersecurity simulation* 