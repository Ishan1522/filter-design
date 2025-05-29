#include "../../include/filter/LowPassFilter.hpp"
#include <cmath>
#include <stdexcept>
#include <vector>
#include <complex>
#include <string>

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

double LowPassFilter::processSample(double input) {
    // First-order low-pass filter: y[n] = α * x[n] + (1-α) * y[n-1]
    double output = alpha_ * input + (1.0f - alpha_) * prevOutput_;
    prevOutput_ = static_cast<float>(output);
    return output;
}

std::vector<double> LowPassFilter::getNumeratorCoefficients() const {
    return {alpha_};
}

std::vector<double> LowPassFilter::getDenominatorCoefficients() const {
    return {1.0, -(1.0 - alpha_)};
}

std::vector<std::complex<double>> LowPassFilter::getPoles() const {
    return {std::complex<double>(1.0 - alpha_, 0.0)};
}

std::vector<std::complex<double>> LowPassFilter::getZeros() const {
    return {std::complex<double>(0.0, 0.0)};
}

std::vector<std::complex<double>> LowPassFilter::getFrequencyResponse(const std::vector<double>& frequencies) const {
    std::vector<std::complex<double>> response;
    response.reserve(frequencies.size());
    
    for (double freq : frequencies) {
        std::complex<double> z = std::exp(std::complex<double>(0, 2.0 * M_PI * freq / sampleRate_));
        response.push_back(evaluateTransferFunction(z));
    }
    
    return response;
}

std::string LowPassFilter::getTypeName() const {
    return "LowPassFilter";
}

void LowPassFilter::setParameter(const std::string& name, double value) {
    if (name == "cutoffFreq") {
        cutoffFreq_ = static_cast<float>(value);
        // Recalculate filter coefficient
        float dt = 1.0f / sampleRate_;
        float RC = 1.0f / (2.0f * M_PI * cutoffFreq_);
        alpha_ = dt / (dt + RC);
    } else if (name == "sampleRate") {
        sampleRate_ = static_cast<float>(value);
        // Recalculate filter coefficient
        float dt = 1.0f / sampleRate_;
        float RC = 1.0f / (2.0f * M_PI * cutoffFreq_);
        alpha_ = dt / (dt + RC);
    } else {
        throw std::invalid_argument("Unknown parameter: " + name);
    }
}

double LowPassFilter::getParameter(const std::string& name) const {
    if (name == "cutoffFreq") {
        return cutoffFreq_;
    } else if (name == "sampleRate") {
        return sampleRate_;
    } else {
        throw std::invalid_argument("Unknown parameter: " + name);
    }
}

std::vector<double> LowPassFilter::processBlock(const std::vector<double>& input) {
    return Filter::processBlock(input);  // Use base class implementation
}

std::vector<double> LowPassFilter::getCoefficients() const {
    // TODO: Implement coefficient calculation for LowPassFilter
    return std::vector<double>();
}

std::complex<double> LowPassFilter::evaluateTransferFunction(const std::complex<double>& z) const {
    // For a first-order low-pass filter, the transfer function is:
    // H(z) = α / (1 - (1-α)z^-1)
    // where α is the filter coefficient
    return static_cast<double>(alpha_) / (1.0 - (1.0 - static_cast<double>(alpha_)) / z);
}

} // namespace filter 