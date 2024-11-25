#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmSlider.h"
#include "SliderLabel.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class ParameterControl
{
public:
    CtmSlider slider;
    std::unique_ptr<SliderAttachment> attachment;
    SliderLabel label;

    ParameterControl();
    ~ParameterControl();

    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);
};