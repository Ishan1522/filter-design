#pragma once

#include "DataSource.h"
#include <memory>
#include <string>
#include <map>
#include <set>
#include <functional>

namespace data {

class WpiLogDataSource : public DataSource {
public:
    explicit WpiLogDataSource(const std::string& filename);
    
    // DataSource interface implementation
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    std::vector<std::string> getAvailableFields() const override;
    bool subscribe(const std::string& field) override;
    void unsubscribe(const std::string& field) override;
    void setDataCallback(std::function<void(const DataPoint&)> callback) override;
    std::vector<DataPoint> getHistoricalData(
        const std::string& field,
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const override;

private:
    std::string filename_;
    bool connected_;
    std::unique_ptr<wpigui::DataLogReader> reader_;
    std::map<int, std::string> entries_;
    std::set<int> subscribedEntries_;
    std::function<void(const DataPoint&)> dataCallback_;
};

} // namespace data 