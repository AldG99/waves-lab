#ifndef FOURIER_ANALYZER_H
#define FOURIER_ANALYZER_H

#include <vector>
#include <complex>

struct Complex {
    double real;
    double imag;
    
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}
    
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    Complex operator*(const Complex& other) const {
        return Complex(real * other.real - imag * other.imag,
                      real * other.imag + imag * other.real);
    }
    
    double magnitude() const {
        return std::sqrt(real * real + imag * imag);
    }
    
    double phase() const {
        return std::atan2(imag, real);
    }
};

struct FrequencyBin {
    double frequency;
    double magnitude;
    double phase;
};

struct Harmonic {
    double frequency;
    double amplitude;
    double phase;
    int order;  // 1 for fundamental, 2 for second harmonic, etc.
};

struct FrequencySpectrum {
    std::vector<FrequencyBin> bins;
    double sampleRate;
    double frequencyResolution;
    double maxFrequency;
    std::vector<Harmonic> harmonics;
};

class FourierAnalyzer {
public:
    FourierAnalyzer() = default;
    ~FourierAnalyzer() = default;
    
    // FFT implementation
    std::vector<Complex> fft(const std::vector<double>& signal);
    std::vector<Complex> ifft(const std::vector<Complex>& spectrum);
    
    // Spectrum analysis
    FrequencySpectrum getSpectrum(const std::vector<double>& signal, double sampleRate);
    std::vector<Harmonic> findHarmonics(const FrequencySpectrum& spectrum, double threshold = 0.1);
    
    // Filtering
    std::vector<double> lowPassFilter(const std::vector<double>& signal, double cutoffFreq, double sampleRate);
    std::vector<double> highPassFilter(const std::vector<double>& signal, double cutoffFreq, double sampleRate);
    std::vector<double> bandPassFilter(const std::vector<double>& signal, double lowFreq, double highFreq, double sampleRate);
    
    // Analysis utilities
    double findDominantFrequency(const FrequencySpectrum& spectrum);
    double calculateTHD(const std::vector<Harmonic>& harmonics); // Total Harmonic Distortion
    std::vector<double> getFrequencyAxis(size_t fftSize, double sampleRate);
    
private:
    // Helper functions
    std::vector<Complex> fftRecursive(const std::vector<Complex>& x);
    size_t nextPowerOfTwo(size_t n);
    void applyWindow(std::vector<double>& signal, const std::string& windowType = "hanning");
};

#endif // FOURIER_ANALYZER_H