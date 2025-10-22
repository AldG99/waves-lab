#include "InterferenceCalculator.h"
#include "PhysicsConstants.h"
#include <cmath>
#include <algorithm>
#include <sstream>

InterferenceResult InterferenceCalculator::calculateTwoWaveInterference(
    const WaveFunction& wave1, 
    const WaveFunction& wave2,
    double time,
    double length,
    int numPoints) {
    
    InterferenceResult result;
    
    std::vector<double> positions;
    std::vector<double> amplitudes;
    positions.reserve(numPoints);
    amplitudes.reserve(numPoints);
    
    double dx = length / (numPoints - 1);
    
    // Calculate superposition at each point
    for (int i = 0; i < numPoints; ++i) {
        double x = i * dx;
        double amp1 = wave1.evaluate(x, time);
        double amp2 = wave2.evaluate(x, time);
        double totalAmp = amp1 + amp2;
        
        positions.push_back(x);
        amplitudes.push_back(totalAmp);
    }
    
    // Analyze the result
    auto minmax = std::minmax_element(amplitudes.begin(), amplitudes.end());
    result.amplitude = std::max(std::abs(*minmax.first), std::abs(*minmax.second));
    
    // Calculate phase shift
    result.phase = calculatePhaseShift(wave1, wave2);
    
    // Classify interference type
    double amp1 = wave1.getAmplitude();
    double amp2 = wave2.getAmplitude();
    result.type = classifyInterference(amp1, amp2, result.amplitude);
    
    // Calculate beat frequency if applicable
    result.beatFrequency = calculateBeatFrequency(wave1.getFrequency(), wave2.getFrequency());
    
    // Find nodes and antinodes
    std::vector<InterferenceNode> nodes = findInterferenceNodes({&wave1, &wave2}, time, length, numPoints);
    
    for (const auto& node : nodes) {
        if (node.type == InterferenceNode::NODE) {
            result.nodePositions.push_back(node.position);
        } else {
            result.antinodePositions.push_back(node.position);
        }
    }
    
    result.description = generateDescription(result);
    
    return result;
}

InterferenceResult InterferenceCalculator::calculateMultiWaveInterference(
    const std::vector<const WaveFunction*>& waves,
    double time,
    double length,
    int numPoints) {
    
    InterferenceResult result;
    
    if (waves.empty()) {
        result.type = InterferenceResult::NO_INTERFERENCE;
        result.description = "No waves provided";
        return result;
    }
    
    if (waves.size() == 1) {
        result.type = InterferenceResult::NO_INTERFERENCE;
        result.amplitude = waves[0]->getAmplitude();
        result.description = "Single wave - no interference";
        return result;
    }
    
    std::vector<double> amplitudes;
    amplitudes.reserve(numPoints);
    
    double dx = length / (numPoints - 1);
    
    // Calculate superposition at each point
    for (int i = 0; i < numPoints; ++i) {
        double x = i * dx;
        double totalAmp = calculateTotalAmplitude(waves, x, time);
        amplitudes.push_back(totalAmp);
    }
    
    // Analyze the result
    auto minmax = std::minmax_element(amplitudes.begin(), amplitudes.end());
    result.amplitude = std::max(std::abs(*minmax.first), std::abs(*minmax.second));
    
    // For multi-wave, we look for beat frequency between dominant waves
    if (waves.size() >= 2) {
        result.beatFrequency = calculateBeatFrequency(waves[0]->getFrequency(), waves[1]->getFrequency());
    }
    
    // Find nodes and antinodes
    std::vector<InterferenceNode> nodes = findInterferenceNodes(waves, time, length, numPoints);
    
    for (const auto& node : nodes) {
        if (node.type == InterferenceNode::NODE) {
            result.nodePositions.push_back(node.position);
        } else {
            result.antinodePositions.push_back(node.position);
        }
    }
    
    // Detect resonance
    if (detectResonance(waves)) {
        result.type = InterferenceResult::CONSTRUCTIVE;
        result.description = "Resonance detected - constructive interference";
    } else if (result.beatFrequency > 0 && result.beatFrequency < 2.0) {
        result.type = InterferenceResult::PARTIAL;
        result.description = "Beat phenomenon detected";
    } else {
        result.type = InterferenceResult::PARTIAL;
        result.description = "Complex multi-wave interference";
    }
    
    return result;
}

