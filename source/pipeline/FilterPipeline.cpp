#include "pipeline/FilterPipeline.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace pipeline {

FilterPipeline::FilterPipeline() = default;
FilterPipeline::~FilterPipeline() = default;

std::string FilterPipeline::addNode(const std::string& type, const std::map<std::string, double>& params) {
    // Generate unique ID for the node
    std::string id = "node_" + std::to_string(nodes_.size());
    
    // Create the node
    PipelineNode node;
    node.id = id;
    node.type = type;
    node.parameters = params;
    
    // Add node to the pipeline
    nodes_.push_back(node);
    
    return id;
}

void FilterPipeline::removeNode(const std::string& nodeId) {
    // Remove the node
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [&](const PipelineNode& node) { return node.id == nodeId; }),
        nodes_.end());
    
    // Remove any connections involving this node
    for (auto& node : nodes_) {
        node.inputIds.erase(
            std::remove(node.inputIds.begin(), node.inputIds.end(), nodeId),
            node.inputIds.end());
        node.outputIds.erase(
            std::remove(node.outputIds.begin(), node.outputIds.end(), nodeId),
            node.outputIds.end());
    }
}

bool FilterPipeline::connectNodes(const std::string& sourceId, const std::string& targetId) {
    // Find the nodes
    auto sourceIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == sourceId; });
    auto targetIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == targetId; });
    
    if (sourceIt == nodes_.end() || targetIt == nodes_.end()) {
        return false;
    }
    
    // Add connection
    sourceIt->outputIds.push_back(targetId);
    targetIt->inputIds.push_back(sourceId);
    
    return true;
}

void FilterPipeline::disconnectNodes(const std::string& sourceId, const std::string& targetId) {
    // Find the nodes
    auto sourceIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == sourceId; });
    auto targetIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == targetId; });
    
    if (sourceIt == nodes_.end() || targetIt == nodes_.end()) {
        return;
    }
    
    // Remove connection
    sourceIt->outputIds.erase(
        std::remove(sourceIt->outputIds.begin(), sourceIt->outputIds.end(), targetId),
        sourceIt->outputIds.end());
    targetIt->inputIds.erase(
        std::remove(targetIt->inputIds.begin(), targetIt->inputIds.end(), sourceId),
        targetIt->inputIds.end());
}

std::map<std::string, double> FilterPipeline::getNodeParameters(const std::string& nodeId) const {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    
    if (it == nodes_.end()) {
        return {};
    }
    
    return it->parameters;
}

void FilterPipeline::setNodeParameters(const std::string& nodeId, const std::map<std::string, double>& params) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    
    if (it == nodes_.end()) {
        return;
    }
    
    it->parameters = params;
}

std::vector<double> FilterPipeline::processData(const std::vector<double>& input) {
    // TODO: Implement actual data processing through the pipeline
    // This is a placeholder that just returns the input
    return input;
}

std::vector<double> FilterPipeline::getLinearFilterCoefficients() const {
    // TODO: Implement conversion to LinearFilter coefficients
    // This is a placeholder that returns an empty vector
    return {};
}

std::string FilterPipeline::generateCode() const {
    std::stringstream ss;
    ss << "// Generated Filter Pipeline Code\n\n";
    ss << "#include <wpimath/filter/LinearFilter.h>\n\n";
    ss << "class FilterPipeline {\n";
    ss << "public:\n";
    ss << "    FilterPipeline() {\n";
    
    // Generate filter creation code
    for (const auto& node : nodes_) {
        ss << "        // Create " << node.type << " filter\n";
        ss << "        " << node.id << " = LinearFilter::" << node.type << "(";
        
        // Add parameters
        bool first = true;
        for (const auto& [name, value] : node.parameters) {
            if (!first) {
                ss << ", ";
            }
            ss << std::fixed << std::setprecision(6) << value;
            first = false;
        }
        ss << ");\n";
    }
    
    ss << "    }\n\n";
    ss << "    double process(double input) {\n";
    ss << "        double value = input;\n";
    
    // Generate processing code
    for (const auto& node : nodes_) {
        ss << "        value = " << node.id << ".Calculate(value);\n";
    }
    
    ss << "        return value;\n";
    ss << "    }\n\n";
    ss << "private:\n";
    
    // Generate member variables
    for (const auto& node : nodes_) {
        ss << "    LinearFilter " << node.id << ";\n";
    }
    
    ss << "};\n";
    return ss.str();
}

std::vector<PipelineNode> FilterPipeline::getPipelineNodes() const {
    return nodes_;
}

} // namespace pipeline 