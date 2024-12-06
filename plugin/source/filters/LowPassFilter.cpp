#include "LowPassFilter.h"

using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
LowPassFilter::LowPassFilter(std::string nameArg, std::string parameterText)
    : CtmFilter(nameArg, parameterText), order(1), pendingOrder(-1),
    fadeSamples(-1), isShelf(false), sampleRate(48000)
{
    smoothFrequency.setCurrentAndTargetValue(20000);
    smoothBypass.setCurrentAndTargetValue(0);
    smoothGain.setCurrentAndTargetValue(0);
    smoothResonance.setCurrentAndTargetValue(0.71f);
    updateFilters();
}

// === Parameter Information ==================================================
void LowPassFilter::onChangedParameter(const juce::String& param, float value)
{
    if (param.compare(name + "-" + onOffParam.idPostfix) == 0)
        smoothBypass.setTargetValue(value);
    else if (param.compare(name + "-" + shelfModeParam.idPostfix) == 0)
        setIsShelf(value >= 1);
    else if (param.compare(name + "-" + freqParam.idPostfix) == 0)
        setFrequency(value);
    else if (param.compare(name + "-" + falloffParam.idPostfix) == 0)
        setOrder((int) value / 6);
    else if (param.compare(name + "-" + shelfGainParam.idPostfix) == 0)
        setShelfGain(value);
    else if (param.compare(name + "-" + resParam.idPostfix) == 0)
        setResonance(value);
}

void LowPassFilter::getParameters(std::vector<ParameterFields>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(freqParam);
    parameters.push_back(falloffParam);
    parameters.push_back(resParam);
    parameters.push_back(shelfModeParam);
    parameters.push_back(shelfGainParam);
}

void LowPassFilter::getMagnitudes
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
    if (filterOneEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(1, curOrder)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterTwoEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(2, curOrder)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterThreeEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(3, curOrder)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterFourEnabled())
    {
        juce::ReferenceCountedObjectPtr<Coefficients> coefficients;
        if (isShelf)
        {
            float res = smoothResonance.getTargetValue();
            float gain = smoothGain.getTargetValue();
            coefficients = Coefficients::makeHighShelf(
                sampleRate * 2, freq, res, pow(10.0f, gain / 20.f)
            );
        }
        else
        {
            coefficients = Coefficients::makeFirstOrderLowPass(
                sampleRate * 2, freq
            );
        }
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
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
    smoothGain.reset(samplesPerBlock);
    smoothResonance.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void LowPassFilter::setBypass(bool isBypassed)
{
    if (isProcessing())
        smoothBypass.setTargetValue(isBypassed ? 0 : 1);
    else
        smoothBypass.setCurrentAndTargetValue(isBypassed ? 0 : 1);
}

void LowPassFilter::setFrequency(float newFrequency)
{
    if (isProcessing())
        smoothFrequency.setTargetValue(newFrequency);
    else
    {
        smoothFrequency.setCurrentAndTargetValue(newFrequency);
        updateFilters();
    }
}

void LowPassFilter::setOrder(int newOrder)
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

void LowPassFilter::setIsShelf(bool shelf)
{
    isShelf = shelf;
    updateFilters();
}

void LowPassFilter::setShelfGain(float gain)
{
    if (isProcessing())
        smoothGain.setTargetValue(gain);
    else
    {
        smoothGain.setCurrentAndTargetValue(gain);
        updateFilters();
    }
}

void LowPassFilter::setResonance(float res)
{
    if (isProcessing())
        smoothResonance.setTargetValue(res);
    else
    {
        smoothResonance.setCurrentAndTargetValue(res);
        updateFilters();
    }
}

// === Process Audio ==========================================================
void LowPassFilter::prepare(const dsp::ProcessSpec& spec)
{
    filterOne.prepare(spec);
    filterTwo.prepare(spec);
    filterThree.prepare(spec);
    filterFour.prepare(spec);
}

float LowPassFilter::processSampleProtected(float sample)
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
void LowPassFilter::updateFilters()
{
    float freq = smoothFrequency.getCurrentValue();
    float gain = smoothGain.getCurrentValue();
    float res = smoothResonance.getCurrentValue();
    updateFilters(freq, gain, res);
}

void LowPassFilter::updateFilters(float f, float gain, float resonance)
{
    if (filterOneEnabled())
    {
        float q = getQForFilter(1, order);
        filterOne.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterTwoEnabled())
    {
        float q = getQForFilter(2, order);
        filterTwo.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterThreeEnabled())
    {
        float q = getQForFilter(3, order);
        filterThree.coefficients = Coefficients::makeLowPass(sampleRate, f, q);
    }
    if (filterFourEnabled())
    {
        if (isShelf)
        {
            filterFour.coefficients = Coefficients::makeHighShelf(
                sampleRate, f, resonance, pow(10.0f, gain / 20.0f)
            );
        }
        else
        {
            filterFour.coefficients
                = Coefficients::makeFirstOrderLowPass(sampleRate, f);
        }
    }
}

void LowPassFilter::delayedUpdateOrder()
{
    order = pendingOrder;
    pendingOrder = -1;
    updateFilters();
}

bool LowPassFilter::anythingSmoothing()
{
    return smoothFrequency.isSmoothing() || smoothGain.isSmoothing()
        || smoothResonance.isSmoothing();
}

float LowPassFilter::getQForFilter(int filter, int ord)
{
    if (filter == 1)
    {
        if (ord == 2)
            return 0.707f;
        else if (ord == 3)
            return 1;
        else if (ord == 4)
            return 0.541f;
        else if (ord == 5)
            return 0.618f;
        else if (ord == 6)
            return 0.518f;
        else if (ord == 7)
            return 0.555f;
    }
    else if (filter == 2)
    {
        if (ord == 4)
            return 1.307f;
        else if (ord == 5)
            return 1.618f;
        else if (ord == 6)
            return 0.707f;
        else if (ord == 7)
            return 0.802f;
    }
    else if (filter == 3)
    {
        if (ord == 6)
            return 1.932f;
        else if (ord == 7)
            return 2.247f;
    }
    // this shouldn't happen, but return a neutral q just in case
    return 0.707f;
}

void LowPassFilter::combineMagnitudes
(double* totals, const double* toCombine, size_t len)
{
    for (size_t i = 0;i < len;i++)
        totals[i] *= toCombine[i];
}