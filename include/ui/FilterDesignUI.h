#pragma once

#include "../pipeline/FilterPipeline.h"
#include "../data/DataSource.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <vector>

namespace ui {

class FilterDesignUI {
public:
    FilterDesignUI();
    ~FilterDesignUI();

    // Initialize the UI
    bool initialize();
    
    // Main UI loop
    void run();
    
    // Cleanup
    void cleanup();

private:
    // UI Components
    void showMainMenuBar();
    void showDataSourceWindow();
    void showFilterPipelineWindow();
    void showFilterDesignWindow();
    void showFrequencyAnalysisWindow();
    void showPoleZeroPlot();
    void showCodeGeneratorWindow();
    
    // Helper functions
    void loadWpiLogFile(const std::string& filename);
    void connectToNetworkTables(const std::string& server);
    void exportFilterCode();
    void updateFrequencyAnalysis();
    
    // Member variables
    GLFWwindow* window = nullptr;
    std::unique_ptr<pipeline::FilterPipeline> pipeline_;
    std::unique_ptr<data::DataSource> dataSource_;
    bool showDataSourceWindow_ = true;
    bool showFilterPipelineWindow_ = true;
    bool showFilterDesignWindow_ = true;
    bool showFrequencyAnalysisWindow_ = true;
    bool showPoleZeroPlot_ = true;
    bool showCodeGeneratorWindow_ = true;
    
    // Frequency analysis data
    std::vector<double> frequencies_;
    std::vector<std::complex<double>> frequencyResponse_;
    std::vector<std::complex<double>> poles_;
    std::vector<std::complex<double>> zeros_;
};

} // namespace ui 