#include "../../include/filter/InputNodes.hpp"
#include <sstream>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <filesystem>

namespace filter {

// LogFileInput implementation
LogFileInput::LogFileInput(const std::string& filename, const std::string& columnName)
    : filename_(filename)
    , columnName_(columnName)
    , running_(false)
    , connected_(false)
    , columnIndex_(-1) {
    fileType_ = determineFileType();
}

LogFileInput::~LogFileInput() {
    stop();
}

std::vector<double> LogFileInput::getData() {
    std::vector<double> data;
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    while (!dataBuffer_.empty()) {
        data.push_back(dataBuffer_.front());
        dataBuffer_.pop();
    }
    
    return data;
}

bool LogFileInput::isConnected() const {
    return connected_;
}

void LogFileInput::start() {
    if (running_) return;
    
    if (fileType_ == FileType::CSV) {
        file_.open(filename_);
        if (!file_.is_open()) {
            std::cerr << "Failed to open log file: " << filename_ << std::endl;
            return;
        }
    } else {
        file_.open(filename_, std::ios::binary);
        if (!file_.is_open()) {
            std::cerr << "Failed to open WPILog file: " << filename_ << std::endl;
            return;
        }
    }
    
    connected_ = true;
    running_ = true;
    readerThread_ = std::thread(&LogFileInput::readLoop, this);
}

void LogFileInput::stop() {
    if (!running_) return;
    
    running_ = false;
    if (readerThread_.joinable()) {
        readerThread_.join();
    }
    
    if (file_.is_open()) {
        file_.close();
    }
    connected_ = false;
}

void LogFileInput::readLoop() {
    if (fileType_ == FileType::CSV) {
        std::string line;
        bool foundHeader = false;
        
        // Read header to find column index
        if (std::getline(file_, line)) {
            std::istringstream iss(line);
            std::string column;
            int index = 0;
            
            while (std::getline(iss, column, ',')) {
                if (column == columnName_) {
                    columnIndex_ = index;
                    foundHeader = true;
                    break;
                }
                index++;
            }
        }
        
        if (!foundHeader) {
            std::cerr << "Column not found in log file: " << columnName_ << std::endl;
            connected_ = false;
            return;
        }
        
        // Read data lines
        while (running_ && std::getline(file_, line)) {
            double value;
            if (parseLine(line, value)) {
                std::lock_guard<std::mutex> lock(bufferMutex_);
                dataBuffer_.push(value);
            }
            
            // Small delay to prevent CPU hogging
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } else {
        // Read magic number
        uint32_t magic;
        file_.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (magic != 0x574C4F47) {  // "WLOG"
            std::cerr << "Invalid WPILog file format" << std::endl;
            connected_ = false;
            return;
        }
        
        // Read version
        uint32_t version;
        file_.read(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Read entry count
        uint32_t entryCount;
        file_.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
        
        // Read entries
        while (running_ && !file_.eof()) {
            uint32_t entryType, entrySize;
            file_.read(reinterpret_cast<char*>(&entryType), sizeof(entryType));
            file_.read(reinterpret_cast<char*>(&entrySize), sizeof(entrySize));
            
            if (file_.eof()) break;
            
            // Read entry data
            std::vector<char> data(entrySize);
            file_.read(data.data(), entrySize);
            
            // Process entry based on type
            if (entryType == 0x01) {  // Double value
                double value;
                std::memcpy(&value, data.data(), sizeof(double));
                std::lock_guard<std::mutex> lock(bufferMutex_);
                dataBuffer_.push(value);
            }
            
            // Small delay to prevent CPU hogging
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

bool LogFileInput::parseLine(const std::string& line, double& value) {
    std::istringstream iss(line);
    std::string token;
    int currentIndex = 0;
    
    while (std::getline(iss, token, ',')) {
        if (currentIndex == columnIndex_) {
            try {
                value = std::stod(token);
                return true;
            } catch (const std::exception& e) {
                return false;
            }
        }
        currentIndex++;
    }
    
    return false;
}

LogFileInput::FileType LogFileInput::determineFileType() const {
    return std::filesystem::path(filename_).extension() == ".wpilog" ? FileType::WPILOG : FileType::CSV;
}

// NetworkTableInput implementation
NetworkTableInput::NetworkTableInput(const std::string& tableName, const std::string& key)
    : tableName_(tableName)
    , key_(key)
    , running_(false)
    , connected_(false) {
}

NetworkTableInput::~NetworkTableInput() {
    stop();
}

std::vector<double> NetworkTableInput::getData() {
    std::vector<double> data;
    std::lock_guard<std::mutex> lock(bufferMutex_);
    
    while (!dataBuffer_.empty()) {
        data.push_back(dataBuffer_.front());
        dataBuffer_.pop();
    }
    
    return data;
}

bool NetworkTableInput::isConnected() const {
    return connected_;
}

void NetworkTableInput::start() {
    if (running_) return;
    
    // Initialize NetworkTable connection
    try {
        // Get server address based on connection type
        std::string serverAddress = getServerAddress();
        
        // TODO: Initialize NetworkTable connection using the server address
        // This is a placeholder for the actual NetworkTable implementation
        // You'll need to integrate with your specific NetworkTable library
        
        connected_ = true;
        running_ = true;
        updateThread_ = std::thread(&NetworkTableInput::updateLoop, this);
    } catch (const std::exception& e) {
        std::cerr << "Failed to connect to NetworkTables server: " << e.what() << std::endl;
        connected_ = false;
    }
}

void NetworkTableInput::stop() {
    if (!running_) return;
    
    running_ = false;
    if (updateThread_.joinable()) {
        updateThread_.join();
    }
    
    connected_ = false;
}

void NetworkTableInput::updateLoop() {
    // TODO: Implement NetworkTable update loop
    // This is a placeholder for the actual NetworkTable implementation
    // You'll need to integrate with your specific NetworkTable library
    
    while (running_) {
        // Simulate receiving data
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void NetworkTableInput::onValueChanged(double value) {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    dataBuffer_.push(value);
}

std::string NetworkTableInput::getServerAddress() const {
    if (useUSB_) {
        // For USB connection, use localhost
        return "localhost";
    } else if (!ipAddress_.empty()) {
        // Use custom IP address if provided
        return ipAddress_;
    } else if (teamNumber_ > 0) {
        // Use team number to construct IP address (10.TE.AM.2)
        int team = teamNumber_;
        int hundreds = team / 100;
        int tens = (team % 100) / 10;
        int ones = team % 10;
        return "10." + std::to_string(hundreds) + "." + std::to_string(tens) + std::to_string(ones) + ".2";
    }
    
    // Default to localhost if no valid connection settings
    return "localhost";
}

} // namespace filter 