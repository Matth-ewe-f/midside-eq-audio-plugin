#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmSlider.h"
#include "SliderLabel.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class ParameterControl : juce::AudioProcessorValueTreeState::Listener
{
public:
    std::string parameterName;
    CtmSlider slider;
    std::unique_ptr<SliderAttachment> attachment;
    SliderLabel label;

    // === Lifecycle ==========================================================
    ParameterControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);

    // === Parameter Linking ==================================================
    void linkParameter(ParameterControl*);
    void linkParameterBidirectional(ParameterControl*);
    void unlinkParameter(ParameterControl*);
    void unlinkParameterBidirectional(ParameterControl*);
    void parameterChanged(const juce::String&, float) override;
private:
    juce::AudioProcessorValueTreeState* tree;
    std::vector<std::string> linkedParameters;

    void copyValueToParameter(const std::string, float);
};