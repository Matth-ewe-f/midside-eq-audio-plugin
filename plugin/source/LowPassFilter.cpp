#include "LowPassFilter.h"

using Coefficients = juce::dsp::IIR::Coefficients<float>;

LowPassFilter::LowPassFilter(double sampleRate) : lastSampleRate(sampleRate)
{
    filter.coefficients = Coefficients::makeLowPass(lastSampleRate, 20000);
    smoothFrequency.setCurrentAndTargetValue(20000);
}

LowPassFilter::~LowPassFilter() { }

void LowPassFilter::reset(double newSampleRate, int samplesPerBlock)
{
    filter.reset();
    smoothFrequency.reset(samplesPerBlock);
    lastSampleRate = newSampleRate;
}

void LowPassFilter::setFrequency(float newFrequency)
{
    smoothFrequency.setTargetValue(newFrequency);
}

void LowPassFilter::prepare(const dsp::ProcessSpec& spec)
{
    filter.prepare(spec);
}

float LowPassFilter::processSample(float sample)
{
    if (smoothFrequency.isSmoothing())
    {
        float freq = smoothFrequency.getNextValue();
        filter.coefficients = Coefficients::makeLowPass(lastSampleRate, freq);
    }
    return filter.processSample(sample);
}