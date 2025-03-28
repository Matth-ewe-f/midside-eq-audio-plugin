#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"

namespace dsp = juce::dsp;

class HighPassFilter : public CtmFilter
{
public:
    // === Lifecycle ==========================================================
    HighPassFilter
    (std::string name, std::string parameterText, std::string secondParamText);

    // === Parameter Information ==============================================
    void onChangedParameter(const juce::String&, float) override;
    inline std::string getOnOffParameter() override
        { return getIdForParameter(&onOffParam); }
    inline std::string getShelfModeParameter()
        { return getIdForParameter(&shelfModeParam); }
    inline std::string getCutFrequencyParameter()
        { return getIdForParameter(&cutFreqParam); }
    inline std::string getShelfFrequencyParameter()
        { return getIdForParameter(&shelfFreqParam); }
    inline std::string getFalloffParameter()
        { return getIdForParameter(&falloffParam); }
    inline std::string getShelfGainParameter()
        { return getIdForParameter(&shelfGainParam); }
    inline std::string getCutResonanceParameter()
        { return getIdForParameter(&cutResParam); }
    inline std::string getShelfResonanceParameter()
        { return getIdForParameter(&shelfResParam); }
    void getParameters(std::vector<ParameterBlueprint>&) override;
    void getMagnitudes(const double*, double*, size_t) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setCutFrequency(float);
    void setShelfFrequency(float);
    void setOrder(int);
    void setCutResonance(float);
    void setShelfResonance(float);
    void setIsShelf(bool);
    void setShelfGain(float);
    
    // === Linking ============================================================
    void setParamsOnLink(std::string) override;

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSampleProtected(float) override;

private:
    // === Private Variables ==================================================
    // filters 1~3 are always second order, filter 4 is always first order
    // different combinations can create a filter of any order from 1 to 7
    dsp::IIR::Filter<float> filterOne;
    dsp::IIR::Filter<float> filterTwo;
    dsp::IIR::Filter<float> filterThree;
    dsp::IIR::Filter<float> filterFour;
    juce::SmoothedValue<float> smoothCutFreq;
    juce::SmoothedValue<float> smoothShelfFreq;
    juce::SmoothedValue<float> smoothBypass;
    juce::SmoothedValue<float> smoothGain;
    juce::SmoothedValue<float> smoothCutRes;
    juce::SmoothedValue<float> smoothShelfRes;
    int order;
    int pendingOrder;
    int fadeSamples;
    bool isShelf;
    double sampleRate;
    
    // === Static Constants ===================================================
    inline static const ParameterBlueprint onOffParam {
        ParameterBlueprint("on", "On")
            .withTwoStepDiscrete("ON", "OFF")
            .withDefault(0)
    };
    inline static const ParameterBlueprint shelfModeParam {
        ParameterBlueprint("shelf-mode", "Enabled")
            .withUseSecondFilterName()
            .withTwoStepDiscrete("SHELF", "CUT")
            .withDefault(0)
    };
    inline static const ParameterBlueprint cutFreqParam {
        ParameterBlueprint("freq", "Frequency")
            .withRange(20, 20000, 0.1f, 0.35f)
            .withDefault(20)
            .withMaxDecimals(1)
            .withUnits("Hz")
    };
    inline static const ParameterBlueprint shelfFreqParam {
        ParameterBlueprint("shelf-freq", "Frequency")
            .withUseSecondFilterName()
            .withRange(20, 20000, 0.1f, 0.35f)
            .withDefault(20)
            .withMaxDecimals(1)
            .withUnits("Hz")
    };
    inline static const ParameterBlueprint falloffParam {
        ParameterBlueprint("falloff", "Falloff")
            .withRange(6, 36, 6)
            .withDiscrete()
            .withDefault(6)
            .withAlwaysNegative()
            .withUnits("dB/oct")
    };
    inline static const ParameterBlueprint shelfGainParam {
        ParameterBlueprint("shelf-gain", "Gain")
            .withUseSecondFilterName()
            .withRange(-18, 18, 0.1f)
            .withDefault(0)
            .withShowPlus()
            .withMaxDecimals(1)
            .withUnits("dB")
    };
    inline static const ParameterBlueprint cutResParam {
        ParameterBlueprint("shelf-res", "Resonance")
            .withRange(0.25f, 10, 0.01f, 0.7f)
            .withDefault(0.71f)
            .withMaxDecimals(2)
    };
    inline static const ParameterBlueprint shelfResParam {
        ParameterBlueprint("res", "Resonance")
            .withUseSecondFilterName()
            .withRange(0.25f, 10, 0.01f, 0.7f)
            .withDefault(0.71f)
            .withMaxDecimals(2)
    };
    inline static const int fadeLength { 200 };

    // === Private Helper =====================================================
    void updateFilters();
    void updateFilters
    (float cutFreq, float shelfFreq, float gain, float cutRes, float shelfRes);
    void delayedUpdateOrder();
    bool anythingSmoothing();
    float getQForFilter(int, int, float);
    void combineMagnitudes(double* totals, const double* toCombine, size_t);
    inline bool filterOneEnabled() { return order >= 2 && !isShelf; }
    inline bool filterTwoEnabled() { return order >= 4 && !isShelf; }
    inline bool filterThreeEnabled() { return order >= 6 && !isShelf; }
    inline bool filterFourEnabled() { return order % 2 == 1 || isShelf; }
    inline bool filterOneEnabled(int ord) { return ord >= 2 && !isShelf; }
    inline bool filterTwoEnabled(int ord) { return ord >= 4 && !isShelf; }
    inline bool filterThreeEnabled(int ord) { return ord >= 6 && !isShelf; }
    inline bool filterFourEnabled(int ord) { return ord % 2 == 1 || isShelf; }
};