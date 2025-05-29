#pragma once

#include <vector>
#include <complex>
#include <string>
#include <memory>

namespace filter {

class Filter {
public:
    virtual ~Filter() = default;

    // Process a single sample
    virtual double processSample(double input) = 0;

    // Process a block of samples
    virtual std::vector<double> processBlock(const std::vector<double>& input) {
        std::vector<double> output;
        output.reserve(input.size());
        for (double sample : input) {
            output.push_back(processSample(sample));
        }
        return output;
    }

    // Get filter coefficients
    virtual std::vector<double> getNumeratorCoefficients() const = 0;
    virtual std::vector<double> getDenominatorCoefficients() const = 0;

    // Get poles and zeros
    virtual std::vector<std::complex<double>> getPoles() const = 0;
    virtual std::vector<std::complex<double>> getZeros() const = 0;

    // Get frequency response
    virtual std::vector<std::complex<double>> getFrequencyResponse(const std::vector<double>& frequencies) const = 0;

    // Get filter type name
    virtual std::string getTypeName() const = 0;

    // Set filter parameters
    virtual void setParameter(const std::string& name, double value) = 0;
    virtual double getParameter(const std::string& name) const = 0;

protected:
    // Helper function to calculate frequency response from poles and zeros
    virtual std::complex<double> evaluateTransferFunction(const std::complex<double>& z) const = 0;
};

} // namespace filter 