double InterferenceCalculator::calculateBeatFrequency(double f1, double f2) {
    return std::abs(f1 - f2);
}

double InterferenceCalculator::calculateBeatPeriod(double f1, double f2) {
    double beatFreq = calculateBeatFrequency(f1, f2);
    return beatFreq > 0 ? 1.0 / beatFreq : 0.0;
}

std::vector<double> InterferenceCalculator::calculateBeatEnvelope(
    const WaveFunction& wave1,
    const WaveFunction& wave2,
    double duration,
    double sampleRate) {
    
    std::vector<double> envelope;
    int numSamples = static_cast<int>(duration * sampleRate);
    envelope.reserve(numSamples);
    
    double dt = 1.0 / sampleRate;
    double beatFreq = calculateBeatFrequency(wave1.getFrequency(), wave2.getFrequency());
    double avgAmplitude = (wave1.getAmplitude() + wave2.getAmplitude()) / 2.0;
    double amplitudeDiff = std::abs(wave1.getAmplitude() - wave2.getAmplitude());
    
    for (int i = 0; i < numSamples; ++i) {
        double t = i * dt;
        double envelopeAmp = avgAmplitude + amplitudeDiff * std::cos(Physics::TWO_PI * beatFreq * t / 2.0);
        envelope.push_back(std::abs(envelopeAmp));
    }
    
    return envelope;
}

std::vector<InterferenceNode> InterferenceCalculator::findInterferenceNodes(
    const std::vector<const WaveFunction*>& waves,
    double time,
    double length,
    int numPoints,
    double threshold) {
    
    std::vector<InterferenceNode> nodes;
    std::vector<double> amplitudes;
    amplitudes.reserve(numPoints);
    
    double dx = length / (numPoints - 1);
    
    // Calculate total amplitude at each point
    for (int i = 0; i < numPoints; ++i) {
        double x = i * dx;
        double totalAmp = calculateTotalAmplitude(waves, x, time);
        amplitudes.push_back(std::abs(totalAmp));
    }
    
    // Find local minima (nodes) and maxima (antinodes)
    std::vector<double> minima = findLocalExtrema(amplitudes, false);
    std::vector<double> maxima = findLocalExtrema(amplitudes, true);
    
    // Convert indices to positions and filter by threshold
    for (double minIdx : minima) {
        int idx = static_cast<int>(minIdx);
        if (idx >= 0 && idx < numPoints && amplitudes[idx] <= threshold) {
            nodes.push_back({idx * dx, amplitudes[idx], InterferenceNode::NODE});
        }
    }
    
    for (double maxIdx : maxima) {
        int idx = static_cast<int>(maxIdx);
        if (idx >= 0 && idx < numPoints && amplitudes[idx] >= threshold) {
            nodes.push_back({idx * dx, amplitudes[idx], InterferenceNode::ANTINODE});
        }
    }
    
    return nodes;
}

std::vector<double> InterferenceCalculator::calculateStandingWave(
    double amplitude1, double amplitude2,
    double frequency,
    double phaseShift,
    double length,
    int numPoints,
    double time) {
    
    std::vector<double> wave;
    wave.reserve(numPoints);
    
    double dx = length / (numPoints - 1);
    double k = Physics::TWO_PI * frequency;  // Assuming unit velocity
    double omega = Physics::TWO_PI * frequency;
    
    for (int i = 0; i < numPoints; ++i) {
        double x = i * dx;
        
        // Standing wave: superposition of forward and backward traveling waves
        double forward = amplitude1 * std::sin(k * x - omega * time);
        double backward = amplitude2 * std::sin(k * x + omega * time + phaseShift);
        
        wave.push_back(forward + backward);
    }
    
    return wave;
}

double InterferenceCalculator::calculatePhaseShift(const WaveFunction& wave1, const WaveFunction& wave2) {
    double phase1 = wave1.getPhase();
    double phase2 = wave2.getPhase();
    double diff = phase2 - phase1;
    
    // Normalize to [0, 360) degrees
    while (diff < 0) diff += 360.0;
    while (diff >= 360.0) diff -= 360.0;
    
    return diff;
}

