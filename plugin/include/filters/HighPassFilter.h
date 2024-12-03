#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"

namespace dsp = juce::dsp;

class HighPassFilter : public CtmFilter
{
public:
    // === Lifecycle ==========================================================
    HighPassFilter(std::string name, std::string parameterText);

    // === Parameter Information ==============================================
    void onChangedParameter(const juce::String&, float) override;
    inline std::string getOnOffParameter() override
        { return name + "-" + onOffParam.idPostfix; }
    inline std::string getShelfModeParameter()
        { return name + "-" + shelfModeParam.idPostfix; }
    inline std::string getFrequencyParameter()
        { return name + "-" + freqParam.idPostfix; }
    inline std::string getFalloffParameter()
        { return name + "-" + falloffParam.idPostfix; }
    inline std::string getShelfGainParameter()
        { return name + "-" + shelfGainParam.idPostfix; }
    inline std::string getResonanceParameter()
        { return name + "-" + resParam.idPostfix; }
    void getParameters(std::vector<ParameterFields>&) override;
    void getMagnitudes(const double*, double*, size_t) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setOrder(int);
    void setResonance(float);
    void setIsShelf(bool);
    void setShelfGain(float);

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
    juce::SmoothedValue<float> smoothFrequency;
    juce::SmoothedValue<float> smoothBypass;
    juce::SmoothedValue<float> smoothGain;
    juce::SmoothedValue<float> smoothResonance;
    int order;
    int pendingOrder;
    int fadeSamples;
    bool isShelf;
    double sampleRate;
    
    // === Static Constants ===================================================
    inline static const ParameterFields onOffParam {
        makeParamFields("on", "On/Off", 0, 1, 1, 1, 0)
    };
    inline static const ParameterFields shelfModeParam {
        makeParamFields("shelf-mode", "Shelf Mode", 0, 1, 1, 1, 0)
    };
    inline static const ParameterFields freqParam {
        makeParamFields("freq", "Frequency", 20, 20000, 0.1f, 0.35f, 20)
    };
    inline static const ParameterFields falloffParam {
        makeParamFields("falloff", "Falloff", 6, 36, 6, 1, 6)
    };
    inline static const ParameterFields shelfGainParam {
        makeParamFields("shelf-gain", "(Shelf Mode) Gain", -18, 18, 0.1f, 1, 0)
    };
    inline static const ParameterFields resParam {
        makeParamFields("res", "Resonance", 0.25f, 10, 0.01f, 0.7f, 0.71f)
    };
    inline static const int fadeLength { 200 };

    // === Private Helper =====================================================
    void updateFilters();
    void updateFilters(float frequency, float gain, float resonance);
    void delayedUpdateOrder();
    bool anythingSmoothing();
    float getQForFilter(int);
    void combineMagnitudes(double* totals, const double* toCombine, size_t);
    inline bool filterOneEnabled() { return order >= 2 && !isShelf; }
    inline bool filterTwoEnabled() { return order >= 4 && !isShelf; }
    inline bool filterThreeEnabled() { return order >= 6 && !isShelf; }
    inline bool filterFourEnabled() { return order % 2 == 1 || isShelf; }
};