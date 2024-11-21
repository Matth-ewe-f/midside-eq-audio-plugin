#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class CtmSlider : public juce::Slider
{
public:
    CtmSlider();

    void paint(juce::Graphics&) override;
    void setColorOverride(juce::Colour);
private:
    juce::Colour colorOverride;
    bool useColorOverride;
};