#include "GainFilter.h"

// === Lifecycle ==============================================================
GainFilter::GainFilter
(std::string idPostfix, std::string display, float min, float max,
float defaultValue)
    : parameterName("gain-" + idPostfix), displayName(display), minimum(min),
    maximum(max), defaultVal(defaultValue)
{
    smoothGain.setCurrentAndTargetValue(0);
}

// === Parameter Listener =====================================================
void GainFilter::listenTo(juce::AudioProcessorValueTreeState* tree)
{
    tree->addParameterListener(parameterName, this);
    setGain(*tree->getRawParameterValue(parameterName));
}

void GainFilter::parameterChanged(const juce::String& parameter, float value)
{
    juce::ignoreUnused(parameter);
    setGain(value);
}

// === Set Parameters =========================================================
void GainFilter::addParameters(ParameterLayout* parameters)
{
    parameters->add(std::make_unique<Parameter>(
        parameterName,
        displayName,
        juce::NormalisableRange<float>(minimum, maximum, 0.1f),
        defaultVal
    ));
}

void GainFilter::setGain(float value)
{
    smoothGain.setTargetValue(value);
}

// === Process Audio ==========================================================
void GainFilter::reset(int blockSize)
{
    smoothGain.reset(blockSize);
}

float GainFilter::processSample(float sample)
{
    if (smoothGain.isSmoothing())
        return sample * pow(10.0f, smoothGain.getNextValue() / 20.0f);
    else
        return sample * pow(10.0f, smoothGain.getCurrentValue() / 20.0f);
}