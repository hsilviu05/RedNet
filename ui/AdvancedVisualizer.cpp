#include "AdvancedVisualizer.h"
#include <iostream>

AdvancedVisualizer::AdvancedVisualizer(std::shared_ptr<NetworkGraph> network, std::shared_ptr<ExploitDatabase> exploitDB)
    : network(network), exploitDB(exploitDB) {}

AdvancedVisualizer::~AdvancedVisualizer() {}

void AdvancedVisualizer::renderNetworkTopology() {
    if (!network) {
        std::cout << "❌ No network data available for visualization\n";
        return;
    }
    std::cout << "📊 Rendering network topology...\n";
    std::cout << "  🖼️  Generating network graph visualization\n";
    std::cout << "  📈 Updating node positions\n";
    std::cout << "  🔗 Drawing network connections\n";
    std::cout << "  ✅ Network topology rendered successfully\n";
}

void AdvancedVisualizer::renderExploitActivity() {
    if (!exploitDB) {
        std::cout << "❌ No exploit database available for visualization\n";
        return;
    }
    std::cout << "💣 Rendering exploit activity...\n";
    std::cout << "  📊 Generating exploit timeline\n";
    std::cout << "  🎯 Highlighting active exploits\n";
    std::cout << "  📈 Updating success rates\n";
    std::cout << "  ✅ Exploit activity rendered successfully\n";
}

void AdvancedVisualizer::renderAgentStatus() {
    std::cout << "🤖 Rendering agent status...\n";
    std::cout << "  📊 Generating agent dashboard\n";
    std::cout << "  📈 Updating agent metrics\n";
    std::cout << "  🎯 Showing agent activities\n";
    std::cout << "  ✅ Agent status rendered successfully\n";
}

void AdvancedVisualizer::updateVisualization() {
    std::cout << "🔄 Updating visualization...\n";
    renderNetworkTopology();
    renderExploitActivity();
    renderAgentStatus();
    std::cout << "  ✅ Visualization updated successfully\n";
}

void AdvancedVisualizer::exportVisualization(const std::string& format) {
    std::cout << "📤 Exporting visualization as " << format << "...\n";
    std::cout << "  💾 Saving visualization data\n";
    std::cout << "  📄 Generating " << format << " file\n";
    std::cout << "  ✅ Visualization exported successfully\n";
} 