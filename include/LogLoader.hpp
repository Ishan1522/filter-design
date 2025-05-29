#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <wpigui/DataLogReader.h>
#include <wpi/DataLog.h>
#include <wpi/DataLogRecord.h>
#include <wpi/DataLogRecordType.h>

struct LogEntry {
    double timestamp;
    std::map<std::string, double> values;
};

class LogLoader {
public:
    LogLoader() = default;
    ~LogLoader() = default;

    // Load a WPILib log file from the given path
    bool LoadFile(const std::string& filename);

    // Get all log entries
    const std::vector<LogEntry>& GetEntries() const { return entries; }

    // Get available data fields/columns
    const std::vector<std::string>& GetFields() const { return fields; }

    // Clear all loaded data
    void Clear() { entries.clear(); fields.clear(); }

private:
    void ProcessRecord(const wpigui::DataLogReader::Record& record);
    void ProcessControlRecord(const wpigui::DataLogReader::Record& record);
    void ProcessDataRecord(const wpigui::DataLogReader::Record& record);
    // void ProcessRecord(const wpi::log::DataLogRecord& record);
    // void ProcessControlRecord(const wpi::log::StartRecordData& data);
    // void ProcessDataRecord(const wpi::log::DataRecord& data);

    std::vector<LogEntry> entries;
    std::vector<std::string> fields;
    std::map<int, std::string> entryNames;
}; 