#include "../../include/ui/FilterDesignUI.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imnodes.h"
#include "implot.h"
#include <cmath>
#include <sstream>
#include <fstream>
#include "pipeline/FilterPipeline.hpp"
#include "filter/Filter.hpp"
#include "filter/ButterworthFilter.hpp"
#include <stdexcept>

#define M_PI 3.14159265358979323846

namespace ui {

FilterDesignUI::FilterDesignUI() : nextNodeId_(1), nextLinkId_(1) {
    pipeline_ = std::make_unique<pipeline::FilterPipeline>();
}

FilterDesignUI::~FilterDesignUI() {
    cleanup();
}

bool FilterDesignUI::initGLFW() {
    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(1280, 720, "Filter Design", nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    return true;
}

bool FilterDesignUI::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window_, true)) {
        return false;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        return false;
    }

    return true;
}

void FilterDesignUI::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImNodes::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void FilterDesignUI::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderMenu();
        renderNodeEditor();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }
}

void FilterDesignUI::renderMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                // TODO: Implement new pipeline
            }
            if (ImGui::MenuItem("Open")) {
                // TODO: Implement open pipeline
            }
            if (ImGui::MenuItem("Save")) {
                // TODO: Implement save pipeline
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Add Node")) {
                // TODO: Show node creation dialog
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void FilterDesignUI::renderNodeEditor() {
    // Set window size and position
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Filter Design", nullptr, ImGuiWindowFlags_MenuBar);

    renderNodeMenu();

    // Create a default node if none exist
    if (nodes_.empty()) {
        createNode(Node::NodeType::Butterworth);
    }

    ImNodes::BeginNodeEditor();

    // Render all nodes
    for (const auto& [id, node] : nodes_) {
        renderNode(id, node.title);
    }

    // Render all links
    for (const auto& [id, link] : links_) {
        renderLink(id, link.fromNode, link.fromPin, link.toNode, link.toPin);
    }

    ImNodes::EndNodeEditor();

    // Handle new links
    int fromNode, fromPin, toNode, toPin;
    if (ImNodes::IsLinkCreated(&fromNode, &fromPin, &toNode, &toPin)) {
        links_[nextLinkId_] = {nextLinkId_, fromNode, fromPin, toNode, toPin};
        nextLinkId_++;
        
        // Process data through the new connection
        processFilters();
    }

    // Handle deleted links
    int linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) {
        links_.erase(linkId);
    }

    ImGui::End();
}

