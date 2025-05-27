#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <chrono>

namespace data {

struct DataPoint {
    std::chrono::system_clock::time_point timestamp;
    double value;
    std::string field;
};

class DataSource {
public:
    virtual ~DataSource() = default;

    // Connect to the data source
    virtual bool connect() = 0;
    
    // Disconnect from the data source
    virtual void disconnect() = 0;
    
    // Check if connected
    virtual bool isConnected() const = 0;
    
    // Get available fields/channels
    virtual std::vector<std::string> getAvailableFields() const = 0;
    
    // Subscribe to a field
    virtual bool subscribe(const std::string& field) = 0;
    
    // Unsubscribe from a field
    virtual void unsubscribe(const std::string& field) = 0;
    
    // Set callback for new data
    virtual void setDataCallback(std::function<void(const DataPoint&)> callback) = 0;
    
    // Get historical data for a field
    virtual std::vector<DataPoint> getHistoricalData(
        const std::string& field,
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const = 0;
};

} // namespace data 