bool InterferenceCalculator::areInPhase(const WaveFunction& wave1, const WaveFunction& wave2, double tolerance) {
    double phaseDiff = calculatePhaseShift(wave1, wave2);
    return (phaseDiff <= tolerance) || (std::abs(phaseDiff - 360.0) <= tolerance);
}

bool InterferenceCalculator::areOutOfPhase(const WaveFunction& wave1, const WaveFunction& wave2, double tolerance) {
    double phaseDiff = calculatePhaseShift(wave1, wave2);
    return std::abs(phaseDiff - 180.0) <= tolerance;
}

bool InterferenceCalculator::detectResonance(
    const std::vector<const WaveFunction*>& waves,
    double frequencyTolerance) {
    
    if (waves.size() < 2) return false;
    
    for (size_t i = 0; i < waves.size(); ++i) {
        for (size_t j = i + 1; j < waves.size(); ++j) {
            double freq1 = waves[i]->getFrequency();
            double freq2 = waves[j]->getFrequency();
            
            if (std::abs(freq1 - freq2) <= frequencyTolerance) {
                return true;
            }
        }
    }
    
    return false;
}

double InterferenceCalculator::calculateResonanceAmplification(
    const std::vector<const WaveFunction*>& waves) {
    
    if (waves.empty()) return 0.0;
    
    double totalAmplitude = 0.0;
    double individualSum = 0.0;
    
    for (const auto* wave : waves) {
        individualSum += wave->getAmplitude();
    }
    
    // Calculate actual amplitude at a test point
    totalAmplitude = std::abs(calculateTotalAmplitude(waves, 0.0, 0.0));
    
    return individualSum > 0 ? totalAmplitude / individualSum : 0.0;
}

double InterferenceCalculator::calculateTotalAmplitude(
    const std::vector<const WaveFunction*>& waves,
    double position,
    double time) {
    
    double total = 0.0;
    for (const auto* wave : waves) {
        total += wave->evaluate(position, time);
    }
    return total;
}

InterferenceResult::Type InterferenceCalculator::classifyInterference(
    double amplitude1,
    double amplitude2,
    double resultAmplitude,
    double tolerance) {
    
    double maxPossible = amplitude1 + amplitude2;
    double minPossible = std::abs(amplitude1 - amplitude2);
    
    if (resultAmplitude >= maxPossible - tolerance) {
        return InterferenceResult::CONSTRUCTIVE;
    } else if (resultAmplitude <= minPossible + tolerance) {
        return InterferenceResult::DESTRUCTIVE;
    } else {
        return InterferenceResult::PARTIAL;
    }
}

std::vector<double> InterferenceCalculator::findLocalExtrema(const std::vector<double>& data, bool findMaxima) {
    std::vector<double> extrema;
    
    if (data.size() < 3) return extrema;
    
    for (size_t i = 1; i < data.size() - 1; ++i) {
        bool isExtremum = false;
        
        if (findMaxima) {
            isExtremum = (data[i] > data[i-1]) && (data[i] > data[i+1]);
        } else {
            isExtremum = (data[i] < data[i-1]) && (data[i] < data[i+1]);
        }
        
        if (isExtremum) {
            extrema.push_back(static_cast<double>(i));
        }
    }
    
    return extrema;
}

double InterferenceCalculator::calculateRMSAmplitude(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    double sumSquares = 0.0;
    for (double val : data) {
        sumSquares += val * val;
    }
    
    return std::sqrt(sumSquares / data.size());
}

std::string InterferenceCalculator::generateDescription(const InterferenceResult& result) {
    std::ostringstream oss;
    
    switch (result.type) {
        case InterferenceResult::CONSTRUCTIVE:
            oss << "Constructive interference - waves reinforce each other";
            break;
        case InterferenceResult::DESTRUCTIVE:
            oss << "Destructive interference - waves cancel each other";
            break;
        case InterferenceResult::PARTIAL:
            oss << "Partial interference";
            if (result.beatFrequency > 0) {
                oss << " with beating at " << result.beatFrequency << " Hz";
            }
            break;
        case InterferenceResult::NO_INTERFERENCE:
            oss << "No interference detected";
            break;
    }
    
    if (result.nodePositions.size() > 0) {
        oss << ". " << result.nodePositions.size() << " nodes detected";
    }
    
    if (result.antinodePositions.size() > 0) {
        oss << ". " << result.antinodePositions.size() << " antinodes detected";
    }
    
    return oss.str();
}