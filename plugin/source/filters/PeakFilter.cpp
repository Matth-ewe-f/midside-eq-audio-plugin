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
void PeakFilter::onChangedParameter(const juce::String& param, float value)
{
    if (param.compare(onOffParam.idPostfix) == 0)
        setBypass(value <= 0);
    else if (param.compare(freqParamIdPostfix) == 0)
        setFrequency(value);
    else if (param.compare(gainParam.idPostfix) == 0)
        setGain(pow(10.0f, value / 20));
    else if (param.compare(qParam.idPostfix) == 0)
        setQFactor(value);
}

void PeakFilter::getParameters(std::vector<ParameterFields>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(gainParam);
    parameters.push_back(qParam);
    parameters.push_back(getFreqParameterFields());
}

void PeakFilter::getMagnitudes
(const double* frequencies, double* magnitudes, size_t len)
{
    if (smoothBypass.getTargetValue() <= 0)
    {
        for (size_t i = 0;i < len;i++)
        {
            magnitudes[i] = 1;
        }
    }
    else
    {
        auto coefficients = Coefficients::makePeakFilter(
            sampleRate * 2, smoothFrequency.getTargetValue(), q, gain
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, magnitudes, len, sampleRate * 2
        );
    }
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
    if (isProcessing())
        smoothBypass.setTargetValue(isBypassed ? 0 : 1);
    else
        smoothBypass.setCurrentAndTargetValue(isBypassed ? 0 : 1);
}

void PeakFilter::setFrequency(float frequency)
{
    if (isProcessing())
        smoothFrequency.setTargetValue(frequency);
    else
    {
        smoothFrequency.setCurrentAndTargetValue(frequency);
        setFilterParameters(frequency, gain, q);
    }
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

// === Linking ================================================================
void PeakFilter::setParamsOnLink(std::string paramName)
{
    std::atomic<float>* param;
    std::string paramId;
    paramId = paramName + "-" + onOffParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setBypass(*param <= 0);
    paramId = paramName + "-" + freqParamIdPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setFrequency(*param);
    paramId = paramName + "-" + gainParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setGain(pow(10.0f, *param / 20));
    paramId = paramName + "-" + qParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setQFactor(*param);
}

// === Process Audio ==========================================================
void PeakFilter::prepare(const dsp::ProcessSpec& spec)
{
    filter.prepare(spec);
}

float PeakFilter::processSampleProtected(float sample)
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

// === Private Helper =========================================================
void PeakFilter::setFilterParameters(float newFreq, float newGain, float newQ)
{
    filter.coefficients
        = Coefficients::makePeakFilter(sampleRate, newFreq, newQ, newGain);
}