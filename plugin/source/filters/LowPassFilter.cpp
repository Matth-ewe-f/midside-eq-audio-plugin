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
        return;
    double* perFilter = new double[len];
    if (filterOneEnabled())
    {
        filterOne.coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterTwoEnabled())
    {
        filterTwo.coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterThreeEnabled())
    {
        filterThree.coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterFourEnabled())
    {

        filterFour.coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (!isShelf)
    {
        // don't show filter mirroring above the Nyquist frequency
        // ensure that the magnitudes are non-increasing
        double lastMagnitude = magnitudes[0];
        for (size_t i = 1;i < len;i++)
        {
            if (magnitudes[i] > lastMagnitude)
                magnitudes[i] = lastMagnitude;
            lastMagnitude = magnitudes[i];
        }
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

void LowPassFilter::setIsShelf(bool shelf)
{
    isShelf = shelf;
    updateFilters();
}

void LowPassFilter::setShelfGain(float gain)
{
    smoothGain.setTargetValue(gain);
}

void LowPassFilter::setResonance(float res)
{
    smoothResonance.setTargetValue(res);
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

void LowPassFilter::combineMagnitudes
(double* totals, const double* toCombine, size_t len)
{
    for (size_t i = 0;i < len;i++)
        totals[i] *= toCombine[i];
}