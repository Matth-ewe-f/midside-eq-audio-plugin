#include "HighPassFilter.h"
#include <format>

using Parameter = juce::AudioProcessorValueTreeState::Parameter;
using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
HighPassFilter::HighPassFilter(std::string nameArg, std::string parameterText)
    : CtmFilter(nameArg, parameterText), order(1), pendingOrder(-1),
    fadeSamples(-1), isShelf(false), sampleRate(48000)
{
    smoothFrequency.setCurrentAndTargetValue(20);
    smoothBypass.setCurrentAndTargetValue(0);
    smoothGain.setCurrentAndTargetValue(0);
    smoothResonance.setCurrentAndTargetValue(0.71f);
    updateFilters();
}

// === Parameter Information ==================================================
void HighPassFilter::onChangedParameter(const juce::String& param, float value)
{
    if (param.compare(onOffParam.idPostfix) == 0)
        setBypass(value <= 0);
    else if (param.compare(freqParam.idPostfix) == 0)
        setFrequency(value);
    else if (param.compare(falloffParam.idPostfix) == 0)
        setOrder((int) value / 6);
    else if (param.compare(resParam.idPostfix) == 0)
        setResonance(value);
    else if (param.compare(shelfModeParam.idPostfix) == 0)
        setIsShelf(value >= 1);
    else if (param.compare(shelfGainParam.idPostfix) == 0)
        setShelfGain(value);
}

void HighPassFilter::getParameters(std::vector<ParameterBlueprint>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(shelfModeParam);
    parameters.push_back(freqParam);
    parameters.push_back(falloffParam);
    parameters.push_back(shelfGainParam);
    parameters.push_back(resParam);
}

