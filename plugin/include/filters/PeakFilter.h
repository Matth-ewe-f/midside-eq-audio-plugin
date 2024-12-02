#pragma once
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
    void getParameters(std::vector<ParameterFields>&) override;
    void getMagnitudes(const double*, double*, size_t) override;

    // === Set Parameters =====================================================
    void reset(double newSampleRate, int samplesPerBlock);
    void setBypass(bool);
    void setFrequency(float);
    void setGain(float);
    void setQFactor(float);

    // === Process Audio ======================================================
    void prepare(const dsp::ProcessSpec&);
    float processSample(float);

private:
    dsp::IIR::Filter<float> filter;
    juce::SmoothedValue<float> smoothFrequency;
    const float defaultFreq;
    juce::SmoothedValue<float> smoothBypass;
    float gain;
    float q;
    double sampleRate;
    
    // === Parameter Settings =================================================
    inline static const ParameterFields onOffParam {
        makeParamFields("on", "On/Off", 0, 1, 1, 1, 1)
    };
    inline static const ParameterFields gainParam {
        makeParamFields("gain", "Gain", -18, 18, 0.1f, 1, 0)
    };
    inline static const ParameterFields qParam {
        makeParamFields("q", "Q Factor", 0.25f, 10, 0.01f, 0.7f, 0.71f)
    };
    inline static const std::string freqParamIdPostfix { "freq" };
    inline ParameterFields getFreqParameterFields()
    {
        return makeParamFields(
            freqParamIdPostfix, "Frequency", 20, 20000, 0.1f, 0.35f, defaultFreq
        );
    }

    // === Private Helper =====================================================
    void setFilterParameters(float freq, float gain, float q);
};