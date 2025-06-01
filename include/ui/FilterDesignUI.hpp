#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <complex>
#include <GLFW/glfw3.h>
#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_glfw.h"
#include "../../imgui/backends/imgui_impl_opengl3.h"
#include "../../imnodes/imnodes.h"
#include "../../implot/implot.h"
#include "../filter/Filter.hpp"
#include "../pipeline/FilterPipeline.hpp"

namespace pipeline {
    class FilterPipeline;
}

namespace ui {

class FilterDesignUI {
public:
    FilterDesignUI();
    ~FilterDesignUI();

    bool initialize();
    void run();

private:

    struct Node {
        enum class FilterType {
            None,
            Butterworth,
            Chebyshev,
            Notch,
            BandPass
        };

        int id;
        std::string title;
        FilterType filterType = FilterType::None;
        std::vector<int> inputPins;
        std::vector<int> outputPins;
        std::string pipelineNodeId;
        
        // Filter parameters
        int order = 2;
        double cutoffFreq = 1000.0;
        double sampleRate = 44100.0;
        double ripple = 1.0;
        double bandwidth = 100.0;
        
        // UI parameters
        float ui_cutoffFreq = static_cast<float>(cutoffFreq);
        float ui_sampleRate = static_cast<float>(sampleRate);
        float ui_ripple = static_cast<float>(ripple);
        float ui_bandwidth = static_cast<float>(bandwidth);
        
        // Filter coefficients and state
        std::vector<double> b;  // Numerator coefficients
        std::vector<double> a;  // Denominator coefficients
        std::vector<std::complex<double>> poles;
        std::vector<std::complex<double>> zeros;
        std::vector<double> xHistory;  // Input history
        std::vector<double> yHistory;  // Output history
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

    enum class NodeType {
        Input,
        Output,
        Butterworth,
        Chebyshev,
        Notch,
        BandPass
    };

    bool initGLFW();
    bool initImGui();
    void cleanup();
    void renderMenu();
    void renderNodeEditor();
    void renderNodeMenu();
    void renderNode(int nodeId, const std::string& title);
    void renderFilterParameters(int nodeId);
    void renderFrequencyResponse(int nodeId);
    void renderPoleZeroPlot(int nodeId);
    void renderCodeExport(int nodeId);
    void processFilters();
    void updatePipelineNode(Node& node) const;
    void updatePipelineConnections();
    void calculateFilterCoefficients(Node& node);
    double processSample(Node& node, double input);
    std::vector<double> processBlock(Node& node, const std::vector<double>& input);
    void calculateFrequencyResponse(Node& node);
    void calculatePoleZero(Node& node);
    std::string generateLinearFilterCode(const Node& node);
    void exportToClipboard(const std::string& code);
    void exportToFile(const std::string& code, const std::string& filename);
    void createNode(NodeType type);
    void deleteNode(int nodeId);
    void deleteLink(int linkId);
    void renderLink(int linkId, int fromNode, int fromPin, int toNode, int toPin);


    GLFWwindow* window_ = nullptr;
    std::map<int, Node> nodes_;
    std::map<int, Link> links_;
    int nextNodeId_ = 1;
    int nextLinkId_ = 1;
    std::unique_ptr<pipeline::FilterPipeline> pipeline_;
};

} // namespace ui 