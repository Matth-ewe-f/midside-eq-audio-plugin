#pragma once
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmToggle.h"
#include "ParameterToggle.h"
#include "Linkable.h"

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class ParameterToggle :
    public juce::AudioProcessorValueTreeState::Listener,
    public Linkable<ParameterToggle>
{
public:
    std::string parameterName;
    CtmToggle toggle;
    std::unique_ptr<ButtonAttachment> attachment;
    std::function<void(bool)> onToggle;

    // === Lifecycle ==========================================================
    ParameterToggle();

    // === Settings ===========================================================
    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);

    // === ValueTreeState Listener ============================================
    void parameterChanged(const juce::String&, float) override;

    // === Linking ============================================================
    void link(const ParameterToggle*) override;
    void unlink(const ParameterToggle*) override;
    void linkToggleBidirectional(ParameterToggle*);
    void unlinkToggleBidirectional(ParameterToggle*);

private:
    juce::AudioProcessorValueTreeState* tree;
    std::vector<std::string> linkedToggles;

    void copyValueToParameter(const std::string, float);
};