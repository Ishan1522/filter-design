#ifndef WPILOGDECODER_H
#define WPILOGDECODER_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

const std::string HEADER_STRING = "WPI_LOG"; // Example header string
const uint16_t HEADER_VERSION = 0x0100; // Example version number

class WPILOGDecoderRecord {
public:
    WPILOGDecoderRecord(uint32_t entry, uint64_t timestamp, const std::vector<uint8_t>& data)
        : entry(entry), timestamp(timestamp), data(data) {}

    uint32_t getEntry() const { return entry; }
    uint64_t getTimestamp() const { return timestamp; }
    
    // Further decoding functions
    bool isControl() const { return entry == CONTROL_ENTRY; }
    // Add additional methods as necessary...

private:
    uint32_t entry;
    uint64_t timestamp;
    std::vector<uint8_t> data;
};

class WPILOGDecoder {
public:
    WPILOGDecoder(const std::vector<uint8_t>& data) : data(data) {}

    bool isValid() const {
        return data.size() >= 12 &&
               std::string(data.begin(), data.begin() + 6) == HEADER_STRING &&
               getVersion() == HEADER_VERSION;
    }

    uint16_t getVersion() const {
        if (data.size() < 12) return 0;
        return *reinterpret_cast<const uint16_t*>(data.data() + 6);
    }

    void forEach(void (*callback)(const WPILOGDecoderRecord&, size_t)) {
        if (!isValid()) throw std::runtime_error("Log is not valid");
        
        size_t position = 12; // Starting after the header
        while (position < data.size()) {
            auto [record, size] = getRecordAtPosition(position);
            if (record == nullptr) break;
            callback(*record, position);
            position += size;
        }
    }

private:
    const std::vector<uint8_t>& data;

    std::pair<std::unique_ptr<WPILOGDecoderRecord>, size_t> getRecordAtPosition(size_t position) {
        if (data.size() < position + 4) return {nullptr, 0};

        // Read variable-length integers and create WPILOGDecoderRecord
        // Adjust as necessary for your protocol
        uint8_t entryLength = (data[position] & 0x03) + 1;
        uint8_t sizeLength = ((data[position] >> 2) & 0x03) + 1;
        uint8_t timestampLength = ((data[position] >> 4) & 0x07) + 1;
        size_t headerLength = 1 + entryLength + sizeLength + timestampLength;

        if (data.size() < position + headerLength) return {nullptr, 0};

        uint32_t entry = readVariableInteger(position + 1, entryLength);
        uint32_t size = readVariableInteger(position + 1 + entryLength, sizeLength);
        uint64_t timestamp = readVariableInteger(position + 1 + entryLength + sizeLength, timestampLength);

        if (data.size() < position + headerLength + size || entry < 0 || size < 0) return {nullptr, 0};

        std::vector<uint8_t> recordData(data.begin() + position + headerLength,
                                         data.begin() + position + headerLength + size);
        
        return {std::make_unique<WPILOGDecoderRecord>(entry, timestamp, recordData), headerLength + size};
    }

    uint64_t readVariableInteger(size_t position, uint8_t length) {
        uint64_t value = 0;
        for (size_t i = 0; i < length && i < 8; ++i) {
            uint8_t byte = data[position + i];
            if (i == 7) {
                // Last byte, apply sign
                if (byte & (1 << 7)) {
                    value -= (1ULL << 63);
                }
                byte &= ~(1 << 7);
            }
            value |= static_cast<uint64_t>(byte) << (i * 8);
        }
        return value;
    }
};

#endif // WPILOGDECODER_H
