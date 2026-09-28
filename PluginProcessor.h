#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "PitchDetector.h"

class ElectroBowAudioProcessor : public juce::AudioProcessor
{
public:
    ElectroBowAudioProcessor();
    ~ElectroBowAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getPitchFrequencyHz() const noexcept
    {
        return pitchDetector.getFrequencyHz();
    }

    float getPitchConfidence() const noexcept
    {
        return pitchDetector.getConfidence();
    }

    int getPitchMidiNote() const noexcept
    {
        return pitchDetector.getMidiNote();
    }

private:
    PitchDetector pitchDetector;

    // Currently sounding MIDI note.
    int currentMidiNote = 0;

    // Candidate note waiting for confirmation.
    int candidateMidiNote = 0;

    // How many consecutive detector updates confirmed the candidate.
    int candidateCount = 0;

    // How many consecutive detector updates contained no valid pitch.
    int noPitchCount = 0;

    // Number of confirmations required before changing note.
    static constexpr int kNoteConfirmations = 2;

    // Number of empty detections required before Note Off.
    static constexpr int kNoPitchConfirmations = 2;

    std::vector<float> monoScratch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElectroBowAudioProcessor)
};