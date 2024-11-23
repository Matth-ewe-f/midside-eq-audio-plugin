#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace dsp = juce::dsp;

class LowPassFilter
{
public:
    // === Lifecycle ==========================================================
    LowPassFilter();
    ~LowPassFilter();

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setOrder(int);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);
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