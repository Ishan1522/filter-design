#include "filter/LowPassFilter.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace filter {

LowPassFilter::LowPassFilter(float cutoffFreq, float sampleRate)
    : cutoffFreq_(cutoffFreq)
    , sampleRate_(sampleRate)
    , prevOutput_(0.0f) {
    // Calculate filter coefficient
    float dt = 1.0f / sampleRate_;
    float RC = 1.0f / (2.0f * M_PI * cutoffFreq_);
    alpha_ = dt / (dt + RC);
}

float LowPassFilter::process(float input) {
    // First-order low-pass filter: y[n] = α * x[n] + (1-α) * y[n-1]
    float output = alpha_ * input + (1.0f - alpha_) * prevOutput_;
    prevOutput_ = output;
    return output;
}

void LowPassFilter::reset() {
    prevOutput_ = 0.0f;
}

std::vector<float> LowPassFilter::getParameters() const {
    return {cutoffFreq_, sampleRate_};
}

void LowPassFilter::setParameters(const std::vector<float>& params) {
    if (params.size() >= 2) {
        cutoffFreq_ = params[0];
        sampleRate_ = params[1];
        
        // Recalculate filter coefficient
        float dt = 1.0f / sampleRate_;
        float RC = 1.0f / (2.0f * M_PI * cutoffFreq_);
        alpha_ = dt / (dt + RC);
    }
}

} // namespace filter 