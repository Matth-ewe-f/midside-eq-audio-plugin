#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
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

    // === Static Functions ===================================================
    static void addParameters
    (ParameterLayout*, std::string prefix, std::string number,
    std::string channels, float defaultFreq);

private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    juce::SmoothedValue<float> smoothBypass;
    float gain;
    float q;
    double sampleRate;
    juce::AudioProcessorValueTreeState* listenTo;
    std::string name;
    
    // === Parameter Settings =================================================
    inline static const juce::NormalisableRange<float> onOffRange
        { juce::NormalisableRange<float>(0, 1, 1) };
    inline static const juce::NormalisableRange<float> freqRange
        { juce::NormalisableRange<float>(20, 20000, 0.1f, 0.35f) };
    inline static const juce::NormalisableRange<float> gainRange
        { juce::NormalisableRange<float>(-18, 18, 0.1f) };
    inline static const juce::NormalisableRange<float> qRange
        { juce::NormalisableRange<float>(0.25, 10, 0.01f, 0.7f) };

    // === Private Helper =====================================================
    void setFilterParameters(float freq, float gain, float q);
};