#include "filter/ButterworthFilter.h"
#include <cmath>
#include <complex>
#include <algorithm>

namespace filter {

ButterworthFilter::ButterworthFilter(int order, double cutoffFreq, double sampleRate)
    : order_(order), cutoffFreq_(cutoffFreq), sampleRate_(sampleRate) {
    calculateCoefficients();
}

void ButterworthFilter::calculateCoefficients() {
    // Calculate normalized cutoff frequency
    double wc = 2.0 * M_PI * cutoffFreq_ / sampleRate_;
    
    // Calculate poles
    poles_.clear();
    for (int k = 0; k < order_; ++k) {
        double angle = M_PI * (2.0 * k + 1) / (2.0 * order_);
        std::complex<double> pole = std::exp(std::complex<double>(0, angle));
        poles_.push_back(wc * pole);
    }
    
    // Calculate coefficients for direct form II
    calculateDirectFormIICoefficients();
}

void ButterworthFilter::calculateDirectFormIICoefficients() {
    // Convert poles to direct form II coefficients
    // This is a simplified version - in practice, you'd want to use
    // a more robust method to handle numerical stability
    a_.resize(order_ + 1);
    b_.resize(order_ + 1);
    
    // Initialize coefficients
    std::fill(a_.begin(), a_.end(), 0.0);
    std::fill(b_.begin(), b_.end(), 0.0);
    
    // Calculate coefficients from poles
    // This is a simplified version - in practice, you'd want to use
    // a more robust method to handle numerical stability
    a_[0] = 1.0;
    for (size_t i = 0; i < poles_.size(); ++i) {
        a_[i + 1] = -std::real(poles_[i]);
    }
    
    // Normalize coefficients
    double gain = 1.0;
    for (size_t i = 0; i < poles_.size(); ++i) {
        gain *= std::abs(poles_[i]);
    }
    for (double& b : b_) {
        b /= gain;
    }
}

double ButterworthFilter::process(double input) {
    // Direct form II implementation
    double w = input;
    for (size_t i = 1; i < a_.size(); ++i) {
        w -= a_[i] * w_[i - 1];
    }
    
    double y = 0.0;
    for (size_t i = 0; i < b_.size(); ++i) {
        y += b_[i] * w_[i];
    }
    
    // Update state
    for (size_t i = w_.size() - 1; i > 0; --i) {
        w_[i] = w_[i - 1];
    }
    w_[0] = w;
    
    return y;
}

std::vector<double> ButterworthFilter::processBlock(const std::vector<double>& input) {
    std::vector<double> output;
    output.reserve(input.size());
    
    for (double sample : input) {
        output.push_back(process(sample));
    }
    
    return output;
}

std::vector<double> ButterworthFilter::getCoefficients() const {
    std::vector<double> coeffs;
    coeffs.insert(coeffs.end(), a_.begin(), a_.end());
    coeffs.insert(coeffs.end(), b_.begin(), b_.end());
    return coeffs;
}

std::vector<std::complex<double>> ButterworthFilter::getPoles() const {
    return poles_;
}

std::vector<std::complex<double>> ButterworthFilter::getZeros() const {
    // Butterworth filters have all zeros at infinity
    return std::vector<std::complex<double>>();
}

std::vector<std::complex<double>> ButterworthFilter::getFrequencyResponse(
    const std::vector<double>& frequencies) const {
    std::vector<std::complex<double>> response;
    response.reserve(frequencies.size());
    
    for (double f : frequencies) {
        std::complex<double> z = std::exp(std::complex<double>(0, 2.0 * M_PI * f / sampleRate_));
        std::complex<double> H = 1.0;
        
        // Calculate frequency response from poles
        for (const auto& pole : poles_) {
            H /= (z - pole);
        }
        
        response.push_back(H);
    }
    
    return response;
}

std::string ButterworthFilter::getType() const {
    return "Butterworth";
}

std::vector<std::pair<std::string, double>> ButterworthFilter::getParameters() const {
    return {
        {"Order", static_cast<double>(order_)},
        {"Cutoff Frequency", cutoffFreq_},
        {"Sample Rate", sampleRate_}
    };
}

void ButterworthFilter::setParameters(const std::vector<std::pair<std::string, double>>& params) {
    for (const auto& [name, value] : params) {
        if (name == "Order") {
            order_ = static_cast<int>(value);
        } else if (name == "Cutoff Frequency") {
            cutoffFreq_ = value;
        } else if (name == "Sample Rate") {
            sampleRate_ = value;
        }
    }
    calculateCoefficients();
}

void ButterworthFilter::reset() {
    std::fill(w_.begin(), w_.end(), 0.0);
}

} // namespace filter 