void FilterDesignUI::renderNodeMenu() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Add Node")) {
            if (ImGui::BeginMenu("Input")) {
                if (ImGui::MenuItem("Log File")) createNode(Node::NodeType::LogFileInput);
                if (ImGui::MenuItem("Network Table")) createNode(Node::NodeType::NetworkTableInput);
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Output")) createNode(Node::NodeType::Output);
            if (ImGui::MenuItem("Butterworth")) createNode(Node::NodeType::Butterworth);
            if (ImGui::MenuItem("Chebyshev")) createNode(Node::NodeType::Chebyshev);
            if (ImGui::MenuItem("Notch")) createNode(Node::NodeType::Notch);
            if (ImGui::MenuItem("Band Pass")) createNode(Node::NodeType::BandPass);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void FilterDesignUI::renderNode(int nodeId, const std::string& title) {
    ImNodes::BeginNode(nodeId);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(title.c_str());
    ImNodes::EndNodeTitleBar();

    auto& node = nodes_[nodeId];

    // Render input pins
    for (int i = 0; i < node.inputPins.size(); ++i) {
        ImNodes::BeginInputAttribute(node.inputPins[i]);
        ImGui::Text("Input %d", i);
        ImNodes::EndInputAttribute();
    }

    // Render input parameters for input nodes
    if (node.nodeType == Node::NodeType::LogFileInput || 
        node.nodeType == Node::NodeType::NetworkTableInput) {
        renderInputParameters(nodeId);
    }

    // Render filter parameters for filter nodes
    if (node.filterType != Node::FilterType::None) {
        renderFilterParameters(nodeId);
        renderFrequencyResponse(nodeId);
        renderPoleZeroPlot(nodeId);
        renderCodeExport(nodeId);
    }

    // Render output pins
    for (int i = 0; i < node.outputPins.size(); ++i) {
        ImNodes::BeginOutputAttribute(node.outputPins[i]);
        ImGui::Text("Output %d", i);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

void FilterDesignUI::renderFilterParameters(int nodeId) {
    auto& node = nodes_[nodeId];
    
    if (node.filterType == Node::FilterType::None) {
        return;
    }

    ImGui::Separator();
    ImGui::Text("Filter Parameters");

    // Common parameters
    if (ImGui::DragInt("Order", &node.order, 1, 1, 10)) {
        calculateFilterCoefficients(node);
    }
    if (ImGui::DragFloat("Cutoff (Hz)", &node.ui_cutoffFreq, 1.0f, 1.0f, 20000.0f)) {
        node.cutoffFreq = static_cast<double>(node.ui_cutoffFreq);
        calculateFilterCoefficients(node);
    }
    if (ImGui::DragFloat("Sample Rate (Hz)", &node.ui_sampleRate, 1.0f, 1000.0f, 192000.0f)) {
        node.sampleRate = static_cast<double>(node.ui_sampleRate);
        calculateFilterCoefficients(node);
    }

    // Type-specific parameters
    switch (node.filterType) {
        case Node::FilterType::Chebyshev:
            if (ImGui::DragFloat("Ripple (dB)", &node.ui_ripple, 0.1f, 0.1f, 10.0f)) {
                node.ripple = static_cast<double>(node.ui_ripple);
                calculateFilterCoefficients(node);
            }
            break;
        case Node::FilterType::Notch:
        if (ImGui::DragFloat("Frequency (Hz)", &node.ui_bandwidth, 1.0f, 1.0f, 10000.0f)) {
            node.bandwidth = static_cast<double>(node.ui_bandwidth);
            calculateFilterCoefficients(node);
        } break;
        case Node::FilterType::BandPass:
            if (ImGui::DragFloat("Bandwidth (Hz)", &node.ui_bandwidth, 1.0f, 1.0f, 10000.0f)) {
                node.bandwidth = static_cast<double>(node.ui_bandwidth);
                calculateFilterCoefficients(node);
            }
            break;
        default:
            break;
    }
}

void FilterDesignUI::renderFrequencyResponse(int nodeId) {
    auto& node = nodes_[nodeId];
    
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Frequency Response")) {
        if (ImPlot::BeginPlot("Frequency Response Plot", ImVec2(-1, 300))) {
            // Set up the plot
            ImPlot::SetupAxes("Frequency (Hz)", "Magnitude (dB)", ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisLimits(ImAxis_X1, -1.5, 1.5);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1.5, 1.5);

            // Plot poles
            std::vector<float> poleX, poleY;
            for (const auto& pole : node.poles) {
                poleX.push_back(static_cast<float>(pole.real()));
                poleY.push_back(static_cast<float>(pole.imag()));
            }
            if (!poleX.empty()) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8, ImVec4(1,0,0,1), 2.0f);
                ImPlot::PlotScatter("Poles", poleX.data(), poleY.data(), poleX.size());
            }

            // Plot zeros
            std::vector<float> zeroX, zeroY;
            for (const auto& zero : node.zeros) {
                zeroX.push_back(static_cast<float>(zero.real()));
                zeroY.push_back(static_cast<float>(zero.imag()));
            }
            if (!zeroX.empty()) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8, ImVec4(0,0,1,1), 2.0f, ImVec4(1,1,1,1));
                ImPlot::PlotScatter("Zeros", zeroX.data(), zeroY.data(), zeroX.size());
            }

            ImPlot::EndPlot();
        }
    }
}

