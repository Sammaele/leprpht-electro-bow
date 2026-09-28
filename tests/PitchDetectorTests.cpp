#include "../PitchDetector.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

namespace {
bool approximatelyEqual(float actual, float expected, float tolerance) {
    return std::abs(actual - expected) <= tolerance;
}
} // namespace

int main() {
    constexpr double sampleRate = 48000.0;
    constexpr float frequency = 440.0f;

    std::vector<float> samples(PitchDetector::kWindowSize);

    for (size_t i = 0; i < samples.size(); ++i) {
        const auto phase = static_cast<float>(2.0 * juce::MathConstants<double>::pi * frequency *
                                              static_cast<double>(i) / sampleRate);
        samples[i] = std::sin(phase);
    }

    PitchDetector detector;
    detector.push(samples.data(), static_cast<int>(samples.size()), sampleRate);

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);

    while (std::chrono::steady_clock::now() < deadline) {
        if (detector.getFrequencyHz() > 0.0f)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    const auto detectedFrequency = detector.getFrequencyHz();
    const auto detectedConfidence = detector.getConfidence();
    const auto detectedMidiNote = detector.getMidiNote();

    if (!approximatelyEqual(detectedFrequency, frequency, 8.0f)) {
        std::cerr << "Expected approximately 440 Hz, got " << detectedFrequency << " Hz\n";
        return 1;
    }

    if (detectedConfidence <= 0.0f) {
        std::cerr << "Expected positive pitch confidence\n";
        return 1;
    }

    if (detectedMidiNote != 69) {
        std::cerr << "Expected MIDI note 69, got " << detectedMidiNote << "\n";
        return 1;
    }

    return 0;
}
