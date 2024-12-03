#include "GainFilter.h"

// === Lifecycle ==============================================================
GainFilter::GainFilter(std::string nameArg, std::string parameterText)
    : CtmFilter(nameArg, parameterText)
{
    smoothGain.setCurrentAndTargetValue(0);
    smoothBypass.setCurrentAndTargetValue(1);
}

// === Parameters =============================================================
void GainFilter::onChangedParameter(const juce::String& parameter, float value)
{
    if (parameter.compare(name + "-" + gainParam.idPostfix) == 0)
        setGain(value);
    else if (parameter.compare(name + "-" + onOffParam.idPostfix) == 0)
        setBypass(value <= 0);
}

void GainFilter::getParameters(std::vector<ParameterFields>& params)
{
    params.push_back(onOffParam);
    params.push_back(gainParam);
}

void GainFilter::getMagnitudes
(const double* frequencies, double* magnitudes, size_t len)
{
    juce::ignoreUnused(frequencies);
    for (size_t i = 0;i < len;i++)
    {
        if (smoothBypass.getCurrentValue() <= 0)
            magnitudes[i] = 1;
        else
            magnitudes[i] = pow(10, smoothGain.getTargetValue() / 20);
    }
}

void GainFilter::setGain(float value)
{
    if (isProcessing())
        smoothGain.setTargetValue(value);
    else
        smoothGain.setCurrentAndTargetValue(value);    
}

void GainFilter::setBypass(bool b)
{
    if (isProcessing())
        smoothBypass.setTargetValue(b ? 0 : 1);
    else
        smoothBypass.setCurrentAndTargetValue(b ? 0 : 1);
}

// === Process Audio ==========================================================
void GainFilter::reset(int blockSize)
{
    smoothGain.reset(blockSize);
    smoothBypass.reset(blockSize);
}

float GainFilter::processSampleProtected(float sample)
{
    if (!smoothBypass.isSmoothing() && smoothBypass.getCurrentValue() <= 0)
        return sample;
    float result;
    if (smoothGain.isSmoothing())
        result = sample * pow(10.0f, smoothGain.getNextValue() / 20.0f);
    else
        result = sample * pow(10.0f, smoothGain.getCurrentValue() / 20.0f);
    if (smoothBypass.isSmoothing())
    {
        float p = smoothBypass.getNextValue();
        result = (result * p) + (sample * (1 - p));
    }
    return result;
}