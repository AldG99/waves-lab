#include "FourierAnalyzer.h"
#include "PhysicsConstants.h"
#include <cmath>
#include <algorithm>

std::vector<Complex> FourierAnalyzer::fft(const std::vector<double>& signal) {
    // Convert to complex
    std::vector<Complex> complexSignal;
    complexSignal.reserve(signal.size());
    for (double val : signal) {
        complexSignal.emplace_back(val, 0.0);
    }
    
    // Pad to next power of 2
    size_t n = nextPowerOfTwo(signal.size());
    complexSignal.resize(n, Complex(0.0, 0.0));
    
    return fftRecursive(complexSignal);
}

std::vector<Complex> FourierAnalyzer::ifft(const std::vector<Complex>& spectrum) {
    // Conjugate the spectrum
    std::vector<Complex> conjugated;
    conjugated.reserve(spectrum.size());
    for (const auto& c : spectrum) {
        conjugated.emplace_back(c.real, -c.imag);
    }
    
    // Apply FFT to conjugated spectrum
    auto result = fftRecursive(conjugated);
    
    // Conjugate and normalize
    double n = static_cast<double>(result.size());
    for (auto& c : result) {
        c.real = c.real / n;
        c.imag = -c.imag / n;
    }
    
    return result;
}

std::vector<Complex> FourierAnalyzer::fftRecursive(const std::vector<Complex>& x) {
    size_t n = x.size();
    if (n <= 1) return x;
    
    // Divide
    std::vector<Complex> even, odd;
    for (size_t i = 0; i < n; i += 2) {
        even.push_back(x[i]);
        if (i + 1 < n) {
            odd.push_back(x[i + 1]);
        }
    }
    
    // Conquer
    auto evenFFT = fftRecursive(even);
    auto oddFFT = fftRecursive(odd);
    
    // Combine
    std::vector<Complex> result(n);
    for (size_t k = 0; k < n / 2; ++k) {
        double angle = -Physics::TWO_PI * k / n;
        Complex w(std::cos(angle), std::sin(angle));
        Complex t = w * oddFFT[k];
        
        result[k] = evenFFT[k] + t;
        result[k + n / 2] = evenFFT[k] - t;
    }
    
    return result;
}

