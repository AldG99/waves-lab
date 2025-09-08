#ifndef WAVE_ENGINE_H
#define WAVE_ENGINE_H

#include "WaveFunction.h"
#include <vector>
#include <memory>

struct WaveAnalysis {
    double maxAmplitude;
    double minAmplitude;
    double rmsAmplitude;
    double frequency;
    double period;
    double energy;
    std::string phenomenon;
};

struct TimePoint {
    double time;
    double amplitude;
    double velocity;
    double acceleration;
};

class WaveEngine {
private:
    std::vector<std::unique_ptr<WaveFunction>> waves_;
    double velocity_;
    double currentTime_;
    
public:
    WaveEngine(double velocity = 1.0);
    ~WaveEngine() = default;
    
    // Wave management
    void addWave(std::unique_ptr<WaveFunction> wave);
    void removeWave(size_t index);
    void clearWaves();
    size_t getWaveCount() const { return waves_.size(); }
    const WaveFunction* getWave(size_t index) const;
    
    // Wave evaluation
    double evaluateSuperposition(double x, double t) const;
    double evaluateWave(size_t waveIndex, double x, double t) const;
    
    // Time series generation
    std::vector<double> generateTimeSeries(double duration, double sampleRate, double position = 0.0) const;
    std::vector<TimePoint> generateDetailedTimeSeries(double duration, double sampleRate, double position = 0.0) const;
    
    // Spatial series generation
    std::vector<double> generateSpatialSeries(double length, double sampleRate, double time = 0.0) const;
    
    // Analysis
    WaveAnalysis analyzeWaves(const std::vector<double>& data, double sampleRate) const;
    double calculateBeatFrequency() const;
    bool detectInterference() const;
    std::string detectPhenomenon() const;
    
    // Properties
    void setVelocity(double velocity) { velocity_ = velocity; }
    double getVelocity() const { return velocity_; }
    void setCurrentTime(double time) { currentTime_ = time; }
    double getCurrentTime() const { return currentTime_; }
    
    // Utility functions
    double calculateTotalEnergy() const;
    double getMaxAmplitude() const;
    double getDominantFrequency() const;
};

#endif // WAVE_ENGINE_H