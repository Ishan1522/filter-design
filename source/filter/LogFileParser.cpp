#include "../../include/filter/LogFileParser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace filter {

bool LogFileParser::loadFile(const std::string& filename) {
    clear();
    
    file_.open(filename);
    if (!file_.is_open()) {
        return false;
    }

    std::string line;
    bool headerParsed = false;

    // Read and process each line
    while (std::getline(file_, line)) {
        if (!headerParsed) {
            if (parseHeader(line)) {
                headerParsed = true;
            }
        } else {
            parseData(line);
        }
    }

    file_.close();
    return !entries_.empty();
}

void LogFileParser::clear() {
    entries_.clear();
    fields_.clear();
    fieldIndices_.clear();
    if (file_.is_open()) {
        file_.close();
    }
}

bool LogFileParser::parseHeader(const std::string& line) {
    std::istringstream iss(line);
    std::string field;
    
    // Skip timestamp column
    if (!(iss >> field)) {
        return false;
    }

    // Read field names
    while (iss >> field) {
        // Remove any quotes
        field.erase(std::remove(field.begin(), field.end(), '"'), field.end());
        fields_.push_back(field);
        fieldIndices_[field] = fields_.size() - 1;
    }

    return !fields_.empty();
}

bool LogFileParser::parseData(const std::string& line) {
    std::istringstream iss(line);
    double timestamp;
    
    // Read timestamp
    if (!(iss >> timestamp)) {
        return false;
    }

    LogEntry entry;
    entry.timestamp = timestamp;

    // Read values for each field
    double value;
    for (const auto& field : fields_) {
        if (iss >> value) {
            entry.values[field] = value;
        } else {
            // If we can't read a value, use the last known value or 0
            if (!entries_.empty() && entries_.back().values.count(field) > 0) {
                entry.values[field] = entries_.back().values[field];
            } else {
                entry.values[field] = 0.0;
            }
        }
    }

    entries_.push_back(entry);
    return true;
}

std::vector<double> LogFileParser::getFieldData(const std::string& fieldName) const {
    std::vector<double> data;
    data.reserve(entries_.size());

    for (const auto& entry : entries_) {
        auto it = entry.values.find(fieldName);
        if (it != entry.values.end()) {
            data.push_back(it->second);
        }
    }

    return data;
}

} // namespace filter 