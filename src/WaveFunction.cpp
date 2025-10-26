#include "WaveFunction.h"
#include <cmath>
#include <sstream>

// SinusoidalWave implementation
SinusoidalWave::SinusoidalWave(double amplitude, double frequency, double phase)
    : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

double SinusoidalWave::evaluate(double x, double t) const {
    return amplitude_ * std::sin(Physics::TWO_PI * frequency_ * t + phase_ * Physics::DEG_TO_RAD);
}

std::string SinusoidalWave::getEquation() const {
    std::ostringstream oss;
    oss << "y = " << amplitude_ << " * sin(2π * " << frequency_ << " * t + " << phase_ << "°)";
    return oss.str();
}

void SinusoidalWave::setParameters(double amplitude, double frequency, double phase) {
    amplitude_ = amplitude;
    frequency_ = frequency;
    phase_ = phase;
}

// CosineWave implementation
CosineWave::CosineWave(double amplitude, double frequency, double phase)
    : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

double CosineWave::evaluate(double x, double t) const {
    return amplitude_ * std::cos(Physics::TWO_PI * frequency_ * t + phase_ * Physics::DEG_TO_RAD);
}

std::string CosineWave::getEquation() const {
    std::ostringstream oss;
    oss << "y = " << amplitude_ << " * cos(2π * " << frequency_ << " * t + " << phase_ << "°)";
    return oss.str();
}

void CosineWave::setParameters(double amplitude, double frequency, double phase) {
    amplitude_ = amplitude;
    frequency_ = frequency;
    phase_ = phase;
}

// SquareWave implementation
SquareWave::SquareWave(double amplitude, double frequency, double phase)
    : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

double SquareWave::evaluate(double x, double t) const {
    double arg = Physics::TWO_PI * frequency_ * t + phase_ * Physics::DEG_TO_RAD;
    return amplitude_ * (std::sin(arg) >= 0 ? 1.0 : -1.0);
}

std::string SquareWave::getEquation() const {
    std::ostringstream oss;
    oss << "y = " << amplitude_ << " * sign(sin(2π * " << frequency_ << " * t + " << phase_ << "°))";
    return oss.str();
}

void SquareWave::setParameters(double amplitude, double frequency, double phase) {
    amplitude_ = amplitude;
    frequency_ = frequency;
    phase_ = phase;
}

// TriangularWave implementation
TriangularWave::TriangularWave(double amplitude, double frequency, double phase)
    : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

double TriangularWave::evaluate(double x, double t) const {
    double arg = frequency_ * t + phase_ / 360.0;
    double fractional_part = arg - std::floor(arg);
    
    if (fractional_part < 0.25) {
        return amplitude_ * 4.0 * fractional_part;
    } else if (fractional_part < 0.75) {
        return amplitude_ * (2.0 - 4.0 * fractional_part);
    } else {
        return amplitude_ * (4.0 * fractional_part - 4.0);
    }
}

std::string TriangularWave::getEquation() const {
    std::ostringstream oss;
    oss << "y = " << amplitude_ << " * triangular(" << frequency_ << " * t + " << phase_ << "°)";
    return oss.str();
}

void TriangularWave::setParameters(double amplitude, double frequency, double phase) {
    amplitude_ = amplitude;
    frequency_ = frequency;
    phase_ = phase;
}

// SawtoothWave implementation
SawtoothWave::SawtoothWave(double amplitude, double frequency, double phase)
    : amplitude_(amplitude), frequency_(frequency), phase_(phase) {}

double SawtoothWave::evaluate(double x, double t) const {
    double arg = frequency_ * t + phase_ / 360.0;
    double fractional_part = arg - std::floor(arg);
    return amplitude_ * (2.0 * fractional_part - 1.0);
}

std::string SawtoothWave::getEquation() const {
    std::ostringstream oss;
    oss << "y = " << amplitude_ << " * sawtooth(" << frequency_ << " * t + " << phase_ << "°)";
    return oss.str();
}

void SawtoothWave::setParameters(double amplitude, double frequency, double phase) {
    amplitude_ = amplitude;
    frequency_ = frequency;
    phase_ = phase;
}