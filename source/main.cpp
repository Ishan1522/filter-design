

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "ui/FilterDesignUI.h"

int main(int argc, char** argv) {
    // Create and initialize the UI
    auto ui = std::make_unique<ui::FilterDesignUI>();
    if (!ui->initialize()) {
        return 1;
    }

    // Run the main loop
    ui->run();

    return 0;
}