void FilterDesignUI::renderPoleZeroPlot(int nodeId) {
    auto& node = nodes_[nodeId];
    
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Pole-Zero Plot")) {
        if (ImPlot::BeginPlot("Pole-Zero Plot", ImVec2(-1, 300))) {
            // Set up the plot
            ImPlot::SetupAxes("Real", "Imaginary", ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisLimits(ImAxis_X1, -1.5, 1.5);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1.5, 1.5);
            
            // Draw unit circle
            const int numPoints = 100;
            std::vector<float> circleX(numPoints), circleY(numPoints);
            for (int i = 0; i < numPoints; ++i) {
                float angle = 2.0f * M_PI * i / (numPoints - 1);
                circleX[i] = std::cos(angle);
                circleY[i] = std::sin(angle);
            }
            ImPlot::PlotLine("Unit Circle", circleX.data(), circleY.data(), numPoints);
            
            // Plot poles
            std::vector<float> poleX, poleY;
            for (const auto& pole : node.poles) {
                poleX.push_back(static_cast<float>(pole.real()));
                poleY.push_back(static_cast<float>(pole.imag()));
            }
            if (!poleX.empty()) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8, ImVec4(1,0,0,1), 2.0f);
                ImPlot::PlotScatter("Poles", poleX.data(), poleY.data(), poleX.size());
            }
            
            // Plot zeros
            std::vector<float> zeroX, zeroY;
            for (const auto& zero : node.zeros) {
                zeroX.push_back(static_cast<float>(zero.real()));
                zeroY.push_back(static_cast<float>(zero.imag()));
            }
            if (!zeroX.empty()) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8, ImVec4(0,0,1,1), 2.0f, ImVec4(1,1,1,1));
                ImPlot::PlotScatter("Zeros", zeroX.data(), zeroY.data(), zeroX.size());
            }
            
            ImPlot::EndPlot();
        }
    }
}

void FilterDesignUI::renderCodeExport(int nodeId) {
    auto& node = nodes_[nodeId];
    
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Export Code")) {
        std::string code = generateLinearFilterCode(node);
        
        ImGui::InputTextMultiline("##code", const_cast<char*>(code.c_str()), 
            code.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10),
            ImGuiInputTextFlags_ReadOnly);
            
        if (ImGui::Button("Copy to Clipboard")) {
            exportToClipboard(code);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Save to File")) {
            // TODO: Add file dialog
            exportToFile(code, "filter_" + std::to_string(nodeId) + ".cpp");
        }
    }
}

bool FilterDesignUI::openFileDialog(std::string& outPath) {
    static char filename[256] = "";
    static bool showDialog = true;
    
    if (showDialog) {
        ImGui::OpenPopup("Select File");
        showDialog = false;
    }
    
    if (ImGui::BeginPopupModal("Select File", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Filename", filename, sizeof(filename));
        
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            outPath = filename;
            ImGui::CloseCurrentPopup();
            showDialog = true;
            return true;
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            showDialog = true;
            return false;
        }
        
        ImGui::EndPopup();
    }
    
    return false;
}

