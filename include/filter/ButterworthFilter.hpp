#pragma once

#include "Filter.hpp"
#include <vector>
#include <complex>
#include <string>
#include <map>

namespace filter {

class ButterworthFilter : public Filter {
public:
    ButterworthFilter(int order = 2, double cutoffFreq = 1000.0, double sampleRate = 44100.0);

    // Filter interface implementation
    double processSample(double input) override;
    std::vector<double> getNumeratorCoefficients() const override;
    std::vector<double> getDenominatorCoefficients() const override;
    std::vector<std::complex<double>> getPoles() const override;
    std::vector<std::complex<double>> getZeros() const override;
    std::vector<std::complex<double>> getFrequencyResponse(const std::vector<double>& frequencies) const override;
    std::string getTypeName() const override;
    void setParameter(const std::string& name, double value) override;
    double getParameter(const std::string& name) const override;

private:
    void calculateCoefficients();
    void calculatePoles();

    int order_;
    double cutoffFreq_;
    double sampleRate_;
    std::vector<double> b_; // Numerator coefficients
    std::vector<double> a_; // Denominator coefficients
    std::vector<std::complex<double>> poles_;
    std::vector<std::complex<double>> zeros_;
    std::vector<double> xHistory_; // Input history
    std::vector<double> yHistory_; // Output history
};

} // namespace filter 