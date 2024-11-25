#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

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
    // === Lifecycle ==========================================================
    CtmFilter(std::string nameArg, std::string parameterText);

    // === Listener ===========================================================
    void setListenTo(juce::AudioProcessorValueTreeState*);
    void stopListeningTo(juce::AudioProcessorValueTreeState*);

    // === Parameters =========================================================
    void addParameters(ParameterLayout*);

protected:
    const std::string name;
    const std::string paramText;

    virtual void getParameters(std::vector<ParameterFields>& container) = 0;
};