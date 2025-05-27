#pragma once

#include "FilterBase.h"
#include <vector>
#include <complex>

namespace filter {

class ButterworthFilter : public FilterBase {
public:
    ButterworthFilter(int order, double cutoffFreq, double sampleRate);
    
    // FilterBase interface implementation
    double process(double input) override;
    std::vector<double> processBlock(const std::vector<double>& input) override;
    std::vector<double> getCoefficients() const override;
    std::vector<std::complex<double>> getPoles() const override;
    std::vector<std::complex<double>> getZeros() const override;
    std::vector<std::complex<double>> getFrequencyResponse(
        const std::vector<double>& frequencies) const override;
    std::string getType() const override;
    std::vector<std::pair<std::string, double>> getParameters() const override;
    void setParameters(const std::vector<std::pair<std::string, double>>& params) override;
    void reset() override;

private:
    void calculateCoefficients();
    void calculateDirectFormIICoefficients();

    int order_;
    double cutoffFreq_;
    double sampleRate_;
    std::vector<double> a_;  // Denominator coefficients
    std::vector<double> b_;  // Numerator coefficients
    std::vector<double> w_;  // State variables
    std::vector<std::complex<double>> poles_;
};

} // namespace filter 