#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <functional>
#include <filesystem>

namespace filter {

class InputNode {
public:
    virtual ~InputNode() = default;
    virtual std::vector<double> getData() = 0;
    virtual bool isConnected() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

class LogFileInput : public InputNode {
public:
    enum class FileType {
        CSV,
        WPILOG
    };

    LogFileInput(const std::string& filename, const std::string& columnName);
    ~LogFileInput();

    std::vector<double> getData() override;
    bool isConnected() const override;
    void start() override;
    void stop() override;

private:
    void readLoop();
    bool parseLine(const std::string& line, double& value);
    FileType determineFileType() const;

    std::string filename_;
    std::string columnName_;
    std::ifstream file_;
    std::queue<double> dataBuffer_;
    std::mutex bufferMutex_;
    std::thread readerThread_;
    std::atomic<bool> running_;
    bool connected_;
    FileType fileType_;
    int columnIndex_;
};

class NetworkTableInput : public InputNode {
public:
    NetworkTableInput(const std::string& tableName, const std::string& key);
    ~NetworkTableInput();

    std::vector<double> getData() override;
    bool isConnected() const override;
    void start() override;
    void stop() override;

    // Connection settings
    void setTeamNumber(int teamNumber) { teamNumber_ = teamNumber; }
    void setUseUSB(bool useUSB) { useUSB_ = useUSB; }
    void setIPAddress(const std::string& ipAddress) { ipAddress_ = ipAddress; }

private:
    void updateLoop();
    void onValueChanged(double value);
    std::string getServerAddress() const;

    std::string tableName_;
    std::string key_;
    std::queue<double> dataBuffer_;
    std::mutex bufferMutex_;
    std::thread updateThread_;
    std::atomic<bool> running_;
    bool connected_;
    std::function<void(double)> valueCallback_;

    // Connection settings
    int teamNumber_ = 0;
    bool useUSB_ = true;
    std::string ipAddress_;
};

} // namespace filter 