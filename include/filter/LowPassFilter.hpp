#pragma once

#include "Filter.hpp"
#include <vector>
#include <complex>

namespace filter {

class LowPassFilter : public Filter {
public:
    LowPassFilter(float cutoffFreq = 1000.0f, float sampleRate = 44100.0f);

    // Override base class methods
    double processSample(double input) override;
    std::vector<double> getNumeratorCoefficients() const override;
    std::vector<double> getDenominatorCoefficients() const override;
    std::vector<std::complex<double>> getPoles() const override;
    std::vector<std::complex<double>> getZeros() const override;
    std::vector<std::complex<double>> getFrequencyResponse(const std::vector<double>& frequencies) const override;
    std::string getTypeName() const override;
    void setParameter(const std::string& name, double value) override;
    double getParameter(const std::string& name) const override;
    std::vector<double> processBlock(const std::vector<double>& input) override;

    std::vector<double> getCoefficients() const;

protected:
    std::complex<double> evaluateTransferFunction(const std::complex<double>& z) const override;

private:
    float cutoffFreq_;
    float sampleRate_;
    float alpha_;  // Filter coefficient
    float prevOutput_;  // Previous output sample
};

} // namespace filter 