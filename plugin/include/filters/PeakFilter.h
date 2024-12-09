#pragma once
#include <mutex>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"
#include "FilterStateListener.h"

namespace dsp = juce::dsp;

class PeakFilter : public CtmFilter
{
public:
    // === Lifecycle ==========================================================
    PeakFilter(std::string nameArg, std::string paramText, float defaultFreq);

    // === Parameter Information ==============================================
    void onChangedParameter(const juce::String&, float) override;
    inline std::string getOnOffParameter() override
        { return name + "-" + onOffParam.idPostfix; }
    inline std::string getFrequencyParameter()
        { return name + "-" + freqParamIdPostfix; }
    inline std::string getGainParameter()
        { return name + "-" + gainParam.idPostfix; }
    inline std::string getQFactorParameter()
        { return name + "-" + qParam.idPostfix; }
    void getParameters(std::vector<ParameterBlueprint>&) override;
    void getMagnitudes(const double*, double*, size_t) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setGain(float);
    void setQFactor(float);

    // === Linking ============================================================
    void setParamsOnLink(std::string) override;

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSampleProtected(float) override;

private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    const float defaultFreq;
    juce::SmoothedValue<float> smoothBypass;
    float gain;
    float q;
    double sampleRate;
    
    // === Parameter Settings =================================================
    inline static const ParameterBlueprint onOffParam {
        ParameterBlueprint("on", "On/Off")
            .withTwoStepDiscrete("ON", "OFF")
            .withDefault(1)
    };
    inline static const ParameterBlueprint gainParam {
        ParameterBlueprint("gain", "Gain")
            .withRange(-18, 18, 0.1f)
            .withDefault(0)
            .withShowPlus()
            .withMaxDecimals(1)
            .withUnits("dB")
    };
    inline static const ParameterBlueprint qParam {
        ParameterBlueprint("q", "Q-Factor")
            .withRange(0.25f, 10, 0.01f, 0.7f)
            .withDefault(0.71f)
            .withMaxDecimals(2)
    };
    inline static const std::string freqParamIdPostfix { "freq" };
    inline ParameterBlueprint getFreqParameterFields()
    {
        return ParameterBlueprint(freqParamIdPostfix, "Frequency")
            .withRange(20, 20000, 0.1f)
            .withDefault(defaultFreq)
            .withMaxDecimals(1)
            .withUnits("Hz");
    }

    // === Private Helper =====================================================
    void setFilterParameters(float freq, float gain, float q);
};