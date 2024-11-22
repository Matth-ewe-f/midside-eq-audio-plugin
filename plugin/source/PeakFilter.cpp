#include "PeakFilter.h"

using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
PeakFilter::PeakFilter(float frequency) : gain(1), q(0.707f), sampleRate(48000)
{
    setFilterParameters(frequency, gain, q);
    smoothFrequency.setCurrentAndTargetValue(frequency);
}

PeakFilter::~PeakFilter() { }

// === Set Parameters =========================================================
void PeakFilter::reset(double newSampleRate, int samplesPerBlock)
{
    filter.reset();
    smoothFrequency.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void PeakFilter::setFrequency(float frequency)
{
    smoothFrequency.setTargetValue(frequency);
}

void PeakFilter::setGain(float newGain)
{
    gain = newGain;
    setFilterParameters(smoothFrequency.getCurrentValue(), gain, q);
}

void PeakFilter::setQFactor(float newQ)
{
    q = newQ;
    setFilterParameters(smoothFrequency.getCurrentValue(), gain, q);
}

// === Process Audio ==========================================================
void PeakFilter::prepare(const dsp::ProcessSpec& spec)
{
    filter.prepare(spec);
}

float PeakFilter::processSample(float sample)
{
    if (smoothFrequency.isSmoothing())
        setFilterParameters(smoothFrequency.getNextValue(), gain, q);
    return filter.processSample(sample);
}

// === Private Helper =========================================================
void PeakFilter::setFilterParameters(float newFreq, float newGain, float newQ)
{
    filter.coefficients
        = Coefficients::makePeakFilter(sampleRate, newFreq, newQ, newGain);
}