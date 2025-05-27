# Filter Design Application

A node-based filter design application using ImGui and ImNodes.

## Features

- Node-based filter design interface
- Support for various filter types:
  - Low-pass filter
  - High-pass filter
  - Band-pass filter
  - Gain control
- Real-time filter visualization
- Export filter coefficients

## Building

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- vcpkg package manager

### Build Steps

1. Clone the repository:
```bash
git clone https://github.com/yourusername/filter-design.git
cd filter-design
```

2. Install dependencies using vcpkg:
```bash
vcpkg install
```

3. Configure and build:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

1. Run the application:
```bash
./filter-design
```

2. Use the "Add Node" menu to add filter nodes to the canvas
3. Connect nodes by dragging from output pins to input pins
4. Adjust filter parameters using the node controls
5. Export filter coefficients using the "Export" menu

## License

MIT License 