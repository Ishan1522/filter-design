#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../imgui/imgui.h"
#include "../../imnodes/imnodes.h"

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

    // Node data
    struct Node {
        int id;
        std::string title;
        std::vector<int> inputPins;
        std::vector<int> outputPins;
        float value = 0.0f;
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

    // Node types
    enum class NodeType {
        Input,
        Output,
        LowPass,
        HighPass,
        BandPass,
        Gain
    };

    void createNode(NodeType type);
    void deleteNode(int nodeId);
    void deleteLink(int linkId);
};

} // namespace ui 