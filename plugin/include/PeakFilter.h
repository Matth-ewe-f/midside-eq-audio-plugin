#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace dsp = juce::dsp;

class PeakFilter
{
public:
    // === Lifecycle ==========================================================
    PeakFilter(float frequency);
    ~PeakFilter();

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setFrequency(float);
    void setGain(float);
    void setQFactor(float);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);

private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    // juce::SmoothedValue<float> smoothGain;
    // juce::SmoothedValue<float> smoothQ;
    float gain;
    float q;
    double sampleRate;

    // === Private Helper =====================================================
    void setFilterParameters(float freq, float gain, float q);
};