size_t FourierAnalyzer::nextPowerOfTwo(size_t n) {
    if (n <= 1) return 1;
    size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

FrequencySpectrum FourierAnalyzer::getSpectrum(const std::vector<double>& signal, double sampleRate) {
    FrequencySpectrum spectrum;
    
    if (signal.empty()) return spectrum;
    
    // Apply windowing
    std::vector<double> windowedSignal = signal;
    applyWindow(windowedSignal, "hanning");
    
    // Compute FFT
    auto fftResult = fft(windowedSignal);
    
    // Calculate spectrum properties
    size_t fftSize = fftResult.size();
    spectrum.sampleRate = sampleRate;
    spectrum.frequencyResolution = sampleRate / fftSize;
    spectrum.maxFrequency = sampleRate / 2.0;  // Nyquist frequency
    
    // Extract magnitude and phase for positive frequencies
    size_t numBins = fftSize / 2 + 1;  // Include DC and Nyquist
    spectrum.bins.reserve(numBins);
    
    for (size_t i = 0; i < numBins; ++i) {
        double frequency = i * spectrum.frequencyResolution;
        double magnitude = fftResult[i].magnitude();
        double phase = fftResult[i].phase();
        
        // Normalize magnitude (except for DC and Nyquist)
        if (i > 0 && i < fftSize / 2) {
            magnitude *= 2.0 / fftSize;
        } else {
            magnitude /= fftSize;
        }
        
        spectrum.bins.push_back({frequency, magnitude, phase});
    }
    
    // Find harmonics
    spectrum.harmonics = findHarmonics(spectrum);
    
    return spectrum;
}

std::vector<Harmonic> FourierAnalyzer::findHarmonics(const FrequencySpectrum& spectrum, double threshold) {
    std::vector<Harmonic> harmonics;
    
    if (spectrum.bins.empty()) return harmonics;
    
    // Find fundamental frequency (first significant peak above DC)
    double maxMagnitude = 0.0;
    double fundamentalFreq = 0.0;
    
    for (size_t i = 1; i < spectrum.bins.size(); ++i) {  // Skip DC component
        if (spectrum.bins[i].magnitude > maxMagnitude) {
            maxMagnitude = spectrum.bins[i].magnitude;
            fundamentalFreq = spectrum.bins[i].frequency;
        }
    }
    
    if (fundamentalFreq == 0.0 || maxMagnitude < threshold) return harmonics;
    
    // Find harmonics based on fundamental frequency
    double toleranceHz = spectrum.frequencyResolution * 2.0;  // Allow some frequency deviation
    
    for (int order = 1; order <= 10; ++order) {  // Look for up to 10th harmonic
        double targetFreq = order * fundamentalFreq;
        
        if (targetFreq > spectrum.maxFrequency) break;
        
        // Find the bin closest to target frequency
        size_t closestBin = 0;
        double minDistance = std::abs(spectrum.bins[0].frequency - targetFreq);
        
        for (size_t i = 1; i < spectrum.bins.size(); ++i) {
            double distance = std::abs(spectrum.bins[i].frequency - targetFreq);
            if (distance < minDistance) {
                minDistance = distance;
                closestBin = i;
            }
        }
        
        // Check if the closest bin is within tolerance and above threshold
        if (minDistance <= toleranceHz && spectrum.bins[closestBin].magnitude >= threshold) {
            harmonics.push_back({
                spectrum.bins[closestBin].frequency,
                spectrum.bins[closestBin].magnitude,
                spectrum.bins[closestBin].phase,
                order
            });
        }
    }
    
    return harmonics;
}

double FourierAnalyzer::findDominantFrequency(const FrequencySpectrum& spectrum) {
    if (spectrum.bins.empty()) return 0.0;
    
    double maxMagnitude = 0.0;
    double dominantFreq = 0.0;
    
    // Skip DC component
    for (size_t i = 1; i < spectrum.bins.size(); ++i) {
        if (spectrum.bins[i].magnitude > maxMagnitude) {
            maxMagnitude = spectrum.bins[i].magnitude;
            dominantFreq = spectrum.bins[i].frequency;
        }
    }
    
    return dominantFreq;
}

double FourierAnalyzer::calculateTHD(const std::vector<Harmonic>& harmonics) {
    if (harmonics.empty()) return 0.0;
    
    double fundamentalPower = 0.0;
    double harmonicPower = 0.0;
    
    for (const auto& harmonic : harmonics) {
        double power = harmonic.amplitude * harmonic.amplitude;
        if (harmonic.order == 1) {
            fundamentalPower = power;
        } else {
            harmonicPower += power;
        }
    }
    
    if (fundamentalPower == 0.0) return 0.0;
    
    return std::sqrt(harmonicPower / fundamentalPower) * 100.0;  // Return as percentage
}

std::vector<double> FourierAnalyzer::getFrequencyAxis(size_t fftSize, double sampleRate) {
    std::vector<double> frequencies;
    size_t numBins = fftSize / 2 + 1;
    double df = sampleRate / fftSize;
    
    frequencies.reserve(numBins);
    for (size_t i = 0; i < numBins; ++i) {
        frequencies.push_back(i * df);
    }
    
    return frequencies;
}

void FourierAnalyzer::applyWindow(std::vector<double>& signal, const std::string& windowType) {
    size_t n = signal.size();
    
    if (windowType == "hanning") {
        for (size_t i = 0; i < n; ++i) {
            double window = 0.5 - 0.5 * std::cos(Physics::TWO_PI * i / (n - 1));
            signal[i] *= window;
        }
    } else if (windowType == "hamming") {
        for (size_t i = 0; i < n; ++i) {
            double window = 0.54 - 0.46 * std::cos(Physics::TWO_PI * i / (n - 1));
            signal[i] *= window;
        }
    } else if (windowType == "blackman") {
        for (size_t i = 0; i < n; ++i) {
            double window = 0.42 - 0.5 * std::cos(Physics::TWO_PI * i / (n - 1)) + 
                           0.08 * std::cos(4 * Physics::PI * i / (n - 1));
            signal[i] *= window;
        }
    }
    // "rectangular" window (no window) is the default - no modification needed
}

std::vector<double> FourierAnalyzer::lowPassFilter(const std::vector<double>& signal, double cutoffFreq, double sampleRate) {
    auto spectrum = fft(signal);
    size_t cutoffBin = static_cast<size_t>(cutoffFreq * spectrum.size() / sampleRate);
    
    // Zero out frequencies above cutoff
    for (size_t i = cutoffBin; i < spectrum.size() - cutoffBin; ++i) {
        spectrum[i] = Complex(0.0, 0.0);
    }
    
    // Convert back to time domain
    auto filtered = ifft(spectrum);
    std::vector<double> result;
    result.reserve(filtered.size());
    for (const auto& c : filtered) {
        result.push_back(c.real);
    }
    
    return result;
}

std::vector<double> FourierAnalyzer::highPassFilter(const std::vector<double>& signal, double cutoffFreq, double sampleRate) {
    auto spectrum = fft(signal);
    size_t cutoffBin = static_cast<size_t>(cutoffFreq * spectrum.size() / sampleRate);
    
    // Zero out frequencies below cutoff
    for (size_t i = 0; i <= cutoffBin; ++i) {
        spectrum[i] = Complex(0.0, 0.0);
        if (i > 0) {
            spectrum[spectrum.size() - i] = Complex(0.0, 0.0);
        }
    }
    
    // Convert back to time domain
    auto filtered = ifft(spectrum);
    std::vector<double> result;
    result.reserve(filtered.size());
    for (const auto& c : filtered) {
        result.push_back(c.real);
    }
    
    return result;
}

std::vector<double> FourierAnalyzer::bandPassFilter(const std::vector<double>& signal, double lowFreq, double highFreq, double sampleRate) {
    auto spectrum = fft(signal);
    size_t lowBin = static_cast<size_t>(lowFreq * spectrum.size() / sampleRate);
    size_t highBin = static_cast<size_t>(highFreq * spectrum.size() / sampleRate);
    
    // Zero out frequencies outside the band
    for (size_t i = 0; i < spectrum.size(); ++i) {
        if (i < lowBin || i > highBin) {
            // Handle symmetric spectrum
            if (i > spectrum.size() - highBin && i < spectrum.size() - lowBin) {
                continue;  // Keep this frequency
            }
            spectrum[i] = Complex(0.0, 0.0);
        }
    }
    
    // Convert back to time domain
    auto filtered = ifft(spectrum);
    std::vector<double> result;
    result.reserve(filtered.size());
    for (const auto& c : filtered) {
        result.push_back(c.real);
    }
    
    return result;
}