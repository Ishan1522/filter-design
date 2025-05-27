#include "FilterDesignUI.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "../../imgui/backends/imgui_impl_glfw.h"
#include "../../imgui/backends/imgui_impl_opengl3.h"

namespace ui {

FilterDesignUI::FilterDesignUI() = default;

FilterDesignUI::~FilterDesignUI() {
    cleanup();
}

bool FilterDesignUI::initialize() {
    if (!initializeGLFW()) {
        return false;
    }
    if (!initializeImGui()) {
        return false;
    }
    return true;
}

bool FilterDesignUI::initializeGLFW() {
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

bool FilterDesignUI::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

void FilterDesignUI::renderNodeEditor() {
    ImGui::Begin("Filter Design");

    renderNodeMenu();

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
            if (ImGui::MenuItem("Input")) createNode(NodeType::Input);
            if (ImGui::MenuItem("Output")) createNode(NodeType::Output);
            if (ImGui::MenuItem("Low Pass")) createNode(NodeType::LowPass);
            if (ImGui::MenuItem("High Pass")) createNode(NodeType::HighPass);
            if (ImGui::MenuItem("Band Pass")) createNode(NodeType::BandPass);
            if (ImGui::MenuItem("Gain")) createNode(NodeType::Gain);
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

    const auto& node = nodes_[nodeId];

    // Render input pins
    for (int i = 0; i < node.inputPins.size(); ++i) {
        ImNodes::BeginInputAttribute(node.inputPins[i]);
        ImGui::Text("Input %d", i);
        ImNodes::EndInputAttribute();
    }

    // Render output pins
    for (int i = 0; i < node.outputPins.size(); ++i) {
        ImNodes::BeginOutputAttribute(node.outputPins[i]);
        ImGui::Text("Output %d", i);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
}

void FilterDesignUI::renderLink(int linkId, int fromNode, int fromPin, int toNode, int toPin) {
    ImNodes::Link(linkId, fromPin, toPin);
}

void FilterDesignUI::createNode(NodeType type) {
    Node node;
    node.id = nextNodeId_++;
    
    switch (type) {
        case NodeType::Input:
            node.title = "Input";
            node.outputPins.push_back(nextNodeId_++);
            break;
        case NodeType::Output:
            node.title = "Output";
            node.inputPins.push_back(nextNodeId_++);
            break;
        case NodeType::LowPass:
            node.title = "Low Pass";
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            break;
        case NodeType::HighPass:
            node.title = "High Pass";
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            break;
        case NodeType::BandPass:
            node.title = "Band Pass";
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            break;
        case NodeType::Gain:
            node.title = "Gain";
            node.inputPins.push_back(nextNodeId_++);
            node.outputPins.push_back(nextNodeId_++);
            break;
    }

    nodes_[node.id] = node;
}

void FilterDesignUI::deleteNode(int nodeId) {
    nodes_.erase(nodeId);
}

void FilterDesignUI::deleteLink(int linkId) {
    links_.erase(linkId);
}

} // namespace ui 