void FilterDesignUI::renderInputParameters(int nodeId) {
    auto& node = nodes_[nodeId];
    
    ImGui::Separator();
    ImGui::Text("Input Parameters");

    if (node.nodeType == Node::NodeType::LogFileInput) {
        char filename[256] = {0};
        strncpy(filename, node.logFilename.c_str(), sizeof(filename) - 1);
        if (ImGui::InputText("Log File", filename, sizeof(filename))) {
            node.logFilename = filename;
            if (!node.logFilename.empty()) {
                auto inputNode = std::make_shared<filter::LogFileInput>(node.logFilename, node.logColumnName);
                inputNode->start();
                pipeline_->setInputNode(node.pipelineNodeId, inputNode);
            }
        }

        char column[256] = {0};
        strncpy(column, node.logColumnName.c_str(), sizeof(column) - 1);
        if (ImGui::InputText("Column Name", column, sizeof(column))) {
            node.logColumnName = column;
            if (!node.logFilename.empty() && !node.logColumnName.empty()) {
                auto inputNode = std::make_shared<filter::LogFileInput>(node.logFilename, node.logColumnName);
                inputNode->start();
                pipeline_->setInputNode(node.pipelineNodeId, inputNode);
            }
        }

        if (ImGui::Button("Browse...")) {
            std::string selectedPath;
            if (openFileDialog(selectedPath)) {
                node.logFilename = selectedPath;
                if (!node.logFilename.empty() && !node.logColumnName.empty()) {
                    auto inputNode = std::make_shared<filter::LogFileInput>(node.logFilename, node.logColumnName);
                    inputNode->start();
                    pipeline_->setInputNode(node.pipelineNodeId, inputNode);
                }
            }
        }
    }
    else if (node.nodeType == Node::NodeType::NetworkTableInput) {
        char table[256] = {0};
        strncpy(table, node.networkTableName.c_str(), sizeof(table) - 1);
        if (ImGui::InputText("Table Name", table, sizeof(table))) {
            node.networkTableName = table;
            if (!node.networkTableName.empty() && !node.networkTableKey.empty()) {
                auto inputNode = std::make_shared<filter::NetworkTableInput>(node.networkTableName, node.networkTableKey);
                inputNode->start();
                pipeline_->setInputNode(node.pipelineNodeId, inputNode);
            }
        }

        char key[256] = {0};
        strncpy(key, node.networkTableKey.c_str(), sizeof(key) - 1);
        if (ImGui::InputText("Key", key, sizeof(key))) {
            node.networkTableKey = key;
            if (!node.networkTableName.empty() && !node.networkTableKey.empty()) {
                auto inputNode = std::make_shared<filter::NetworkTableInput>(node.networkTableName, node.networkTableKey);
                inputNode->start();
                pipeline_->setInputNode(node.pipelineNodeId, inputNode);
            }
        }

        // Connection settings
        ImGui::Separator();
        ImGui::Text("Connection Settings");

        // Connection type selection
        static int connectionType = 0;
        if (ImGui::RadioButton("USB", &connectionType, 0)) {
            if (auto inputNode = pipeline_->getInputNode(node.pipelineNodeId)) {
                if (auto* ntInput = dynamic_cast<filter::NetworkTableInput*>(inputNode.get())) {
                    ntInput->setUseUSB(true);
                    ntInput->stop();
                    ntInput->start();
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("IP Address", &connectionType, 1)) {
            if (auto inputNode = pipeline_->getInputNode(node.pipelineNodeId)) {
                if (auto* ntInput = dynamic_cast<filter::NetworkTableInput*>(inputNode.get())) {
                    ntInput->setUseUSB(false);
                    ntInput->stop();
                    ntInput->start();
                }
            }
        }

        if (connectionType == 0) {
            // USB connection - no additional settings needed
        } else {
            // IP Address connection
            static int teamNumber = 0;
            static char ipAddress[16] = "10.0.0.2";
            
            if (ImGui::CollapsingHeader("Team Number")) {
                if (ImGui::InputInt("Team Number", &teamNumber)) {
                    if (auto inputNode = pipeline_->getInputNode(node.pipelineNodeId)) {
                        if (auto* ntInput = dynamic_cast<filter::NetworkTableInput*>(inputNode.get())) {
                            ntInput->setTeamNumber(teamNumber);
                            ntInput->stop();
                            ntInput->start();
                        }
                    }
                }
            }
            
            if (ImGui::CollapsingHeader("Custom IP")) {
                char ipAddress[16] = {0};
                if (!node.networkTableIP.empty()) {
                    strncpy(ipAddress, node.networkTableIP.c_str(), sizeof(ipAddress) - 1);
                }
                if (ImGui::InputText("IP Address", ipAddress, sizeof(ipAddress))) {
                    node.networkTableIP = ipAddress;
                    if (auto inputNode = pipeline_->getInputNode(node.pipelineNodeId)) {
                        if (auto* ntInput = dynamic_cast<filter::NetworkTableInput*>(inputNode.get())) {
                            ntInput->setIPAddress(node.networkTableIP);
                            ntInput->stop();
                            ntInput->start();
                        }
                    }
                }
            }
        }
    }

    // Show connection status
    if (auto inputNode = pipeline_->getInputNode(node.pipelineNodeId)) {
        ImGui::Text("Status: %s", inputNode->isConnected() ? "Connected" : "Disconnected");
    }
}

void FilterDesignUI::processFilters() {
    // Update pipeline nodes and connections
    for (auto& [id, node] : nodes_) {
        updatePipelineNode(node);
    }
    updatePipelineConnections();

    // Process data through the pipeline
    std::vector<double> output = pipeline_->processData(std::vector<double>());
    
    // Update node outputs
    for (auto& [id, node] : nodes_) {
        if (node.nodeType == Node::NodeType::LogFileInput || 
            node.nodeType == Node::NodeType::NetworkTableInput) {
            node.outputData = output;
        }
    }
}

void FilterDesignUI::updatePipelineNode(Node& node) const {
    std::map<std::string, double> params;
    
    // Add common parameters
    params["order"] = static_cast<double>(node.order);
    params["cutoffFreq"] = node.cutoffFreq;
    params["sampleRate"] = node.sampleRate;
    
    // Add type-specific parameters
    switch (node.filterType) {
        case Node::FilterType::Chebyshev:
            params["ripple"] = node.ripple;
            break;
        case Node::FilterType::Notch:
        case Node::FilterType::BandPass:
            params["bandwidth"] = node.bandwidth;
            break;
        default:
            break;
    }

    // TODO: Make function for this
    // Convert node type to string
    std::string type;
    switch (node.filterType) {
        case Node::FilterType::Butterworth:
            type = "Butterworth";
            break;
        case Node::FilterType::Chebyshev:
            type = "Chebyshev";
            break;
        case Node::FilterType::Notch:
            type = "Notch";
            break;
        case Node::FilterType::BandPass:
            type = "BandPass";
            break;
        default:
            return;  // Skip non-filter nodes
    }
    
    // Create or update pipeline node
    if (node.pipelineNodeId.empty()) {
        node.pipelineNodeId = pipeline_->addNode(type, params);
    } else {
        pipeline_->setNodeParameters(node.pipelineNodeId, params);
    }
}

void FilterDesignUI::updatePipelineConnections() {
    // Clear all existing connections
    for (const auto& [id, node] : nodes_) {
        if (!node.pipelineNodeId.empty()) {
            for (const auto& [linkId, link] : links_) {
                if (link.fromNode == id) {
                    pipeline_->disconnectNodes(node.pipelineNodeId, nodes_[link.toNode].pipelineNodeId);
                }
            }
        }
    }
    
    // Add new connections
    for (const auto& [linkId, link] : links_) {
        const auto& fromNode = nodes_[link.fromNode];
        const auto& toNode = nodes_[link.toNode];
        
        if (!fromNode.pipelineNodeId.empty() && !toNode.pipelineNodeId.empty()) {
            pipeline_->connectNodes(fromNode.pipelineNodeId, toNode.pipelineNodeId);
        }
    }
}

void FilterDesignUI::calculateFilterCoefficients(Node& node) {
    // Reset coefficients
    node.b.clear();
    node.a.clear();
    node.poles.clear();
    node.zeros.clear();

    // Calculate normalized cutoff frequency
    double wc = 2.0 * M_PI * node.cutoffFreq / node.sampleRate;

    switch (node.filterType) {
        case Node::FilterType::Butterworth: {
            // Calculate poles for Butterworth filter
            for (int k = 0; k < node.order; ++k) {
                double angle = M_PI * (2.0 * k + 1) / (2.0 * node.order);
                std::complex<double> pole = std::exp(std::complex<double>(0, angle));
                node.poles.push_back(wc * pole);
            }

            // Convert poles to coefficients
            node.a.resize(node.order + 1);
            node.b.resize(node.order + 1);
            std::fill(node.a.begin(), node.a.end(), 0.0);
            std::fill(node.b.begin(), node.b.end(), 0.0);
            
            node.a[0] = 1.0;
            for (size_t i = 0; i < node.poles.size(); ++i) {
                node.a[i + 1] = -std::real(node.poles[i]);
            }

            // Normalize coefficients
            double gain = 1.0;
            for (const auto& pole : node.poles) {
                gain *= std::abs(pole);
            }
            for (double& b : node.b) {
                b /= gain;
            }
            break;
        }
        // TODO: Add other filter types
        default:
            break;
    }

    // Calculate frequency response and pole-zero plot
    calculateFrequencyResponse(node);
    calculatePoleZero(node);
}

double FilterDesignUI::processSample(Node& node, double input) {
    // Direct form II implementation
    double w = input;
    for (size_t i = 1; i < node.a.size(); ++i) {
        w -= node.a[i] * node.yHistory[i - 1];
    }
    
    double y = 0.0;
    for (size_t i = 0; i < node.b.size(); ++i) {
        y += node.b[i] * node.xHistory[i];
    }
    
    // Update state
    for (size_t i = node.xHistory.size() - 1; i > 0; --i) {
        node.xHistory[i] = node.xHistory[i - 1];
    }
    node.xHistory[0] = w;
    
    for (size_t i = node.yHistory.size() - 1; i > 0; --i) {
        node.yHistory[i] = node.yHistory[i - 1];
    }
    node.yHistory[0] = y;
    
    return y;
}

void FilterDesignUI::calculateFrequencyResponse(Node& node) {
    // TODO: Implement frequency response calculation
}

void FilterDesignUI::calculatePoleZero(Node& node) {
    // TODO: Implement pole-zero calculation
    // node.zeros.clear();
}

std::string FilterDesignUI::generateLinearFilterCode(const Node& node) {
    std::stringstream ss;
    ss << "#include <frc/filter/LinearFilter.h>\n\n";
    ss << "// Generated filter coefficients\n";
    ss << "frc::LinearFilter<double> filter({\n";
    
    // Output numerator coefficients
    ss << "    // Numerator coefficients (b)\n";
    for (size_t i = 0; i < node.b.size(); ++i) {
        ss << "    " << node.b[i];
        if (i < node.b.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "}, {\n";
    
    // Output denominator coefficients
    ss << "    // Denominator coefficients (a)\n";
    for (size_t i = 0; i < node.a.size(); ++i) {
        ss << "    " << node.a[i];
        if (i < node.a.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "});\n";
    
    return ss.str();
}

void FilterDesignUI::exportToClipboard(const std::string& code) {
    ImGui::SetClipboardText(code.c_str());
}

void FilterDesignUI::exportToFile(const std::string& code, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << code;
        file.close();
    }
}

void FilterDesignUI::createNode(Node::NodeType type) {
    Node node;
    node.id = nextNodeId_++;
    node.nodeType = type;
    
    switch (type) {
        case Node::NodeType::LogFileInput:
            node.title = "Log File Input";
            node.outputPins.push_back(nextNodeId_++);
            break;
        case Node::NodeType::NetworkTableInput:
            node.title = "Network Table Input";
            node.outputPins.push_back(nextNodeId_++);
            break;
        case Node::NodeType::Output:
            node.title = "Output";
            node.inputPins.push_back(nextNodeId_++);
            break;
        case Node::NodeType::Butterworth:
            node.title = "Butterworth";
            node.filterType = Node::FilterType::Butterworth;
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            // Initialize UI parameters
            node.ui_cutoffFreq = static_cast<float>(node.cutoffFreq);
            node.ui_sampleRate = static_cast<float>(node.sampleRate);
            calculateFilterCoefficients(node);
            break;
        case Node::NodeType::Chebyshev:
            node.title = "Chebyshev";
            node.filterType = Node::FilterType::Chebyshev;
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            // Initialize UI parameters
            node.ui_cutoffFreq = static_cast<float>(node.cutoffFreq);
            node.ui_sampleRate = static_cast<float>(node.sampleRate);
            node.ui_ripple = static_cast<float>(node.ripple);
            calculateFilterCoefficients(node);
            break;
        case Node::NodeType::Notch:
            node.title = "Notch";
            node.filterType = Node::FilterType::Notch;
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            // Initialize UI parameters
            node.ui_cutoffFreq = static_cast<float>(node.cutoffFreq);
            node.ui_sampleRate = static_cast<float>(node.sampleRate);
            node.ui_bandwidth = static_cast<float>(node.bandwidth);
            calculateFilterCoefficients(node);
            break;
        case Node::NodeType::BandPass:
            node.title = "Band Pass";
            node.filterType = Node::FilterType::BandPass;
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            // Initialize UI parameters
            node.ui_cutoffFreq = static_cast<float>(node.cutoffFreq);
            node.ui_sampleRate = static_cast<float>(node.sampleRate);
            node.ui_bandwidth = static_cast<float>(node.bandwidth);
            calculateFilterCoefficients(node);
            break;
    }

    nodes_[node.id] = std::move(node);
    updatePipelineNode(nodes_[node.id]);
}

void FilterDesignUI::deleteNode(int nodeId) {
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end() && !it->second.pipelineNodeId.empty()) {
        pipeline_->removeNode(it->second.pipelineNodeId);
    }
    nodes_.erase(nodeId);
}

void FilterDesignUI::deleteLink(int linkId) {
    auto it = links_.find(linkId);
    if (it != links_.end()) {
        const auto& fromNode = nodes_[it->second.fromNode];
        const auto& toNode = nodes_[it->second.toNode];
        
        if (!fromNode.pipelineNodeId.empty() && !toNode.pipelineNodeId.empty()) {
            pipeline_->disconnectNodes(fromNode.pipelineNodeId, toNode.pipelineNodeId);
        }
    }
    links_.erase(linkId);
}

void FilterDesignUI::renderLink(int linkId, int fromNode, int fromPin, int toNode, int toPin) {
    ImNodes::Link(linkId, fromPin, toPin);
}

bool FilterDesignUI::initialize() {
    if (!initGLFW()) {
        return false;
    }
    if (!initImGui()) {
        return false;
    }
    return true;
}

} // namespace ui 