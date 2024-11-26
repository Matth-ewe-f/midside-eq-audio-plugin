#include "HighPassFilter.h"
#include <format>

using Parameter = juce::AudioProcessorValueTreeState::Parameter;
using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
HighPassFilter::HighPassFilter(std::string nameArg, std::string parameterText)
    : CtmFilter(nameArg, parameterText), order(1), pendingOrder(-1),
    fadeSamples(-1), sampleRate(48000)
{
    filterOne.coefficients
        = Coefficients::makeHighPass(sampleRate, 20, 0.71f);
    filterTwo.coefficients
        = Coefficients::makeHighPass(sampleRate, 20, 0.71f);
    filterThree.coefficients
        = Coefficients::makeHighPass(sampleRate, 20, 0.71f);
    filterFour.coefficients
        = Coefficients::makeFirstOrderHighPass(sampleRate, 20);
    smoothFrequency.setCurrentAndTargetValue(20);
    smoothBypass.setCurrentAndTargetValue(1);
}

// === Parameter Information ==================================================
void HighPassFilter::parameterChanged(const juce::String& param, float value)
{
    if (param.compare(name + "-" + onOffParam.idPostfix) == 0)
        smoothBypass.setTargetValue(value);
    else if (param.compare(name + "-" + freqParam.idPostfix) == 0)
        setFrequency(value);
    else if (param.compare(name + "-" + falloffParam.idPostfix) == 0)
        setOrder((int) value / 6);
}

// === Parameter Functions ====================================================
void HighPassFilter::reset(double newSampleRate, int samplesPerBlock)
{
    filterOne.reset();
    filterTwo.reset();
    filterThree.reset();
    filterFour.reset();
    smoothFrequency.reset(samplesPerBlock);
    smoothBypass.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void HighPassFilter::setBypass(bool isBypassed)
{
    smoothBypass.setTargetValue(isBypassed ? 0 : 1);
}

void HighPassFilter::setFrequency(float newFrequency)
{
    smoothFrequency.setTargetValue(newFrequency);
}

void HighPassFilter::setOrder(int newOrder)
{
    if (newOrder <= 1)
        pendingOrder = 1;
    else if (newOrder >= 7)
        pendingOrder = 7;
    else
        pendingOrder = newOrder;
    fadeSamples = 0;
}

// === Process Audio ==========================================================
void HighPassFilter::prepare(const dsp::ProcessSpec& spec)
{
    filterOne.prepare(spec);
    filterTwo.prepare(spec);
    filterThree.prepare(spec);
    filterFour.prepare(spec);
}

float HighPassFilter::processSample(float sample)
{
    if (smoothBypass.getCurrentValue() <= 0 && !smoothBypass.isSmoothing())
        return sample;
    if (smoothFrequency.isSmoothing())
        updateFilters(smoothFrequency.getNextValue());
    float result = sample;
    if (filterOneEnabled())
        result = filterOne.processSample(result);
    if (filterTwoEnabled())
        result = filterTwo.processSample(result);
    if (filterThreeEnabled())
        result = filterThree.processSample(result);
    if (filterFourEnabled())
        result = filterFour.processSample(result);
    if (pendingOrder != -1 || fadeSamples >= 0)
    {
        float gain = std::abs((float)fadeSamples++ - fadeLength) / fadeLength;
        gain = (gain * 1.2f) - 0.2f;
        if (gain < 0)
            gain = 0;
        result *= gain;
        if (fadeSamples == fadeLength)
        {
            delayedUpdateOrder();
        }
        if (fadeSamples == fadeLength * 2)
        {
            fadeSamples = -1;
        }
    }
    if (smoothBypass.isSmoothing())
    {
        float p = smoothBypass.getNextValue();
        result = (result * p) + (sample * (1 - p));
    }
    return result;
}

// === Overriden from CtmFilter ===============================================
void HighPassFilter::getParameters(std::vector<ParameterFields>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(freqParam);
    parameters.push_back(falloffParam);
    parameters.push_back(resParam);
}

// === Private Helper =========================================================
void HighPassFilter::updateFilters(float f)
{
    if (filterOneEnabled())
    {
        float q = getQForFilter(1);
        filterOne.coefficients = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterTwoEnabled())
    {
        float q = getQForFilter(2);
        filterTwo.coefficients = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterThreeEnabled())
    {
        float q = getQForFilter(3);
        filterThree.coefficients
            = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterFourEnabled())
    {
        filterFour.coefficients
            = Coefficients::makeFirstOrderHighPass(sampleRate, f);
    }
}

void HighPassFilter::delayedUpdateOrder()
{
    order = pendingOrder;
    pendingOrder = -1;
    float freq = smoothFrequency.getTargetValue();
    smoothFrequency.setCurrentAndTargetValue(freq);
    updateFilters(freq);
}

float HighPassFilter::getQForFilter(int filter)
{
    if (filter == 1)
    {
        if (order == 2)
            return 0.707f;
        else if (order == 3)
            return 1;
        else if (order == 4)
            return 0.541f;
        else if (order == 5)
            return 0.618f;
        else if (order == 6)
            return 0.518f;
        else if (order == 7)
            return 0.555f;
    }
    else if (filter == 2)
    {
        if (order == 4)
            return 1.307f;
        else if (order == 5)
            return 1.618f;
        else if (order == 6)
            return 0.707f;
        else if (order == 7)
            return 0.802f;
    }
    else if (filter == 3)
    {
        if (order == 6)
            return 1.932f;
        else if (order == 7)
            return 2.247f;
    }
    // this shouldn't happen, but return a neutral q just in case
    return 0.707f;
}