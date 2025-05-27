#pragma once

#include <vector>
#include <memory>
#include <string>
#include <complex>

namespace filter {

class FilterBase {
public:
    virtual ~FilterBase() = default;

    // Process a single sample
    virtual double process(double input) = 0;
    
    // Process a block of samples
    virtual std::vector<double> processBlock(const std::vector<double>& input) = 0;
    
    // Get filter coefficients
    virtual std::vector<double> getCoefficients() const = 0;
    
    // Get filter poles and zeros
    virtual std::vector<std::complex<double>> getPoles() const = 0;
    virtual std::vector<std::complex<double>> getZeros() const = 0;
    
    // Get frequency response
    virtual std::vector<std::complex<double>> getFrequencyResponse(const std::vector<double>& frequencies) const = 0;
    
    // Get filter type name
    virtual std::string getType() const = 0;
    
    // Get filter parameters
    virtual std::vector<std::pair<std::string, double>> getParameters() const = 0;
    
    // Set filter parameters
    virtual void setParameters(const std::vector<std::pair<std::string, double>>& params) = 0;
    
    // Reset filter state
    virtual void reset() = 0;
};

} // namespace filter 