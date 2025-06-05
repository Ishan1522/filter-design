#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace filter {
    class Filter;
    class InputNode;
}

namespace pipeline {

class FilterPipeline {
public:
    struct PipelineNode {
        std::string id;
        std::string type;
        std::map<std::string, double> parameters;
        std::vector<std::string> inputIds;
        std::vector<std::string> outputIds;
        std::shared_ptr<filter::Filter> filter;
        std::shared_ptr<filter::InputNode> inputNode;
    };

    FilterPipeline() = default;
    ~FilterPipeline() = default;

    // Node management
    std::string addNode(const std::string& type, const std::map<std::string, double>& params);
    void removeNode(const std::string& nodeId);
    bool connectNodes(const std::string& sourceId, const std::string& targetId);
    void disconnectNodes(const std::string& sourceId, const std::string& targetId);
    std::map<std::string, double> getNodeParameters(const std::string& nodeId) const;
    void setNodeParameters(const std::string& nodeId, const std::map<std::string, double>& params);

    // Input node management
    void setInputNode(const std::string& nodeId, std::shared_ptr<filter::InputNode> inputNode);
    std::shared_ptr<filter::InputNode> getInputNode(const std::string& nodeId) const;

    // Data processing
    std::vector<double> processData(const std::vector<double>& input);
    std::vector<double> getLinearFilterCoefficients() const;

    // Code generation
    std::string generateCode() const;

    // Node access
    std::vector<PipelineNode> getPipelineNodes() const;

private:
    std::vector<PipelineNode> nodes_;
};

} // namespace pipeline 