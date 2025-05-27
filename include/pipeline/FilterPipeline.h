#pragma once

#include "../filter/FilterBase.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace pipeline {

struct PipelineNode {
    std::string id;
    std::string type;
    std::vector<std::string> inputIds;
    std::vector<std::string> outputIds;
    std::shared_ptr<filter::FilterBase> filter;
    std::map<std::string, double> parameters;
};

class FilterPipeline {
public:
    FilterPipeline();
    ~FilterPipeline();

    // Add a node to the pipeline
    std::string addNode(const std::string& type, const std::map<std::string, double>& params);
    
    // Remove a node from the pipeline
    void removeNode(const std::string& nodeId);
    
    // Connect two nodes
    bool connectNodes(const std::string& sourceId, const std::string& targetId);
    
    // Disconnect two nodes
    void disconnectNodes(const std::string& sourceId, const std::string& targetId);
    
    // Get node parameters
    std::map<std::string, double> getNodeParameters(const std::string& nodeId) const;
    
    // Set node parameters
    void setNodeParameters(const std::string& nodeId, const std::map<std::string, double>& params);
    
    // Process data through the pipeline
    std::vector<double> processData(const std::vector<double>& input);
    
    // Get pipeline as LinearFilter coefficients
    std::vector<double> getLinearFilterCoefficients() const;
    
    // Generate code for the pipeline
    std::string generateCode() const;
    
    // Get pipeline visualization data
    std::vector<PipelineNode> getPipelineNodes() const;

private:
    std::vector<PipelineNode> nodes_;
};

} // namespace pipeline 