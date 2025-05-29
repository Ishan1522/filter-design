#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <complex>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imnodes.h"
#include "../pipeline/FilterPipeline.h"

namespace ui {

class FilterDesignUI {
public:
    FilterDesignUI();
    ~FilterDesignUI();

    bool initialize();
    void run();

private:
    // Window management
    GLFWwindow* window_ = nullptr;
    bool initializeGLFW();
    bool initializeImGui();
    void cleanup();

    // Node editor state
    void renderNodeEditor();
    void renderNodeMenu();
    void renderNode(int nodeId, const std::string& title);
    void renderLink(int linkId, int fromNode, int fromPin, int toNode, int toPin);
    void renderFilterParameters(int nodeId);
    void renderFrequencyResponse(int nodeId);
    void renderPoleZeroPlot(int nodeId);
    void renderCodeExport(int nodeId);

    // Node data
    struct Node {
        int id;
        std::string title;
        std::vector<int> inputPins;
        std::vector<int> outputPins;
        std::string pipelineNodeId;  // ID in the FilterPipeline
        
        // Filter parameters
        enum class FilterType {
            None,
            Input,
            Output,
            Butterworth,
            Chebyshev,
            Notch,
            BandPass
        } filterType = FilterType::None;
        
        // Filter coefficients
        std::vector<double> b;  // Numerator coefficients
        std::vector<double> a;  // Denominator coefficients
        std::vector<std::complex<double>> poles;
        std::vector<std::complex<double>> zeros;
        
        // Filter parameters (double for calculations)
        int order = 2;
        double cutoffFreq = 1000.0;
        double sampleRate = 44100.0;
        double ripple = 1.0;  // For Chebyshev
        double bandwidth = 100.0;  // For bandpass/notch
        
        // UI parameters (float for ImGui controls)
        float ui_cutoffFreq = 1000.0f;
        float ui_sampleRate = 44100.0f;
        float ui_ripple = 1.0f;
        float ui_bandwidth = 100.0f;
        
        // State variables for processing
        std::vector<double> xHistory;
        std::vector<double> yHistory;
        
        // Input/output data
        std::vector<double> inputData;
        std::vector<double> outputData;
    };

    struct Link {
        int id;
        int fromNode;
        int fromPin;
        int toNode;
        int toPin;
    };

    std::unordered_map<int, Node> nodes_;
    std::unordered_map<int, Link> links_;
    int nextNodeId_ = 1;
    int nextLinkId_ = 1;

    // Filter pipeline
    std::unique_ptr<pipeline::FilterPipeline> pipeline_;

    // Node types
    enum class NodeType {
        Input,
        Output,
        Butterworth,
        Chebyshev,
        Notch,
        BandPass
    };

    // Filter processing
    void processFilters();
    void calculateFilterCoefficients(Node& node);
    double processSample(Node& node, double input);
    std::vector<double> processBlock(Node& node, const std::vector<double>& input);
    
    // Frequency analysis
    void calculateFrequencyResponse(Node& node);
    void calculatePoleZero(Node& node);
    
    // Code generation
    std::string generateLinearFilterCode(const Node& node);
    void exportToClipboard(const std::string& code);
    void exportToFile(const std::string& code, const std::string& filename);

    void createNode(NodeType type);
    void deleteNode(int nodeId);
    void deleteLink(int linkId);
    void updatePipelineNode(Node& node) const;
    void updatePipelineConnections();
};

} // namespace ui 