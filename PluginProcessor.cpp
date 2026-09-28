#include "PluginProcessor.h"
#include "PluginEditor.h"

ElectroBowAudioProcessor::ElectroBowAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void ElectroBowAudioProcessor::prepareToPlay (
    double sampleRate,
    int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate);

    monoScratch.assign (
        static_cast<size_t> (juce::jmax (1, samplesPerBlock)),
        0.0f);

    pitchDetector.reset();

    currentMidiNote = 0;
    candidateMidiNote = 0;
    candidateCount = 0;
    noPitchCount = 0;
}

void ElectroBowAudioProcessor::releaseResources()
{
}

bool ElectroBowAudioProcessor::isBusesLayoutSupported (
    const BusesLayout& layouts) const
{
    const auto& in = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (in != out)
        return false;

    return in == juce::AudioChannelSet::mono()
        || in == juce::AudioChannelSet::stereo();
}

void ElectroBowAudioProcessor::processBlock (
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numCh <= 0 || numSamples <= 0)
        return;

    // ------------------------------------------------------------
    // Convert input audio to mono.
    // ------------------------------------------------------------

    if (monoScratch.size() < static_cast<size_t> (numSamples))
    {
        monoScratch.resize (
            static_cast<size_t> (numSamples));
    }

    std::fill (
        monoScratch.begin(),
        monoScratch.begin() + numSamples,
        0.0f);

    for (int ch = 0; ch < numCh; ++ch)
    {
        const float* data =
            buffer.getReadPointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            monoScratch[static_cast<size_t> (i)] += data[i];
        }
    }

    const float scale =
        1.0f / static_cast<float> (numCh);

    for (int i = 0; i < numSamples; ++i)
    {
        monoScratch[static_cast<size_t> (i)] *= scale;
    }

    // ------------------------------------------------------------
    // Send audio to pitch detector.
    // ------------------------------------------------------------

    pitchDetector.push (
        monoScratch.data(),
        numSamples,
        getSampleRate());

    // ------------------------------------------------------------
    // Read detector result.
    // ------------------------------------------------------------

    const int detectedNote =
        getPitchMidiNote();

    // ------------------------------------------------------------
    // No valid pitch.
    // ------------------------------------------------------------

    if (detectedNote <= 0)
    {
        candidateMidiNote = 0;
        candidateCount = 0;

        if (currentMidiNote > 0)
        {
            ++noPitchCount;

            if (noPitchCount >= kNoPitchConfirmations)
            {
                midi.addEvent (
                    juce::MidiMessage::noteOff (
                        1,
                        currentMidiNote),
                    0);

                currentMidiNote = 0;
                noPitchCount = 0;
            }
        }

        return;
    }

    // We have a valid pitch again.
    noPitchCount = 0;

    // ------------------------------------------------------------
    // Same note is still being played.
    // ------------------------------------------------------------

    if (detectedNote == currentMidiNote)
    {
        candidateMidiNote = 0;
        candidateCount = 0;
        return;
    }

    // ------------------------------------------------------------
    // New candidate note.
    // ------------------------------------------------------------

    if (detectedNote != candidateMidiNote)
    {
        candidateMidiNote = detectedNote;
        candidateCount = 1;
        return;
    }

    // Same candidate detected again.
    ++candidateCount;

    // ------------------------------------------------------------
    // Candidate is now confirmed.
    // ------------------------------------------------------------

    if (candidateCount >= kNoteConfirmations)
    {
        if (currentMidiNote > 0)
        {
            midi.addEvent (
                juce::MidiMessage::noteOff (
                    1,
                    currentMidiNote),
                0);
        }

        midi.addEvent (
            juce::MidiMessage::noteOn (
                1,
                candidateMidiNote,
                static_cast<juce::uint8> (100)),
            0);

        currentMidiNote = candidateMidiNote;

        candidateMidiNote = 0;
        candidateCount = 0;
    }
}

bool ElectroBowAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor*
ElectroBowAudioProcessor::createEditor()
{
    return new ElectroBowAudioProcessorEditor (*this);
}

const juce::String ElectroBowAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ElectroBowAudioProcessor::acceptsMidi() const
{
    return false;
}

bool ElectroBowAudioProcessor::producesMidi() const
{
    return true;
}

bool ElectroBowAudioProcessor::isMidiEffect() const
{
    return false;
}

double ElectroBowAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ElectroBowAudioProcessor::getNumPrograms()
{
    return 1;
}

int ElectroBowAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ElectroBowAudioProcessor::setCurrentProgram (int)
{
}

const juce::String
ElectroBowAudioProcessor::getProgramName (int)
{
    return {};
}

void ElectroBowAudioProcessor::changeProgramName (
    int,
    const juce::String&)
{
}

void ElectroBowAudioProcessor::getStateInformation (
    juce::MemoryBlock& destData)
{
    destData.reset();
}

void ElectroBowAudioProcessor::setStateInformation (
    const void*,
    int)
{
}

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new ElectroBowAudioProcessor();
}