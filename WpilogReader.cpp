#include "WPILOGDecoder.h"

void processRecord(const WPILOGDecoderRecord& record, size_t position) {
    std::cout << "Entry: " << record.getEntry() << ", Timestamp: " << record.getTimestamp() << std::endl;
    // Further processing...
}

int main() {
    // Example: Load your WPILog data into a vector of bytes
    std::vector<uint8_t> logData = { /* Load your data here */ };

    WPILOGDecoder decoder(logData);
    if (decoder.isValid()) {
        decoder.forEach(processRecord);
    } else {
        std::cerr << "Invalid WPILog data!" << std::endl;
    }

    return 0;
}
