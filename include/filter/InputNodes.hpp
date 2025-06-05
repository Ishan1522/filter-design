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
#include <portable-file-dialogs.h>
#include <wpi/MemoryBuffer.h>
#include <wpi/DataLogReader.h>
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
    enum class FileType {
        CSV,
        WPILOG
    };

    LogFileInput(const std::string& filename, const std::string& columnName);
    ~LogFileInput() override = default;

    bool isConnected() const override;
    std::vector<double> getData() const override;
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
    std::unique_ptr<wpi::DataLogReader> reader_;
    std::vector<double> data_;
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

class LogLoader {
public:
    LogLoader();
    ~LogLoader() = default;

    void Display();
    bool IsFileLoaded() const { return reader_ != nullptr; }
    std::vector<double> GetData(const std::string& entryName) const;

private:
    void RebuildEntryTree();
    void DisplayEntryTree(const std::vector<EntryTreeNode>& tree);
    static void EmitEntry(const std::string& name, const wpi::DataLogReaderEntry& entry);

    struct EntryTreeNode {
        std::string name;
        std::string path;
        const wpi::DataLogReaderEntry* entry = nullptr;
        std::vector<EntryTreeNode> children;
    };

    std::unique_ptr<pfd::open_file> opener_;
    std::string filename_;
    std::string error_;
    std::unique_ptr<wpi::DataLogReader> reader_;
    std::vector<EntryTreeNode> entryTree_;
    std::string filter_;
};

} // namespace filter 