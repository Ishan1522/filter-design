#pragma once

#include "Filter.h"

namespace filter {

class LowPassFilter : public Filter {
public:
    LowPassFilter(float cutoffFreq = 1000.0f, float sampleRate = 44100.0f);

    float process(float input) override;
    void reset() override;
    std::vector<float> getParameters() const override;
    void setParameters(const std::vector<float>& params) override;

private:
    float cutoffFreq_;
    float sampleRate_;
    float alpha_;  // Filter coefficient
    float prevOutput_;  // Previous output sample
};

} // namespace filter 