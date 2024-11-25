#include "PeakFilter.h"

using Parameter = juce::AudioProcessorValueTreeState::Parameter;
using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
PeakFilter::PeakFilter
(std::string nameArg, std::string parameterText, float defaultFrequency)
    : CtmFilter(nameArg, parameterText), defaultFreq(defaultFrequency),
    gain(1), q(0.707f), sampleRate(48000)
{
    setFilterParameters(defaultFrequency, gain, q);
    smoothFrequency.setCurrentAndTargetValue(defaultFrequency);
    smoothBypass.setCurrentAndTargetValue(1);
}

// === Parameter Information ==================================================
void PeakFilter::parameterChanged(const juce::String& param, float value)
{
    if (param.compare(name + "-" + onOffParam.idPostfix) == 0)
        smoothBypass.setTargetValue(value);
    else if (param.compare(name + "-" + freqParamIdPostfix) == 0)
        setFrequency(value);
    else if (param.compare(name + "-" + gainParam.idPostfix) == 0)
        setGain(pow(10.0f, value / 20));
    else if (param.compare(name + "-" + qParam.idPostfix) == 0)
        setQFactor(value);
}

// === Set Parameters =========================================================
void PeakFilter::reset(double newSampleRate, int samplesPerBlock)
{
    filter.reset();
    smoothFrequency.reset(samplesPerBlock);
    smoothBypass.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void PeakFilter::setBypass(bool isBypassed)
{
    smoothBypass.setTargetValue(isBypassed ? 0 : 1);
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
    if (smoothBypass.getCurrentValue() <= 0 && !smoothBypass.isSmoothing())
        return sample;
    if (smoothFrequency.isSmoothing())
        setFilterParameters(smoothFrequency.getNextValue(), gain, q);
    float result = filter.processSample(sample);
    if (smoothBypass.isSmoothing())
    {
        float p = smoothBypass.getNextValue();
        result = (result * p) + (sample * (1 - p));
    }
    return result;
}

// === Overriden from CtmFilter ===============================================
void PeakFilter::getParameters(std::vector<ParameterFields>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(gainParam);
    parameters.push_back(qParam);
    parameters.push_back(getFreqParameterFields());
}

// === Private Helper =========================================================
void PeakFilter::setFilterParameters(float newFreq, float newGain, float newQ)
{
    filter.coefficients
        = Coefficients::makePeakFilter(sampleRate, newFreq, newQ, newGain);
}