#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
namespace dsp = juce::dsp;

class HighPassFilter : public juce::AudioProcessorValueTreeState::Listener
{
public:
    // === Lifecycle ==========================================================
    HighPassFilter();

    // === State Tree-Related =================================================
    void setListenTo(juce::AudioProcessorValueTreeState*, std::string);
    void parameterChanged(const juce::String&, float) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setOrder(int);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);

    // === Static Functions ===================================================
    static void addParameters
    (ParameterLayout*, std::string prefix, std::string channels);
private:
    // === Private Variables ==================================================
    // filters 1~3 are always second order, filter 4 is always first order
    // different combinations can create a filter of any order from 1 to 7
    dsp::IIR::Filter<float> filterOne;
    dsp::IIR::Filter<float> filterTwo;
    dsp::IIR::Filter<float> filterThree;
    dsp::IIR::Filter<float> filterFour;
    juce::SmoothedValue<float> smoothFrequency;
    juce::SmoothedValue<float> smoothBypass;
    int order;
    int pendingOrder;
    int fadeSamples;
    double sampleRate;
    juce::AudioProcessorValueTreeState* listenTo;
    std::string name;
    
    // === Static Constants ===================================================
    inline static const juce::NormalisableRange<float> onOffRange
        { juce::NormalisableRange<float>(0, 1, 1) };
    inline static const juce::NormalisableRange<float> freqRange
        { juce::NormalisableRange<float>(20, 20000, 0.1f, 0.35f) };
    inline static const juce::NormalisableRange<float> falloffRange
        { juce::NormalisableRange<float>(6, 36, 6, 1) };
    inline static const juce::NormalisableRange<float> resRange
        { juce::NormalisableRange<float>(0.5, 10, 0.01f, 0.7f) };
    inline static const int fadeLength { 1000 };

    // === Private Helper =====================================================
    void updateFilters(float frequency);
    void delayedUpdateOrder();
    float getQForFilter(int);
    inline bool filterOneEnabled() { return order >= 2; }
    inline bool filterTwoEnabled() { return order >= 4; }
    inline bool filterThreeEnabled() { return order >= 6; }
    inline bool filterFourEnabled() { return order % 2 == 1; }
};