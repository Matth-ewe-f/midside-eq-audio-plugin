#pragma once
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>

class ParameterListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    juce::String parameter;
    ParameterListener(juce::String);
    ParameterListener(juce::String, std::function<void(float)> function);
    ParameterListener
    (juce::String, std::function<void(const juce::String&, float)> function);
    void parameterChanged(const juce::String&, float) override;
private:
    std::function<void(const juce::String&, float)> onChange;
};