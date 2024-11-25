#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmToggle.h"

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class ParameterToggle
{
public:
    CtmToggle toggle;
    std::unique_ptr<ButtonAttachment> attachment;

    ParameterToggle();
    ~ParameterToggle();

    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);
};