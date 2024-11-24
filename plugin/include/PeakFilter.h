#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace dsp = juce::dsp;

class PeakFilter : juce::AudioProcessorValueTreeState::Listener
{
public:
    // === Lifecycle ==========================================================
    PeakFilter(float frequency);

    // === State Tree Listener ================================================
    void setListenTo(juce::AudioProcessorValueTreeState*, std::string);
    void parameterChanged(const juce::String&, float) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setGain(float);
    void setQFactor(float);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);

private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    juce::SmoothedValue<float> smoothBypass;
    float gain;
    float q;
    double sampleRate;
    juce::AudioProcessorValueTreeState* listenTo;
    std::string name;

    // === Private Helper =====================================================
    void setFilterParameters(float freq, float gain, float q);
};