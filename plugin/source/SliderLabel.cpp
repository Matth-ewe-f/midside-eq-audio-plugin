#include "SliderLabel.h"
#include <format>

SliderLabel::SliderLabel() : prefix(""), postfix("")
{
    setEditable(true);
    setJustificationType(juce::Justification::centred);
}

void SliderLabel::setPrefix(std::string s)
{
    prefix = s;
}

void SliderLabel::setPostfix(std::string s)
{
    postfix = s;
}

void SliderLabel::updateText(juce::Slider* slider)
{
    std::string value = std::format("{:.0f}", slider->getValue());
    std::string text = prefix + value + postfix;
    setText(text, juce::dontSendNotification);
}

void SliderLabel::sliderValueChanged(juce::Slider* slider)
{
    updateText(slider);
}