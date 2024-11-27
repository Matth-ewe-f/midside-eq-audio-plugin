#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"

namespace dsp = juce::dsp;

class LowPassFilter : public CtmFilter
{
public:
    // === Lifecycle ==========================================================
    LowPassFilter(std::string name, std::string parameterText);

    // === Parameter Information ==============================================
    void parameterChanged(const juce::String&, float) override;
    inline std::string getOnOffParameter() override
        { return name + "-" + onOffParam.idPostfix; }
    inline std::string getFrequencyParameter()
        { return name + "-" + freqParam.idPostfix; }
    inline std::string getFalloffParameter()
        { return name + "-" + falloffParam.idPostfix; }
    inline std::string getResonanceParameter()
        { return name + "-" + resParam.idPostfix; }

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setOrder(int);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);

protected:
    void getParameters(std::vector<ParameterFields>& container) override;

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
    int order;
    int pendingOrder;
    int fadeSamples;
    double sampleRate;
    
    // === Static Constants ===================================================
    inline static const ParameterFields onOffParam {
        makeParamFields("on", "On/Off", 0, 1, 1, 1, 1)
    };
    inline static const ParameterFields freqParam {
        makeParamFields("freq", "Frequency", 20, 20000, 0.1f, 0.35f, 20000)
    };
    inline static const ParameterFields falloffParam {
        makeParamFields("falloff", "Falloff", 6, 36, 6, 1, 6)
    };
    inline static const ParameterFields resParam {
        makeParamFields("res", "Resonance", 0.5, 10, 0.01f, 0.7f, 0.71f)
    };
    inline static const int fadeLength { 1000 };

    // === Private Helper =====================================================
    void updateFilters(float frequency);
    void delayedUpdateOrder();
    float getQForFilter(int);
    inline bool filterOneEnabled() { return order >= 2; }
    inline bool filterTwoEnabled() { return order >= 4; }
    inline bool filterThreeEnabled() { return order >= 6; }
    inline bool filterFourEnabled() { return order % 2 == 1; }
};