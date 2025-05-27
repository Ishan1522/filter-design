#pragma once

#include <vector>
#include <memory>

namespace filter {

class Filter {
public:
    virtual ~Filter() = default;

    // Process a single sample
    virtual float process(float input) = 0;

    // Process a block of samples
    virtual void processBlock(const float* input, float* output, size_t numSamples) {
        for (size_t i = 0; i < numSamples; ++i) {
            output[i] = process(input[i]);
        }
    }

    // Reset filter state
    virtual void reset() = 0;

    // Get filter parameters
    virtual std::vector<float> getParameters() const = 0;
    virtual void setParameters(const std::vector<float>& params) = 0;
};

} // namespace filter 