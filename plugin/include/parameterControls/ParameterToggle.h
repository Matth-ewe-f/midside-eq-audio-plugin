#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmToggle.h"
#include "ParameterToggle.h"
#include "Linkable.h"

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class ParameterToggle :
    juce::AudioProcessorValueTreeState::Listener,
    Linkable<ParameterToggle>
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
    void link(const ParameterToggle*) override;
    void unlink(const ParameterToggle*) override;
    void linkToggleBidirectional(ParameterToggle*);
    void unlinkToggleBidirectional(ParameterToggle*);
    void parameterChanged(const juce::String&, float) override;

private:
    juce::AudioProcessorValueTreeState* tree;
    std::vector<std::string> linkedToggles;

    void copyValueToParameter(const std::string, float);
};