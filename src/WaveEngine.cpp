#include "WaveEngine.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>

WaveEngine::WaveEngine(double velocity) : velocity_(velocity), currentTime_(0.0) {}

void WaveEngine::addWave(std::unique_ptr<WaveFunction> wave) {
    waves_.push_back(std::move(wave));
}

void WaveEngine::removeWave(size_t index) {
    if (index < waves_.size()) {
        waves_.erase(waves_.begin() + index);
    }
}

void WaveEngine::clearWaves() {
    waves_.clear();
}

const WaveFunction* WaveEngine::getWave(size_t index) const {
    if (index < waves_.size()) {
        return waves_[index].get();
    }
    return nullptr;
}

double WaveEngine::evaluateSuperposition(double x, double t) const {
    double result = 0.0;
    for (const auto& wave : waves_) {
        result += wave->evaluate(x, t);
    }
    return result;
}

double WaveEngine::evaluateWave(size_t waveIndex, double x, double t) const {
    if (waveIndex < waves_.size()) {
        return waves_[waveIndex]->evaluate(x, t);
    }
    return 0.0;
}

std::vector<double> WaveEngine::generateTimeSeries(double duration, double sampleRate, double position) const {
    std::vector<double> data;
    double dt = 1.0 / sampleRate;
    int numSamples = static_cast<int>(duration * sampleRate);
    
    data.reserve(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        double t = i * dt;
        data.push_back(evaluateSuperposition(position, t));
    }
    
    return data;
}

std::vector<TimePoint> WaveEngine::generateDetailedTimeSeries(double duration, double sampleRate, double position) const {
    std::vector<TimePoint> data;
    double dt = 1.0 / sampleRate;
    int numSamples = static_cast<int>(duration * sampleRate);
    
    data.reserve(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        double t = i * dt;
        double amp = evaluateSuperposition(position, t);
        
        // Calculate velocity (numerical derivative)
        double vel = 0.0;
        if (i > 0) {
            double prev_amp = evaluateSuperposition(position, (i-1) * dt);
            vel = (amp - prev_amp) / dt;
        }
        
        // Calculate acceleration (second derivative)
        double acc = 0.0;
        if (i > 1) {
            double prev_amp = evaluateSuperposition(position, (i-1) * dt);
            double prev_prev_amp = evaluateSuperposition(position, (i-2) * dt);
            acc = (amp - 2 * prev_amp + prev_prev_amp) / (dt * dt);
        }
        
        data.push_back({t, amp, vel, acc});
    }
    
    return data;
}

std::vector<double> WaveEngine::generateSpatialSeries(double length, double sampleRate, double time) const {
    std::vector<double> data;
    double dx = 1.0 / sampleRate;
    int numSamples = static_cast<int>(length * sampleRate);
    
    data.reserve(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        double x = i * dx;
        data.push_back(evaluateSuperposition(x, time));
    }
    
    return data;
}

WaveAnalysis WaveEngine::analyzeWaves(const std::vector<double>& data, double sampleRate) const {
    WaveAnalysis analysis;
    
    if (data.empty()) {
        analysis = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "No data"};
        return analysis;
    }
    
    // Basic statistics
    auto minmax = std::minmax_element(data.begin(), data.end());
    analysis.maxAmplitude = *minmax.second;
    analysis.minAmplitude = *minmax.first;
    
    // RMS amplitude
    double sumSquares = std::accumulate(data.begin(), data.end(), 0.0, 
        [](double sum, double val) { return sum + val * val; });
    analysis.rmsAmplitude = std::sqrt(sumSquares / data.size());
    
    // Energy (proportional to amplitude squared)
    analysis.energy = 0.5 * analysis.rmsAmplitude * analysis.rmsAmplitude;
    
    // Dominant frequency (simplified - would need FFT for accurate analysis)
    analysis.frequency = getDominantFrequency();
    analysis.period = analysis.frequency > 0 ? 1.0 / analysis.frequency : 0.0;
    
    // Detect phenomenon
    analysis.phenomenon = detectPhenomenon();
    
    return analysis;
}

double WaveEngine::calculateBeatFrequency() const {
    if (waves_.size() < 2) return 0.0;
    
    std::vector<double> frequencies;
    for (const auto& wave : waves_) {
        frequencies.push_back(wave->getFrequency());
    }
    
    std::sort(frequencies.begin(), frequencies.end());
    
    // Beat frequency is the difference between closest frequencies
    double minDiff = std::numeric_limits<double>::max();
    for (size_t i = 1; i < frequencies.size(); ++i) {
        double diff = frequencies[i] - frequencies[i-1];
        if (diff > 0 && diff < minDiff) {
            minDiff = diff;
        }
    }
    
    return minDiff < std::numeric_limits<double>::max() ? minDiff : 0.0;
}

bool WaveEngine::detectInterference() const {
    return waves_.size() > 1;
}

std::string WaveEngine::detectPhenomenon() const {
    if (waves_.size() == 0) return "No waves";
    if (waves_.size() == 1) return "Single wave";
    
    double beatFreq = calculateBeatFrequency();
    if (beatFreq > 0 && beatFreq < 2.0) {
        return "Beating";
    }
    
    // Check for resonance (waves with same frequency)
    std::vector<double> frequencies;
    for (const auto& wave : waves_) {
        frequencies.push_back(wave->getFrequency());
    }
    
    bool hasResonance = false;
    for (size_t i = 0; i < frequencies.size(); ++i) {
        for (size_t j = i + 1; j < frequencies.size(); ++j) {
            if (std::abs(frequencies[i] - frequencies[j]) < 0.01) {
                hasResonance = true;
                break;
            }
        }
        if (hasResonance) break;
    }
    
    if (hasResonance) return "Resonance";
    
    return "Superposition";
}

double WaveEngine::calculateTotalEnergy() const {
    double totalEnergy = 0.0;
    for (const auto& wave : waves_) {
        totalEnergy += wave->getEnergy();
    }
    return totalEnergy;
}

double WaveEngine::getMaxAmplitude() const {
    double maxAmp = 0.0;
    for (const auto& wave : waves_) {
        maxAmp = std::max(maxAmp, wave->getAmplitude());
    }
    return maxAmp;
}

double WaveEngine::getDominantFrequency() const {
    if (waves_.empty()) return 0.0;
    
    // Find frequency of wave with highest amplitude
    double maxAmp = 0.0;
    double dominantFreq = 0.0;
    
    for (const auto& wave : waves_) {
        if (wave->getAmplitude() > maxAmp) {
            maxAmp = wave->getAmplitude();
            dominantFreq = wave->getFrequency();
        }
    }
    
    return dominantFreq;
}