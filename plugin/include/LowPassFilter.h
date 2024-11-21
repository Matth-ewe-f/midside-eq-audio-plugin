#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace dsp = juce::dsp;

class LowPassFilter
{
public:
    LowPassFilter(double sampleRate);
    ~LowPassFilter();

    void reset(double newSampleRate, int samplesPerBlock);
    void setFrequency(float);
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);
private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    double lastSampleRate;
};