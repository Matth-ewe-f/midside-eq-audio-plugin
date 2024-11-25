#include "LowPassFilter.h"

using Parameter = juce::AudioProcessorValueTreeState::Parameter;
using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
LowPassFilter::LowPassFilter()
    : order(1), pendingOrder(-1), fadeSamples(-1), sampleRate(48000),
    listenTo(nullptr), name("")
{
    filterOne.coefficients
        = Coefficients::makeLowPass(sampleRate, 20000, 0.71f);
    filterTwo.coefficients
        = Coefficients::makeLowPass(sampleRate, 20000, 0.71f);
    filterThree.coefficients
        = Coefficients::makeLowPass(sampleRate, 20000, 0.71f);
    filterFour.coefficients
        = Coefficients::makeFirstOrderLowPass(sampleRate, 20000);
    smoothFrequency.setCurrentAndTargetValue(20000);
    smoothBypass.setCurrentAndTargetValue(1);
}

// === State Tree Listener ====================================================
void LowPassFilter::setListenTo
(juce::AudioProcessorValueTreeState* tree, std::string newName)
{
    // stop listening to the old tree (if there is one)
    if (listenTo != nullptr)
    {
        listenTo->removeParameterListener(name + "-on", this);
        listenTo->removeParameterListener(name + "-freq", this);
        listenTo->removeParameterListener(name + "-falloff", this);
        listenTo->removeParameterListener(name + "-res", this);
    }
    // begin listening to the provided tree
    tree->addParameterListener(newName + "-on", this);
    tree->addParameterListener(newName + "-freq", this);
    tree->addParameterListener(newName + "-falloff", this);
    tree->addParameterListener(newName + "-res", this);
    listenTo = tree;
    name = newName;
}

void LowPassFilter::parameterChanged(const juce::String& param, float value)
{
    if (param.compare(name + "-on") == 0)
        smoothBypass.setTargetValue(value);
    else if (param.compare(name + "-freq") == 0)
        setFrequency(value);
    else if (param.compare(name + "-falloff") == 0)
        setOrder((int) value / 6);
}

// === Set Parameters =========================================================
void LowPassFilter::reset(double newSampleRate, int samplesPerBlock)
{
    filterOne.reset();
    filterTwo.reset();
    filterThree.reset();
    filterFour.reset();
    smoothFrequency.reset(samplesPerBlock);
    smoothBypass.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void LowPassFilter::setBypass(bool isBypassed)
{
    smoothBypass.setTargetValue(isBypassed ? 0 : 1);
}

void LowPassFilter::setFrequency(float newFrequency)
{
    smoothFrequency.setTargetValue(newFrequency);
}

void LowPassFilter::setOrder(int newOrder)
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
void LowPassFilter::prepare(const dsp::ProcessSpec& spec)
{
    filterOne.prepare(spec);
    filterTwo.prepare(spec);
    filterThree.prepare(spec);
    filterFour.prepare(spec);
}

float LowPassFilter::processSample(float sample)
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

// === Static Functions ======================================================
void LowPassFilter::addParameters
(ParameterLayout* parameters, std::string prefix, std::string channels)
{
    parameters->add(std::make_unique<Parameter>(
        prefix + "-on", "Low-Pass On/Off " + channels, onOffRange, 1
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-freq", "Low-Pass Frequency " + channels, freqRange, 20000
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-falloff", "Low-Pass Falloff " + channels, falloffRange, 6
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-res", "Low-Pass Resonance " + channels, resRange, 0.71f
    ));
}

// === Private Helper =========================================================
void LowPassFilter::updateFilters(float f)
{
    if (filterOneEnabled())
    {
        float q = getQForFilter(1);
        filterOne.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterTwoEnabled())
    {
        float q = getQForFilter(2);
        filterTwo.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterThreeEnabled())
    {
        float q = getQForFilter(3);
        filterThree.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterFourEnabled())
    {
        filterFour.coefficients
            = Coefficients::makeFirstOrderLowPass(sampleRate, f);
    }
}

void LowPassFilter::delayedUpdateOrder()
{
    order = pendingOrder;
    pendingOrder = -1;
    float freq = smoothFrequency.getTargetValue();
    smoothFrequency.setCurrentAndTargetValue(freq);
    updateFilters(freq);
}

float LowPassFilter::getQForFilter(int filter)
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