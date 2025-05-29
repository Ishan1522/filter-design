

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "../include/ui/FilterDesignUI.h"

void main() {
    // Create and initialize the UI
    const auto ui = std::make_unique<ui::FilterDesignUI>();
    if (!ui->initialize()) {
        return;
    }

    // Run the main loop
    ui->run();
}
