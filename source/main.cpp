#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "../include/ui/FilterDesignUI.hpp"

int main() {
    // Create and initialize the UI
    const auto ui = std::make_unique<ui::FilterDesignUI>();
    if (!ui->initialize()) {
        return 1;
    }

    // Run the main loop
    ui->run();
    return 0;
}
