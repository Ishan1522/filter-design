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
#include "LogFileParser.hpp"
#include <imgui.h>

namespace filter {

class InputNode {
public:
    virtual ~InputNode() = default;
    virtual bool isConnected() const = 0;
    virtual std::vector<double> getData() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

class LogFileInput : public InputNode {
public:
    LogFileInput(const std::string& filename, const std::string& columnName);
    ~LogFileInput() override = default;

    bool isConnected() const override;
    std::vector<double> getData() const override;
    void start() override;
    void stop() override;

    const std::vector<std::string>& getAvailableFields() const;
    void setColumnName(const std::string& columnName);

private:
    std::string filename_;
    std::string columnName_;
    std::unique_ptr<LogFileParser> parser_;
    std::vector<double> data_;
    bool connected_;
};

class NetworkTableInput : public InputNode {
public:
    NetworkTableInput(const std::string& tableName, const std::string& key);
    ~NetworkTableInput() override = default;

    bool isConnected() const override;
    std::vector<double> getData() const override;
    void start() override;
    void stop() override;

    void setUseUSB(bool useUSB);
    void setTeamNumber(int teamNumber);
    void setIPAddress(const std::string& ipAddress);

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
    std::vector<double> data_;
};

} // namespace filter 