void HighPassFilter::getMagnitudes
(const double* frequencies, double* magnitudes, size_t len)
{
    for (size_t i = 0;i < len;i++)
        magnitudes[i] = 1;
    if (smoothBypass.getTargetValue() <= 0)
    {
        return;
    }
    double* perFilter = new double[len];
    float freq = smoothFrequency.getTargetValue();
    int curOrder = pendingOrder == -1 ? order : pendingOrder;
    float res = smoothResonance.getTargetValue();
    if (filterOneEnabled(curOrder))
    {
        auto coefficients = Coefficients::makeHighPass(
            sampleRate * 2, freq, getQForFilter(1, curOrder, res)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterTwoEnabled(curOrder))
    {
        auto coefficients = Coefficients::makeHighPass(
            sampleRate * 2, freq, getQForFilter(2, curOrder, res)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterThreeEnabled(curOrder))
    {
        auto coefficients = Coefficients::makeHighPass(
            sampleRate * 2, freq, getQForFilter(3, curOrder, res)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterFourEnabled(curOrder))
    {
        juce::ReferenceCountedObjectPtr<Coefficients> coefficients;
        if (isShelf)
        {
            float gain = smoothGain.getTargetValue();
            coefficients = Coefficients::makeLowShelf(
                sampleRate * 2, freq, res, pow(10.0f, gain / 20.f)
            );
        }
        else
        {
            coefficients = Coefficients::makeFirstOrderHighPass(
                sampleRate * 2, freq
            );
        }
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
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
    smoothGain.reset(samplesPerBlock);
    smoothResonance.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void HighPassFilter::setBypass(bool isBypassed)
{
    if (isProcessing())
        smoothBypass.setTargetValue(isBypassed ? 0 : 1);
    else
        smoothBypass.setCurrentAndTargetValue(isBypassed ? 0 : 1);
}

void HighPassFilter::setFrequency(float newFrequency)
{
    if (isProcessing())
        smoothFrequency.setTargetValue(newFrequency);
    else
    {
        smoothFrequency.setCurrentAndTargetValue(newFrequency);
        updateFilters();
    }
}

void HighPassFilter::setOrder(int newOrder)
{
    if (isProcessing())
    {
        if (newOrder <= 1)
            pendingOrder = 1;
        else if (newOrder >= 7)
            pendingOrder = 7;
        else
            pendingOrder = newOrder;
        if (fadeSamples < 0)
        {
            fadeSamples = 0;
        }
        else if (fadeSamples >= fadeLength)
        {
            fadeSamples = (2 * fadeLength) - fadeSamples;
        }
        // if fadeSamples > 0 && < fadeLength, nothing needs to be done
    }
    else
    {
        order = newOrder;
        updateFilters();
    }
}

void HighPassFilter::setResonance(float newRes)
{
    if (isProcessing())
        smoothResonance.setTargetValue(newRes);
    else
    {
        smoothResonance.setCurrentAndTargetValue(newRes);
        updateFilters();
    }
}

void HighPassFilter::setIsShelf(bool shelf)
{
    isShelf = shelf;
    updateFilters();
}

void HighPassFilter::setShelfGain(float gain)
{
    if (isProcessing())
        smoothGain.setTargetValue(gain);
    else
    {
        smoothGain.setCurrentAndTargetValue(gain);
        updateFilters();
    }
}

// === Linking ================================================================
void HighPassFilter::setParamsOnLink(std::string paramName)
{
    std::atomic<float>* param;
    std::string paramId;
    paramId = paramName + "-" + onOffParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setBypass(*param <= 0);
    paramId = paramName + "-" + freqParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setFrequency(*param);
    paramId = paramName + "-" + falloffParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setOrder((int)(*param) / 6);
    paramId = paramName + "-" + resParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setResonance(*param);
    paramId = paramName + "-" + shelfModeParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setIsShelf(*param >= 1);
    paramId = paramName + "-" + shelfGainParam.idPostfix;
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setShelfGain(*param);
}

// === Process Audio ==========================================================
void HighPassFilter::prepare(const dsp::ProcessSpec& spec)
{
    filterOne.prepare(spec);
    filterTwo.prepare(spec);
    filterThree.prepare(spec);
    filterFour.prepare(spec);
}

float HighPassFilter::processSampleProtected(float sample)
{
    if (smoothBypass.getCurrentValue() <= 0 && !smoothBypass.isSmoothing())
        return sample;
    if (anythingSmoothing())
    {
        float freq = smoothFrequency.getNextValue();
        float gain = smoothGain.getNextValue();
        float res = smoothResonance.getNextValue();
        updateFilters(freq, gain, res);
    }  
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

// === Private Helper =========================================================
void HighPassFilter::updateFilters()
{
    float freq = smoothFrequency.getCurrentValue();
    float gain = smoothGain.getCurrentValue();
    float res = smoothResonance.getCurrentValue();
    updateFilters(freq, gain, res);
}

void HighPassFilter::updateFilters(float f, float gain, float resonance)
{
    if (filterOneEnabled())
    {
        float q = getQForFilter(1, order, resonance);
        filterOne.coefficients = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterTwoEnabled())
    {
        float q = getQForFilter(2, order, resonance);
        filterTwo.coefficients = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterThreeEnabled())
    {
        float q = getQForFilter(3, order, resonance);
        filterThree.coefficients
            = Coefficients::makeHighPass(sampleRate, f, q);
    }
    if (filterFourEnabled())
    {
        if (isShelf)
        {
            filterFour.coefficients = Coefficients::makeLowShelf(
                sampleRate, f, resonance, pow(10.0f, gain / 20.f)
            );
        }
        else
        {
            filterFour.coefficients
                = Coefficients::makeFirstOrderHighPass(sampleRate, f);
        }
    }
}

void HighPassFilter::delayedUpdateOrder()
{
    order = pendingOrder;
    pendingOrder = -1;
    updateFilters();
}

bool HighPassFilter::anythingSmoothing()
{
    return smoothFrequency.isSmoothing() || smoothGain.isSmoothing()
        || smoothResonance.isSmoothing();
}

float HighPassFilter::getQForFilter(int filter, int filterOrder, float res)
{
    // result should be overwritten, but default to a neutral q just in case
    float result = 0.707f;
    if (filter == 1)
    {
        if (filterOrder == 2)
            result = 0.707f;
        else if (filterOrder == 3)
            result = 1;
        else if (filterOrder == 4)
            result = 0.541f;
        else if (filterOrder == 5)
            result = 0.618f;
        else if (filterOrder == 6)
            result = 0.518f;
        else if (filterOrder == 7)
            result = 0.555f;
    }
    else if (filter == 2)
    {
        if (filterOrder == 4)
            result = 1.307f;
        else if (filterOrder == 5)
            result = 1.618f;
        else if (filterOrder == 6)
            result = 0.707f;
        else if (filterOrder == 7)
            result = 0.802f;
    }
    else if (filter == 3)
    {
        if (filterOrder == 6)
            result = 1.932f;
        else if (filterOrder == 7)
            result = 2.247f;
    }
    int highestBiquad = filterOrder / 2;
    if (filter == highestBiquad)
    {
        result *= res / 0.707f;
    }
    return result;
}

void HighPassFilter::combineMagnitudes
(double* totals, const double* toCombine, size_t len)
{
    for (size_t i = 0;i < len;i++)
        totals[i] *= toCombine[i];
}