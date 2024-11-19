#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class SliderLabel :
    public juce::Label,
    public juce::Slider::Listener
{
public:
    SliderLabel();
    void setPrefix(std::string s);
    void setPostfix(std::string s);
    void updateText(juce::Slider*);
private:
    std::string prefix;
    std::string postfix;
    void sliderValueChanged(juce::Slider*) override;
};