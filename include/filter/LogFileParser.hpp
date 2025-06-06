#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>

namespace filter {

struct LogEntry {
    double timestamp;
    std::map<std::string, double> values;
};

class LogFileParser {
public:
    LogFileParser() = default;
    ~LogFileParser() = default;

    // Load a log file from the given path
    bool loadFile(const std::string& filename);

    // Get all log entries
    const std::vector<LogEntry>& getEntries() const { return entries_; }

    // Get available data fields/columns
    const std::vector<std::string>& getFields() const { return fields_; }

    // Get data for a specific field
    std::vector<double> getFieldData(const std::string& fieldName) const;

    // Clear all loaded data
    void clear();

private:
    void processLine(const std::string& line);
    bool parseHeader(const std::string& line);
    bool parseData(const std::string& line);

    std::vector<LogEntry> entries_;
    std::vector<std::string> fields_;
    std::map<std::string, size_t> fieldIndices_;
    std::ifstream file_;
};

} // namespace filter 