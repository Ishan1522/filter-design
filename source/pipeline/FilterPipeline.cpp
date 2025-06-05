#include "../../include/pipeline/FilterPipeline.hpp"
#include "../../include/filter/Filter.hpp"
#include "../../include/filter/InputNodes.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <queue>
#include <unordered_set>

namespace pipeline {

std::string FilterPipeline::addNode(const std::string& type, const std::map<std::string, double>& params) {
    PipelineNode node;
    node.id = "node_" + std::to_string(nodes_.size());
    node.type = type;
    node.parameters = params;
    nodes_.push_back(node);
    return node.id;
}

void FilterPipeline::removeNode(const std::string& nodeId) {
    // Remove the node
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [&](const PipelineNode& node) { return node.id == nodeId; }),
        nodes_.end()
    );

    // Remove connections to/from this node
    for (auto& node : nodes_) {
        node.inputIds.erase(
            std::remove(node.inputIds.begin(), node.inputIds.end(), nodeId),
            node.inputIds.end()
        );
        node.outputIds.erase(
            std::remove(node.outputIds.begin(), node.outputIds.end(), nodeId),
            node.outputIds.end()
        );
    }
}

bool FilterPipeline::connectNodes(const std::string& sourceId, const std::string& targetId) {
    // Find source and target nodes
    auto sourceIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == sourceId; });
    auto targetIt = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == targetId; });

    if (sourceIt == nodes_.end() || targetIt == nodes_.end()) {
        return false;
    }

    // Check if connection already exists
    if (std::find(sourceIt->outputIds.begin(), sourceIt->outputIds.end(), targetId) != sourceIt->outputIds.end()) {
        return false;
    }

    // Add connection
    sourceIt->outputIds.push_back(targetId);
    targetIt->inputIds.push_back(sourceId);
    return true;
}

void FilterPipeline::disconnectNodes(const std::string& sourceId, const std::string& targetId) {
    // Find source and target nodes
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
        sourceIt->outputIds.end()
    );
    targetIt->inputIds.erase(
        std::remove(targetIt->inputIds.begin(), targetIt->inputIds.end(), sourceId),
        targetIt->inputIds.end()
    );
}

std::map<std::string, double> FilterPipeline::getNodeParameters(const std::string& nodeId) const {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    return it != nodes_.end() ? it->parameters : std::map<std::string, double>();
}

void FilterPipeline::setNodeParameters(const std::string& nodeId, const std::map<std::string, double>& params) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    if (it != nodes_.end()) {
        it->parameters = params;
    }
}

void FilterPipeline::setInputNode(const std::string& nodeId, std::shared_ptr<filter::InputNode> inputNode) {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    if (it != nodes_.end()) {
        it->inputNode = inputNode;
    }
}

std::shared_ptr<filter::InputNode> FilterPipeline::getInputNode(const std::string& nodeId) const {
    auto it = std::find_if(nodes_.begin(), nodes_.end(),
        [&](const PipelineNode& node) { return node.id == nodeId; });
    return it != nodes_.end() ? it->inputNode : nullptr;
}

std::vector<double> FilterPipeline::processData(const std::vector<double>& input) {
    if (nodes_.empty()) {
        return input;
    }

    // Find input nodes (nodes with input nodes or no inputs)
    std::vector<std::string> inputNodes;
    for (const auto& node : nodes_) {
        if (node.inputNode || node.inputIds.empty()) {
            inputNodes.push_back(node.id);
        }
    }

    if (inputNodes.empty()) {
        return input;
    }

    // Process data through each input node
    std::vector<double> output = input;
    for (const auto& inputNodeId : inputNodes) {
        // Find the input node
        auto inputNodeIt = std::find_if(nodes_.begin(), nodes_.end(),
            [&](const PipelineNode& node) { return node.id == inputNodeId; });
        
        if (inputNodeIt == nodes_.end()) {
            continue;
        }

        // Get input data from input node if available
        std::vector<double> nodeInput = input;
        if (inputNodeIt->inputNode && inputNodeIt->inputNode->isConnected()) {
            nodeInput = inputNodeIt->inputNode->getData();
        }

        // Process data through the pipeline starting from this input node
        std::queue<std::string> nodeQueue;
        std::unordered_set<std::string> processedNodes;
        nodeQueue.push(inputNodeId);

        while (!nodeQueue.empty()) {
            std::string currentNodeId = nodeQueue.front();
            nodeQueue.pop();

            if (processedNodes.find(currentNodeId) != processedNodes.end()) {
                continue;
            }
            processedNodes.insert(currentNodeId);

            // Find the current node
            auto currentNodeIt = std::find_if(nodes_.begin(), nodes_.end(),
                [&](const PipelineNode& node) { return node.id == currentNodeId; });
            
            if (currentNodeIt == nodes_.end()) {
                continue;
            }

            // Process data through the current node
            if (currentNodeIt->filter) {
                nodeInput = currentNodeIt->filter->processBlock(nodeInput);
            }

            // Add output nodes to the queue
            for (const auto& outputId : currentNodeIt->outputIds) {
                nodeQueue.push(outputId);
            }
        }

        // Combine outputs from different input nodes
        if (output.size() != nodeInput.size()) {
            output = nodeInput;
        } else {
            for (size_t i = 0; i < output.size(); ++i) {
                output[i] += nodeInput[i];
            }
        }
    }

    return output;
}

std::vector<double> FilterPipeline::getLinearFilterCoefficients() const {
    // TODO: Implement conversion of pipeline to linear filter coefficients
    return std::vector<double>();
}

std::string FilterPipeline::generateCode() const {
    std::stringstream ss;
    ss << "// Generated filter pipeline code\n\n";
    
    // Generate node declarations
    for (const auto& node : nodes_) {
        ss << "// Node: " << node.id << " (" << node.type << ")\n";
        ss << "auto " << node.id << " = std::make_shared<" << node.type << ">();\n";
        
        // Set parameters
        for (const auto& param : node.parameters) {
            ss << node.id << "->setParameter(\"" << param.first << "\", " << param.second << ");\n";
        }
        ss << "\n";
    }
    
    // Generate connections
    ss << "// Connections\n";
    for (const auto& node : nodes_) {
        for (const auto& outputId : node.outputIds) {
            ss << "connectNodes(" << node.id << ", " << outputId << ");\n";
        }
    }
    
    return ss.str();
}

std::vector<FilterPipeline::PipelineNode> FilterPipeline::getPipelineNodes() const {
    return nodes_;
}

} // namespace pipeline 