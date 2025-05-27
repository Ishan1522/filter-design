#include "data/WpiLogDataSource.h"
#include <wpigui/DataLogReader.h>
#include <fstream>
#include <chrono>

namespace data {

WpiLogDataSource::WpiLogDataSource(const std::string& filename)
    : filename_(filename), connected_(false) {}

bool WpiLogDataSource::connect() {
    if (connected_) {
        return true;
    }

    try {
        // Open the log file
        std::ifstream file(filename_, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Create DataLogReader
        reader_ = std::make_unique<wpigui::DataLogReader>(file);
        if (!reader_->IsValid()) {
            return false;
        }

        // Get all entries
        entries_.clear();
        while (reader_->Next()) {
            if (reader_->IsStart()) {
                entries_[reader_->GetStartEntry()] = reader_->GetStartName();
            }
        }

        connected_ = true;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void WpiLogDataSource::disconnect() {
    reader_.reset();
    entries_.clear();
    connected_ = false;
}

bool WpiLogDataSource::isConnected() const {
    return connected_;
}

std::vector<std::string> WpiLogDataSource::getAvailableFields() const {
    std::vector<std::string> fields;
    fields.reserve(entries_.size());
    for (const auto& [id, name] : entries_) {
        fields.push_back(name);
    }
    return fields;
}

bool WpiLogDataSource::subscribe(const std::string& field) {
    if (!connected_) {
        return false;
    }

    // Find the entry ID for this field
    for (const auto& [id, name] : entries_) {
        if (name == field) {
            subscribedEntries_.insert(id);
            return true;
        }
    }
    return false;
}

void WpiLogDataSource::unsubscribe(const std::string& field) {
    if (!connected_) {
        return;
    }

    // Find and remove the entry ID for this field
    for (const auto& [id, name] : entries_) {
        if (name == field) {
            subscribedEntries_.erase(id);
            break;
        }
    }
}

void WpiLogDataSource::setDataCallback(std::function<void(const DataPoint&)> callback) {
    dataCallback_ = std::move(callback);
}

std::vector<DataPoint> WpiLogDataSource::getHistoricalData(
    const std::string& field,
    std::chrono::system_clock::time_point start,
    std::chrono::system_clock::time_point end) const {
    
    std::vector<DataPoint> data;
    if (!connected_) {
        return data;
    }

    // Find the entry ID for this field
    int entryId = -1;
    for (const auto& [id, name] : entries_) {
        if (name == field) {
            entryId = id;
            break;
        }
    }
    if (entryId == -1) {
        return data;
    }

    // Reset reader to start
    reader_->Reset();
    
    // Convert time points to microseconds
    auto startUs = std::chrono::duration_cast<std::chrono::microseconds>(
        start.time_since_epoch()).count();
    auto endUs = std::chrono::duration_cast<std::chrono::microseconds>(
        end.time_since_epoch()).count();

    // Read data
    while (reader_->Next()) {
        if (reader_->IsData() && reader_->GetDataEntry() == entryId) {
            auto timestamp = reader_->GetTimestamp();
            if (timestamp >= startUs && timestamp <= endUs) {
                DataPoint point;
                point.timestamp = std::chrono::system_clock::time_point(
                    std::chrono::microseconds(timestamp));
                point.field = field;
                
                // Convert data to double based on type
                switch (reader_->GetDataType()) {
                    case wpigui::DataLogReader::kBoolean:
                        point.value = reader_->GetBoolean() ? 1.0 : 0.0;
                        break;
                    case wpigui::DataLogReader::kInteger:
                        point.value = static_cast<double>(reader_->GetInteger());
                        break;
                    case wpigui::DataLogReader::kFloat:
                        point.value = static_cast<double>(reader_->GetFloat());
                        break;
                    case wpigui::DataLogReader::kDouble:
                        point.value = reader_->GetDouble();
                        break;
                    default:
                        continue;  // Skip unsupported types
                }
                
                data.push_back(point);
            }
        }
    }

    return data;
}

} // namespace data 