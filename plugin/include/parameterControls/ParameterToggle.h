#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmToggle.h"

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class ParameterToggle : juce::AudioProcessorValueTreeState::Listener
{
public:
    std::string parameterName;
    CtmToggle toggle;
    std::unique_ptr<ButtonAttachment> attachment;

    // === Lifecycle ==========================================================
    ParameterToggle();

    // === Settings ===========================================================
    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);

    // === Linking ============================================================
    void linkToggle(ParameterToggle*);
    void unlinkToggle(ParameterToggle*);
    void linkToggleBidirectional(ParameterToggle*);
    void unlinkToggleBidirectional(ParameterToggle*);
    void parameterChanged(const juce::String&, float) override;

private:
    juce::AudioProcessorValueTreeState* tree;
    std::vector<std::string> linkedToggles;

    void copyValueToParameter(const std::string, float);
};