#include <iostream>
#include <memory>
#include "WaveFunction.h"
#include "WaveEngine.h"
#include "FourierAnalyzer.h"
#include "InterferenceCalculator.h"

void demonstrateBasicWaves() {
    std::cout << "=== Basic Wave Demonstration ===" << std::endl;
    
    // Create different types of waves
    auto sineWave = std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0);
    auto cosineWave = std::make_unique<CosineWave>(1.5, 1.5, 90.0);
    auto squareWave = std::make_unique<SquareWave>(1.0, 0.5, 0.0);
    
    std::cout << "Sine Wave: " << sineWave->getEquation() << std::endl;
    std::cout << "Cosine Wave: " << cosineWave->getEquation() << std::endl;
    std::cout << "Square Wave: " << squareWave->getEquation() << std::endl;
    
    // Evaluate at specific points
    double time = 1.0;
    double position = 0.0;
    
    std::cout << std::endl << "Wave values at t=" << time << "s, x=" << position << "m:" << std::endl;
    std::cout << "Sine: " << sineWave->evaluate(position, time) << std::endl;
    std::cout << "Cosine: " << cosineWave->evaluate(position, time) << std::endl;
    std::cout << "Square: " << squareWave->evaluate(position, time) << std::endl;
}

void demonstrateSuperposition() {
    std::cout << std::endl << "=== Wave Superposition Demonstration ===" << std::endl;
    
    WaveEngine engine;
    
    // Add multiple waves
    engine.addWave(std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0));
    engine.addWave(std::make_unique<SinusoidalWave>(1.0, 1.1, 0.0));
    
    std::cout << "Added two sine waves with frequencies 1.0 Hz and 1.1 Hz" << std::endl;
    std::cout << "Beat frequency: " << engine.calculateBeatFrequency() << " Hz" << std::endl;
    std::cout << "Phenomenon detected: " << engine.detectPhenomenon() << std::endl;
    std::cout << "Total energy: " << engine.calculateTotalEnergy() << " J" << std::endl;
    
    // Generate time series
    auto timeSeries = engine.generateTimeSeries(5.0, 100.0);
    std::cout << "Generated " << timeSeries.size() << " data points over 5 seconds" << std::endl;
    
    // Show some sample values
    std::cout << "Sample values: ";
    for (int i = 0; i < 5 && i < timeSeries.size(); ++i) {
        std::cout << timeSeries[i] << " ";
    }
    std::cout << "..." << std::endl;
}

void demonstrateInterference() {
    std::cout << std::endl << "=== Interference Analysis Demonstration ===" << std::endl;
    
    InterferenceCalculator calculator;
    
    // Create two waves for interference
    SinusoidalWave wave1(2.0, 1.0, 0.0);
    SinusoidalWave wave2(2.0, 1.0, 180.0);  // 180° phase shift
    
    std::cout << "Analyzing interference between two sine waves:" << std::endl;
    std::cout << "Wave 1: " << wave1.getEquation() << std::endl;
    std::cout << "Wave 2: " << wave2.getEquation() << std::endl;
    
    auto result = calculator.calculateTwoWaveInterference(wave1, wave2);
    
    std::cout << "Interference result: " << result.description << std::endl;
    std::cout << "Result amplitude: " << result.amplitude << std::endl;
    std::cout << "Phase shift: " << result.phase << "°" << std::endl;
    std::cout << "Beat frequency: " << result.beatFrequency << " Hz" << std::endl;
    
    if (!result.nodePositions.empty()) {
        std::cout << "Node positions: ";
        for (size_t i = 0; i < std::min(5ul, result.nodePositions.size()); ++i) {
            std::cout << result.nodePositions[i] << " ";
        }
        if (result.nodePositions.size() > 5) std::cout << "...";
        std::cout << std::endl;
    }
}

void demonstrateFourierAnalysis() {
    std::cout << std::endl << "=== Fourier Analysis Demonstration ===" << std::endl;
    
    FourierAnalyzer analyzer;
    WaveEngine engine;
    
    // Create a complex wave (fundamental + harmonics)
    engine.addWave(std::make_unique<SinusoidalWave>(2.0, 1.0, 0.0));  // Fundamental
    engine.addWave(std::make_unique<SinusoidalWave>(1.0, 2.0, 0.0));  // 2nd harmonic
    engine.addWave(std::make_unique<SinusoidalWave>(0.5, 4.0, 0.0));  // 4th harmonic
    
    // Generate signal
    auto signal = engine.generateTimeSeries(4.0, 256.0);  // Power of 2 for efficient FFT
    
    // Analyze spectrum
    auto spectrum = analyzer.getSpectrum(signal, 256.0);
    
    std::cout << "Analyzing complex wave with multiple harmonics" << std::endl;
    std::cout << "Sample rate: " << spectrum.sampleRate << " Hz" << std::endl;
    std::cout << "Frequency resolution: " << spectrum.frequencyResolution << " Hz" << std::endl;
    std::cout << "Harmonics found: " << spectrum.harmonics.size() << std::endl;
    
    // Display harmonics
    for (const auto& harmonic : spectrum.harmonics) {
        std::cout << "  " << harmonic.order << "° harmonic: " 
                  << harmonic.frequency << " Hz, amplitude: " << harmonic.amplitude << std::endl;
    }
    
    // Calculate THD
    double thd = analyzer.calculateTHD(spectrum.harmonics);
    std::cout << "Total Harmonic Distortion: " << thd << "%" << std::endl;
}

int main() {
    std::cout << "🌊 Wave Simulator - Console Demonstration 🌊" << std::endl;
    std::cout << "================================================" << std::endl;
    
    try {
        demonstrateBasicWaves();
        demonstrateSuperposition();
        demonstrateInterference();
        demonstrateFourierAnalysis();
        
        std::cout << std::endl << "=== Demonstration Complete ===" << std::endl;
        std::cout << "For GUI version, compile with: make gui" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}