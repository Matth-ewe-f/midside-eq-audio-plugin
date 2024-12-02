#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "CtmFilter.h"

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
using Parameter = juce::AudioProcessorValueTreeState::Parameter;
namespace dsp = juce::dsp;

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
    void getMagnitudes(const double*, double*, size_t) override;
    void setGain(float);
    void setBypass(bool);

    // === Process Audio ======================================================
    void reset(int blockSize);
    float processSample(float);

protected:
    void getParameters(std::vector<ParameterFields>&) override;

private:
    juce::SmoothedValue<float> smoothGain;
    juce::SmoothedValue<float> smoothBypass;

    // === Static Constants ===================================================
    inline static const ParameterFields onOffParam {
        makeParamFields("on", "On/Off", 0, 1, 1, 1, 1)
    };
    inline static const ParameterFields gainParam {
        makeParamFields("gain", "", -12, 12, 0.1f, 1, 0)
    };
};