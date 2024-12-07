#pragma once
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterStateListener.h"

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;

typedef struct ParameterFields
{
    std::string idPostfix;
    std::string displayName;
    juce::NormalisableRange<float> range;
    float defaultValue;
}
ParameterFields;

inline ParameterFields makeParamFields
(std::string postfix, std::string name, float min, float max, float step,
float skew, float defaultValue)
{
    ParameterFields fields;
    fields.idPostfix = postfix;
    fields.displayName = name;
    fields.range = juce::NormalisableRange<float>(min, max, step, skew);
    fields.defaultValue = defaultValue;
    return fields;
}

class CtmFilter : public juce::AudioProcessorValueTreeState::Listener
{
public:
    const std::string name;
    const std::string paramText;
    
    // === Lifecycle ==========================================================
    CtmFilter(std::string nameArg, std::string parameterText);

    // === ValueTreeState Listener ============================================
    void setListenTo(juce::AudioProcessorValueTreeState*);
    void stopListeningTo(juce::AudioProcessorValueTreeState*);
    void parameterChanged(const juce::String&, float) override;
    
    // === For EQ Displays ====================================================
    void addStateListener(FilterStateListener*);
    void removeStateListener(FilterStateListener*);
    virtual void getMagnitudes(const double*, double*, size_t) = 0;

    // === Parameters =========================================================
    void addParameters(ParameterLayout*);
    virtual std::string getOnOffParameter() = 0;
    virtual void getParameters(std::vector<ParameterFields>& container) = 0;

    // === Process Audio ======================================================
    void link(CtmFilter*);
    void unlink(CtmFilter*);
    virtual void setParamsOnLink(std::string) = 0;

    // === Process Audio ======================================================
    float processSample(float);

protected:
    std::atomic<long long> timeAtLastProcess;
    juce::AudioProcessorValueTreeState* stateTree;

    virtual float processSampleProtected(float) = 0;
    virtual void onChangedParameter(const juce::String&, float) = 0;
    bool isProcessing();

private:
    std::vector<FilterStateListener*> listeners;
    std::vector<CtmFilter*> linked;
    void nofityListeners();
};