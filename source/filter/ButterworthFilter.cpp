#include "../../include/filter/ButterworthFilter.hpp"
#include <cmath>
#include <algorithm>

#define M_PI 3.14159265358979323846

namespace filter {

ButterworthFilter::ButterworthFilter(int order, double cutoffFreq, double sampleRate)
    : order_(order)
    , cutoffFreq_(cutoffFreq)
    , sampleRate_(sampleRate)
{
    calculateCoefficients();
}

double ButterworthFilter::processSample(double input) {
    // Direct form II implementation
    double w = input;
    for (size_t i = 1; i < a_.size(); ++i) {
        w -= a_[i] * yHistory_[i - 1];
    }
    
    double y = 0.0;
    for (size_t i = 0; i < b_.size(); ++i) {
        y += b_[i] * xHistory_[i];
    }
    
    // Update state
    for (size_t i = xHistory_.size() - 1; i > 0; --i) {
        xHistory_[i] = xHistory_[i - 1];
    }
    xHistory_[0] = w;
    
    for (size_t i = yHistory_.size() - 1; i > 0; --i) {
        yHistory_[i] = yHistory_[i - 1];
    }
    yHistory_[0] = y;
    
    return y;
}

std::vector<double> ButterworthFilter::getNumeratorCoefficients() const {
    return b_;
}

std::vector<double> ButterworthFilter::getDenominatorCoefficients() const {
    return a_;
}

std::vector<std::complex<double>> ButterworthFilter::getPoles() const {
    return poles_;
}

std::vector<std::complex<double>> ButterworthFilter::getZeros() const {
    return zeros_;
}

std::vector<std::complex<double>> ButterworthFilter::getFrequencyResponse(
    const std::vector<double>& frequencies) const {
    std::vector<std::complex<double>> response;
    response.reserve(frequencies.size());
    
    for (double freq : frequencies) {
        std::complex<double> z = std::exp(std::complex<double>(0, 2.0 * M_PI * freq / sampleRate_));
        response.push_back(evaluateTransferFunction(z));
    }
    
    return response;
}

std::string ButterworthFilter::getTypeName() const {
    return "Butterworth";
}

void ButterworthFilter::setParameter(const std::string& name, double value) {
    if (name == "order") {
        order_ = static_cast<int>(value);
    } else if (name == "cutoffFreq") {
        cutoffFreq_ = value;
    } else if (name == "sampleRate") {
        sampleRate_ = value;
    }
    calculateCoefficients();
}

double ButterworthFilter::getParameter(const std::string& name) const {
    if (name == "order") {
        return static_cast<double>(order_);
    } else if (name == "cutoffFreq") {
        return cutoffFreq_;
    } else if (name == "sampleRate") {
        return sampleRate_;
    }
    return 0.0;
}

void ButterworthFilter::calculateCoefficients() {
    // Calculate normalized cutoff frequency
    double wc = 2.0 * M_PI * cutoffFreq_ / sampleRate_;
    
    // Calculate poles
    calculatePoles();
    
    // Convert poles to coefficients
    a_.resize(order_ + 1);
    b_.resize(order_ + 1);
    std::fill(a_.begin(), a_.end(), 0.0);
    std::fill(b_.begin(), b_.end(), 0.0);
    
    a_[0] = 1.0;
    for (size_t i = 0; i < poles_.size(); ++i) {
        a_[i + 1] = -std::real(poles_[i]);
    }
    
    // Normalize coefficients
    double gain = 1.0;
    for (const auto& pole : poles_) {
        gain *= std::abs(pole);
    }
    for (double& b : b_) {
        b /= gain;
    }
    
    // Initialize state history
    xHistory_.resize(b_.size(), 0.0);
    yHistory_.resize(a_.size(), 0.0);
}

void ButterworthFilter::calculatePoles() {
    poles_.clear();
    zeros_.clear();
    
    // Calculate poles for Butterworth filter
    for (int k = 0; k < order_; ++k) {
        double angle = M_PI * (2.0 * k + 1) / (2.0 * order_);
        std::complex<double> pole = std::exp(std::complex<double>(0, angle));
        poles_.push_back(pole);
    }
    
    // Butterworth filters have no zeros
}

} // namespace filter 