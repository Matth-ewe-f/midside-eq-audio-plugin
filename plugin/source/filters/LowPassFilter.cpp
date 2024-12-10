#include "LowPassFilter.h"

using Coefficients = juce::dsp::IIR::Coefficients<float>;

// === Lifecycle ==============================================================
LowPassFilter::LowPassFilter
(std::string nameArg, std::string parameterText, std::string secondParamText)
    : CtmFilter(nameArg, parameterText, secondParamText), order(1),
    pendingOrder(-1), fadeSamples(-1), isShelf(false), sampleRate(48000)
{
    smoothCutFreq.setCurrentAndTargetValue(20000);
    smoothShelfFreq.setCurrentAndTargetValue(20000);
    smoothBypass.setCurrentAndTargetValue(0);
    smoothGain.setCurrentAndTargetValue(0);
    smoothCutRes.setCurrentAndTargetValue(0.71f);
    smoothShelfRes.setCurrentAndTargetValue(0.71f);
    updateFilters();
}

// === Parameter Information ==================================================
void LowPassFilter::onChangedParameter(const juce::String& param, float value)
{
    if (param.compare(onOffParam.idPostfix) == 0)
        setBypass(value <= 0);
    else if (param.compare(shelfModeParam.idPostfix) == 0)
        setIsShelf(value >= 1);
    else if (param.compare(cutFreqParam.idPostfix) == 0)
        setCutFrequency(value);
    else if (param.compare(shelfFreqParam.idPostfix) == 0)
        setShelfFrequency(value);
    else if (param.compare(falloffParam.idPostfix) == 0)
        setOrder((int) value / 6);
    else if (param.compare(shelfGainParam.idPostfix) == 0)
        setShelfGain(value);
    else if (param.compare(cutResParam.idPostfix) == 0)
        setCutResonance(value);
    else if (param.compare(shelfResParam.idPostfix) == 0)
        setShelfResonance(value);
}

