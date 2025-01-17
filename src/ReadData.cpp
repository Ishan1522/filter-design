#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

// Function to parse .wpilog files
std::vector<std::pair<uint64_t, double>> ParseWPILog(const std::string& filepath) {
    std::vector<std::pair<uint64_t, double>> data;
    std::ifstream file(filepath, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open .wpilog file." << std::endl;
        return data;
    }

    // Example: Parsing a simplistic structure. Customize this for the actual .wpilog format.
    while (file) {
        uint64_t timestamp;
        double value;

        file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
        file.read(reinterpret_cast<char*>(&value), sizeof(value));

        if (file) {
            data.emplace_back(timestamp, value);
        }
    }

    file.close();
    return data;
}

// Function to write data to .wpilog files
void WriteWPILog(const std::string& filepath, const std::vector<std::pair<uint64_t, double>>& data) {
    std::ofstream file(filepath, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open .wpilog file for writing: " + filepath);
    }

    for (const auto& entry : data) {
        file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
        file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
    }
}

// Function to process incoming NetworkTables data
void NetworkTablesExample() {
    auto ntInstance = nt::NetworkTableInstance::GetDefault();
    ntInstance.StartClientTeam(1234); // Replace with your team number

    auto table = ntInstance.GetTable("datatable");

    while (true) {
        double sensorValue = table->GetEntry("sensorValue").GetDouble(0.0);
        std::cout << "Sensor Value: " << sensorValue << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./frc_filter_tool <wpilog_filepath>" << std::endl;
        return 1;
    }

    // Parse the .wpilog file
    std::string wpilogPath = argv[1];
    auto data = ParseWPILog(wpilogPath);

    std::cout << "Parsed .wpilog data:" << std::endl;
    for (const auto& entry : data) {
        std::cout << "Timestamp: " << entry.first << " Value: " << entry.second << std::endl;
    }

    // Start NetworkTables client example
    std::thread networkTablesThread(NetworkTablesExample);

    networkTablesThread.join();

    return 0;
}
