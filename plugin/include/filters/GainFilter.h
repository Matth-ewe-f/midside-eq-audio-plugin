#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"

class GainFilter : public CtmFilter
{
public:
    // === Lifecycle ==========================================================
    GainFilter(std::string nameArg, std::string displayName);

    // === Parameters =========================================================
    void onChangedParameter(const juce::String&, float) override;
    inline std::string getOnOffParameter() override
        { return name + "-" + onOffParam.idPostfix; }
    inline std::string getGainParameter()
        { return name + "-" + gainParam.idPostfix; }
    void getParameters(std::vector<ParameterBlueprint>&) override;
    void getMagnitudes(const double*, double*, size_t) override;
    void setGain(float);
    void setBypass(bool);

    // === Linking ============================================================
    void setParamsOnLink(std::string) override;

    // === Process Audio ======================================================
    void reset(int blockSize);
    float processSampleProtected(float) override;

private:
    juce::SmoothedValue<float> smoothGain;
    juce::SmoothedValue<float> smoothBypass;

    // === Static Constants ===================================================
    inline static const ParameterBlueprint onOffParam {
        ParameterBlueprint("on", "On/Off")
            .withTwoStepDiscrete("ON", "OFF")
            .withDefault(1)
    };
    inline static const ParameterBlueprint gainParam {
        ParameterBlueprint("gain", "")
            .withRange(-12, 12, 0.1f)
            .withDefault(0)
            .withShowPlus()
            .withMaxDecimals(1)
            .withUnits("dB")
    };
};