void LowPassFilter::getParameters(std::vector<ParameterBlueprint>& parameters)
{
    parameters.push_back(onOffParam);
    parameters.push_back(cutFreqParam);
    parameters.push_back(shelfFreqParam);
    parameters.push_back(falloffParam);
    parameters.push_back(cutResParam);
    parameters.push_back(shelfResParam);
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
    float freq;
    float res;
    if (isShelf)
    {
        freq = smoothShelfFreq.getTargetValue();
        res = smoothShelfRes.getTargetValue();
    }
    else
    {
        freq = smoothCutFreq.getTargetValue();
        res = smoothCutRes.getTargetValue();
    }
    int curOrder = pendingOrder == -1 ? order : pendingOrder;
    if (filterOneEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(1, curOrder, res)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterTwoEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(2, curOrder, res)
        );
        coefficients->getMagnitudeForFrequencyArray(
            frequencies, perFilter, len, sampleRate * 2
        );
        combineMagnitudes(magnitudes, perFilter, len);
    }
    if (filterThreeEnabled())
    {
        auto coefficients = Coefficients::makeLowPass(
            sampleRate * 2, freq, getQForFilter(3, curOrder, res)
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
    smoothCutFreq.reset(samplesPerBlock);
    smoothShelfFreq.reset(samplesPerBlock);
    smoothBypass.reset(samplesPerBlock);
    smoothGain.reset(samplesPerBlock);
    smoothCutRes.reset(samplesPerBlock);
    smoothShelfRes.reset(samplesPerBlock);
    sampleRate = newSampleRate;
}

void LowPassFilter::setBypass(bool isBypassed)
{
    if (isProcessing())
        smoothBypass.setTargetValue(isBypassed ? 0 : 1);
    else
        smoothBypass.setCurrentAndTargetValue(isBypassed ? 0 : 1);
}

void LowPassFilter::setCutFrequency(float newFrequency)
{
    if (!isShelf && isProcessing())
        smoothCutFreq.setTargetValue(newFrequency);
    else
    {
        smoothCutFreq.setCurrentAndTargetValue(newFrequency);
        updateFilters();
    }
}

void LowPassFilter::setShelfFrequency(float newFrequency)
{
    if (isShelf && isProcessing())
        smoothShelfFreq.setTargetValue(newFrequency);
    else
    {
        smoothShelfFreq.setCurrentAndTargetValue(newFrequency);
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

void LowPassFilter::setCutResonance(float res)
{
    if (!isShelf && isProcessing())
        smoothCutRes.setTargetValue(res);
    else
    {
        smoothCutRes.setCurrentAndTargetValue(res);
        updateFilters();
    }
}

void LowPassFilter::setShelfResonance(float res)
{
    if (isShelf && isProcessing())
        smoothShelfRes.setTargetValue(res);
    else
    {
        smoothShelfRes.setCurrentAndTargetValue(res);
        updateFilters();
    }
}

// === Linking ================================================================
void LowPassFilter::setParamsOnLink(std::string paramName)
{
    std::atomic<float>* param;
    std::string paramId;
    paramId = onOffParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setBypass(*param <= 0);
    paramId = cutFreqParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setCutFrequency(*param);
    paramId = shelfFreqParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setShelfFrequency(*param);
    paramId = falloffParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setOrder((int)(*param) / 6);
    paramId = cutResParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setCutResonance(*param);
    paramId = shelfResParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setShelfResonance(*param);
    paramId = shelfModeParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setIsShelf(*param >= 1);
    paramId = shelfGainParam.getIdWithFilterName(paramName);
    if ((param = stateTree->getRawParameterValue(paramId)) != nullptr)
        setShelfGain(*param);
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
        float cutFreq = smoothCutFreq.getNextValue();
        float shelfFreq = smoothShelfFreq.getNextValue();
        float gain = smoothGain.getNextValue();
        float cutRes = smoothCutRes.getNextValue();
        float shelfRes = smoothShelfRes.getNextValue();
        updateFilters(cutFreq, shelfFreq, gain, cutRes, shelfRes);
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
    float cutFreq = smoothCutFreq.getCurrentValue();
    float shelfFreq = smoothShelfFreq.getCurrentValue();
    float gain = smoothGain.getCurrentValue();
    float cutRes = smoothCutRes.getCurrentValue();
    float shelfRes = smoothShelfRes.getCurrentValue();
    updateFilters(cutFreq, shelfFreq, gain, cutRes, shelfRes);
}

void LowPassFilter::updateFilters
(float cutFreq, float shelfFreq, float gain, float cutRes, float shelfRes)
{
    if (filterOneEnabled())
    {
        float q = getQForFilter(1, order, cutRes);
        filterOne.coefficients
            = Coefficients::makeLowPass(sampleRate, cutFreq, q);
    }
    if (filterTwoEnabled())
    {
        float q = getQForFilter(2, order, cutRes);
        filterTwo.coefficients
            = Coefficients::makeLowPass(sampleRate, cutFreq, q);
    }
    if (filterThreeEnabled())
    {
        float q = getQForFilter(3, order, cutRes);
        filterThree.coefficients
            = Coefficients::makeLowPass(sampleRate, cutFreq, q);
    }
    if (filterFourEnabled())
    {
        if (isShelf)
        {
            filterFour.coefficients = Coefficients::makeHighShelf(
                sampleRate, shelfFreq, shelfRes, pow(10.0f, gain / 20.0f)
            );
        }
        else
        {
            filterFour.coefficients
                = Coefficients::makeFirstOrderLowPass(sampleRate, cutFreq);
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
    return smoothCutFreq.isSmoothing() || smoothShelfFreq.isSmoothing()
        || smoothGain.isSmoothing() || smoothCutRes.isSmoothing()
        || smoothShelfRes.isSmoothing();
}

float LowPassFilter::getQForFilter(int filter, int ord, float res)
{
    // result should be overwriten, but default to a neutral q just in case
    float result = 0.707f;
    if (filter == 1)
    {
        if (ord == 2)
            result =  0.707f;
        else if (ord == 3)
            result =  1;
        else if (ord == 4)
            result =  0.541f;
        else if (ord == 5)
            result =  0.618f;
        else if (ord == 6)
            result =  0.518f;
        else if (ord == 7)
            result =  0.555f;
    }
    else if (filter == 2)
    {
        if (ord == 4)
            result =  1.307f;
        else if (ord == 5)
            result =  1.618f;
        else if (ord == 6)
            result =  0.707f;
        else if (ord == 7)
            result =  0.802f;
    }
    else if (filter == 3)
    {
        if (ord == 6)
            result =  1.932f;
        else if (ord == 7)
            result =  2.247f;
    }
    int highestBiquad = ord / 2;
    if (filter == highestBiquad)
    {
        result *= res / 0.707f;
    }
    return result;
}

void LowPassFilter::combineMagnitudes
(double* totals, const double* toCombine, size_t len)
{
    for (size_t i = 0;i < len;i++)
        totals[i] *= toCombine[i];
}