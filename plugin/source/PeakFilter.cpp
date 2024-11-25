#include "PeakFilter.h"

using Parameter = juce::AudioProcessorValueTreeState::Parameter;
using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
PeakFilter::PeakFilter(float frequency)
    : gain(1), q(0.707f), sampleRate(48000), listenTo(nullptr), name("")
{
    setFilterParameters(frequency, gain, q);
    smoothFrequency.setCurrentAndTargetValue(frequency);
    smoothBypass.setCurrentAndTargetValue(1);
}

// === State Tree Listener ====================================================
void PeakFilter::setListenTo
(juce::AudioProcessorValueTreeState* tree, std::string newName)
{
    // stop listening to the old tree
    if (listenTo != nullptr)
    {
        listenTo->removeParameterListener(name + "-on", this);
        listenTo->removeParameterListener(name + "-freq", this);
        listenTo->removeParameterListener(name + "-gain", this);
        listenTo->removeParameterListener(name + "-q", this);
    }
    // start listening to the provided tree
    tree->addParameterListener(newName + "-on", this);
    tree->addParameterListener(newName + "-freq", this);
    tree->addParameterListener(newName + "-gain", this);
    tree->addParameterListener(newName + "-q", this);
    listenTo = tree;
    name = newName;
}

void PeakFilter::parameterChanged(const juce::String& param, float value)
{
    if (param.compare(name + "-on") == 0)
        smoothBypass.setTargetValue(value);
    else if (param.compare(name + "-freq") == 0)
        setFrequency(value);
    else if (param.compare(name + "-gain") == 0)
        setGain(pow(10.0f, value / 20));
    else if (param.compare(name + "-q") == 0)
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

// === Static Functions =======================================================
void PeakFilter::addParameters
(ParameterLayout* parameters, std::string prefix, std::string number,
std::string channels, float defaultFreq)
{
    std::string namePrefix = "Peak Filter " + number;
    parameters->add(std::make_unique<Parameter>(
        prefix + "-on", namePrefix + " On/Off " + channels, onOffRange, 1
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-freq", namePrefix + " Frequency " + channels, freqRange,
        defaultFreq
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-gain", namePrefix + " Gain " + channels, gainRange, 0
    ));
    parameters->add(std::make_unique<Parameter>(
        prefix + "-q", namePrefix + " Q Factor " + channels, qRange, 0.71f
    ));
}

// === Private Helper =========================================================
void PeakFilter::setFilterParameters(float newFreq, float newGain, float newQ)
{
    filter.coefficients
        = Coefficients::makePeakFilter(sampleRate, newFreq, newQ, newGain);
}