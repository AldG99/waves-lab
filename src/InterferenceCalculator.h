#ifndef INTERFERENCE_CALCULATOR_H
#define INTERFERENCE_CALCULATOR_H

#include "WaveFunction.h"
#include <vector>

struct InterferenceResult {
    enum Type {
        CONSTRUCTIVE,
        DESTRUCTIVE,
        PARTIAL,
        NO_INTERFERENCE
    };
    
    Type type;
    double amplitude;
    double phase;
    std::vector<double> nodePositions;
    std::vector<double> antinodePositions;
    double beatFrequency;
    std::string description;
};

struct InterferenceNode {
    double position;
    double amplitude;
    enum NodeType {
        NODE,      // Minimum amplitude
        ANTINODE   // Maximum amplitude
    } type;
};

class InterferenceCalculator {
public:
    InterferenceCalculator() = default;
    ~InterferenceCalculator() = default;
    
    // Two-wave interference
    InterferenceResult calculateTwoWaveInterference(
        const WaveFunction& wave1, 
        const WaveFunction& wave2,
        double time = 0.0,
        double length = 10.0,
        int numPoints = 1000
    );
    
    // Multi-wave interference
    InterferenceResult calculateMultiWaveInterference(
        const std::vector<const WaveFunction*>& waves,
        double time = 0.0,
        double length = 10.0,
        int numPoints = 1000
    );
    
    // Beat phenomena
    double calculateBeatFrequency(double f1, double f2);
    double calculateBeatPeriod(double f1, double f2);
    std::vector<double> calculateBeatEnvelope(
        const WaveFunction& wave1,
        const WaveFunction& wave2,
        double duration = 10.0,
        double sampleRate = 100.0
    );
    
    // Standing waves
    std::vector<InterferenceNode> findInterferenceNodes(
        const std::vector<const WaveFunction*>& waves,
        double time = 0.0,
        double length = 10.0,
        int numPoints = 1000,
        double threshold = 0.1
    );
    
    std::vector<double> calculateStandingWave(
        double amplitude1, double amplitude2,
        double frequency,
        double phaseShift = Physics::PI, // 180 degrees for counter-propagating waves
        double length = 10.0,
        int numPoints = 1000,
        double time = 0.0
    );
    
    // Phase relationships
    double calculatePhaseShift(const WaveFunction& wave1, const WaveFunction& wave2);
    bool areInPhase(const WaveFunction& wave1, const WaveFunction& wave2, double tolerance = 0.1);
    bool areOutOfPhase(const WaveFunction& wave1, const WaveFunction& wave2, double tolerance = 0.1);
    
    // Resonance detection
    bool detectResonance(
        const std::vector<const WaveFunction*>& waves,
        double frequencyTolerance = 0.01
    );
    
    double calculateResonanceAmplification(
        const std::vector<const WaveFunction*>& waves
    );
    
    // Interference patterns
    std::vector<double> calculateYoungsDoubleSlitPattern(
        double wavelength,
        double slitSeparation,
        double screenDistance,
        double screenWidth = 10.0,
        int numPoints = 1000
    );
    
    std::vector<double> calculateSingleSlitDiffraction(
        double wavelength,
        double slitWidth,
        double screenDistance,
        double screenWidth = 10.0,
        int numPoints = 1000
    );
    
    // Utility functions
    double calculateTotalAmplitude(
        const std::vector<const WaveFunction*>& waves,
        double position,
        double time
    );
    
    InterferenceResult::Type classifyInterference(
        double amplitude1,
        double amplitude2,
        double resultAmplitude,
        double tolerance = 0.1
    );

private:
    // Helper functions
    std::vector<double> findLocalExtrema(const std::vector<double>& data, bool findMaxima = true);
    double calculateRMSAmplitude(const std::vector<double>& data);
    std::string generateDescription(const InterferenceResult& result);
};

#endif // INTERFERENCE_CALCULATOR_H