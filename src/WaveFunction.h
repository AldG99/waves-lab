#ifndef WAVE_FUNCTION_H
#define WAVE_FUNCTION_H

#include <string>
#include <memory>
#include "PhysicsConstants.h"

enum class WaveType {
    SINUSOIDAL,
    COSINE,
    SQUARE,
    TRIANGULAR,
    SAWTOOTH,
    CUSTOM
};

class WaveFunction {
public:
    virtual ~WaveFunction() = default;
    virtual double evaluate(double x, double t) const = 0;
    virtual double getAmplitude() const = 0;
    virtual double getFrequency() const = 0;
    virtual double getPhase() const = 0;
    virtual WaveType getType() const = 0;
    virtual std::string getEquation() const = 0;
    virtual void setAmplitude(double amplitude) = 0;
    virtual void setFrequency(double frequency) = 0;
    virtual void setPhase(double phase) = 0;
    
    // Derived properties
    virtual double getPeriod() const { return 1.0 / getFrequency(); }
    virtual double getWavelength(double velocity = 1.0) const { return velocity / getFrequency(); }
    virtual double getAngularFrequency() const { return Physics::TWO_PI * getFrequency(); }
    virtual double getWaveNumber(double velocity = 1.0) const { return Physics::TWO_PI / getWavelength(velocity); }
    virtual double getEnergy() const { return 0.5 * getAmplitude() * getAmplitude(); }
};

class SinusoidalWave : public WaveFunction {
private:
    double amplitude_;
    double frequency_;
    double phase_;

public:
    SinusoidalWave(double amplitude = 1.0, double frequency = 1.0, double phase = 0.0);
    
    double evaluate(double x, double t) const override;
    double getAmplitude() const override { return amplitude_; }
    double getFrequency() const override { return frequency_; }
    double getPhase() const override { return phase_; }
    WaveType getType() const override { return WaveType::SINUSOIDAL; }
    std::string getEquation() const override;
    
    void setAmplitude(double amplitude) override { amplitude_ = amplitude; }
    void setFrequency(double frequency) override { frequency_ = frequency; }
    void setPhase(double phase) override { phase_ = phase; }
    
    void setParameters(double amplitude, double frequency, double phase);
};

class CosineWave : public WaveFunction {
private:
    double amplitude_;
    double frequency_;
    double phase_;

public:
    CosineWave(double amplitude = 1.0, double frequency = 1.0, double phase = 0.0);
    
    double evaluate(double x, double t) const override;
    double getAmplitude() const override { return amplitude_; }
    double getFrequency() const override { return frequency_; }
    double getPhase() const override { return phase_; }
    WaveType getType() const override { return WaveType::COSINE; }
    std::string getEquation() const override;
    
    void setAmplitude(double amplitude) override { amplitude_ = amplitude; }
    void setFrequency(double frequency) override { frequency_ = frequency; }
    void setPhase(double phase) override { phase_ = phase; }
    
    void setParameters(double amplitude, double frequency, double phase);
};

class SquareWave : public WaveFunction {
private:
    double amplitude_;
    double frequency_;
    double phase_;

public:
    SquareWave(double amplitude = 1.0, double frequency = 1.0, double phase = 0.0);
    
    double evaluate(double x, double t) const override;
    double getAmplitude() const override { return amplitude_; }
    double getFrequency() const override { return frequency_; }
    double getPhase() const override { return phase_; }
    WaveType getType() const override { return WaveType::SQUARE; }
    std::string getEquation() const override;
    
    void setAmplitude(double amplitude) override { amplitude_ = amplitude; }
    void setFrequency(double frequency) override { frequency_ = frequency; }
    void setPhase(double phase) override { phase_ = phase; }
    
    void setParameters(double amplitude, double frequency, double phase);
};

class TriangularWave : public WaveFunction {
private:
    double amplitude_;
    double frequency_;
    double phase_;

public:
    TriangularWave(double amplitude = 1.0, double frequency = 1.0, double phase = 0.0);
    
    double evaluate(double x, double t) const override;
    double getAmplitude() const override { return amplitude_; }
    double getFrequency() const override { return frequency_; }
    double getPhase() const override { return phase_; }
    WaveType getType() const override { return WaveType::TRIANGULAR; }
    std::string getEquation() const override;
    
    void setAmplitude(double amplitude) override { amplitude_ = amplitude; }
    void setFrequency(double frequency) override { frequency_ = frequency; }
    void setPhase(double phase) override { phase_ = phase; }
    
    void setParameters(double amplitude, double frequency, double phase);
};

class SawtoothWave : public WaveFunction {
private:
    double amplitude_;
    double frequency_;
    double phase_;

public:
    SawtoothWave(double amplitude = 1.0, double frequency = 1.0, double phase = 0.0);
    
    double evaluate(double x, double t) const override;
    double getAmplitude() const override { return amplitude_; }
    double getFrequency() const override { return frequency_; }
    double getPhase() const override { return phase_; }
    WaveType getType() const override { return WaveType::SAWTOOTH; }
    std::string getEquation() const override;
    
    void setAmplitude(double amplitude) override { amplitude_ = amplitude; }
    void setFrequency(double frequency) override { frequency_ = frequency; }
    void setPhase(double phase) override { phase_ = phase; }
    
    void setParameters(double amplitude, double frequency, double phase);
};

#endif // WAVE_FUNCTION_H