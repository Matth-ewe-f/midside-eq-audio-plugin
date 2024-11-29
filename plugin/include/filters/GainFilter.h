#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
using Parameter = juce::AudioProcessorValueTreeState::Parameter;
namespace dsp = juce::dsp;

class GainFilter : juce::AudioProcessorValueTreeState::Listener
{
public:
    std::string parameterName;

    // === Lifecycle ==========================================================
    GainFilter
    (std::string idPostfix, std::string displayName, float min, float max,
    float defaultValue);

    // === Parameter Listener =================================================
    void listenTo(juce::AudioProcessorValueTreeState*);
    void parameterChanged(const juce::String&, float);

    // === Set Parameters =====================================================
    void addParameters(ParameterLayout*);
    void setGain(float);

    // === Process Audio ======================================================
    void reset(int blockSize);
    float processSample(float);
private:
    std::string displayName;
    float minimum;
    float maximum;
    float defaultVal;
    juce::SmoothedValue<float> smoothGain;
};