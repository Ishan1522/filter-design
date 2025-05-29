#include "../include/LogLoader.hpp"
#include <iostream>
#include <fstream>
#include <wpigui/DataLogReader.h>

LogLoader::LogLoader() = default;
LogLoader::~LogLoader() = default;

void LogLoader::Clear() {
    m_entries.clear();
    m_fields.clear();
    m_entryNames.clear();
}

bool LogLoader::LoadFile(const std::string& filename) {
    Clear();

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    wpigui::DataLogReader reader(file);
    if (!reader.IsValid()) {
        return false;
    }

    while (auto record = reader.Next()) {
        ProcessRecord(*record);
    }

    return true;
}

void LogLoader::ProcessRecord(const wpigui::DataLogReader::Record& record) {
    switch (record.type) {
        case wpigui::DataLogReader::RecordType::kStart:
            ProcessControlRecord(record);
            break;
        case wpigui::DataLogReader::RecordType::kFinish:
            // Handle finish record if needed
            break;
        case wpigui::DataLogReader::RecordType::kSetMetadata:
            // Handle metadata if needed
            break;
        case wpigui::DataLogReader::RecordType::kData:
            ProcessDataRecord(record);
            break;
    }
}

void LogLoader::ProcessControlRecord(const wpigui::DataLogReader::Record& record) {
    if (record.type != wpigui::DataLogReader::RecordType::kStart) {
        return;
    }

    // Store the entry name mapping
    m_entryNames[record.entry] = record.name;
    
    // Add to fields list if not already present
    if (std::find(m_fields.begin(), m_fields.end(), record.name) == m_fields.end()) {
        m_fields.push_back(record.name);
    }
}

void LogLoader::ProcessDataRecord(const wpigui::DataLogReader::Record& record) {
    if (record.type != wpigui::DataLogReader::RecordType::kData) {
        return;
    }

    // Find or create entry for this timestamp
    LogEntry* entry = nullptr;
    if (!m_entries.empty() && m_entries.back().timestamp == record.timestamp) {
        entry = &m_entries.back();
    } else {
        m_entries.push_back({record.timestamp, {}});
        entry = &m_entries.back();
    }

    // Get the field name for this entry ID
    auto it = m_entryNames.find(record.entry);
    if (it == m_entryNames.end()) {
        return;  // Skip if we don't know the field name
    }

    // Convert the data to double based on type
    double value = 0.0;
    switch (record.dataType) {
        case wpigui::DataLogReader::DataType::kBoolean:
            value = record.GetBoolean() ? 1.0 : 0.0;
            break;
        case wpigui::DataLogReader::DataType::kInteger:
            value = static_cast<double>(record.GetInteger());
            break;
        case wpigui::DataLogReader::DataType::kFloat:
            value = static_cast<double>(record.GetFloat());
            break;
        case wpigui::DataLogReader::DataType::kDouble:
            value = record.GetDouble();
            break;
        default:
            return;  // Skip unsupported types
    }

    // Store the value
    entry->values[it->second] = value;
} 
} 