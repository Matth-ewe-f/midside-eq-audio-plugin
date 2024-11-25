#include "ParameterControl.h"

ParameterControl::ParameterControl()
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    label.listenTo(&slider);
    juce::FontOptions mainFont(12);
    label.setMainFont(mainFont);
    juce::FontOptions postfixFont(9);
    label.setPostfixFont(postfixFont);
    label.updateText(&slider);
}

ParameterControl::~ParameterControl() { }

void ParameterControl::setBounds(int x, int y, int width, int height)
{
    slider.setBounds(x, y, width, height - 13);
    label.setBounds(x, y + height - 12, width, 12);
}

void ParameterControl::attachToParameter
(juce::AudioProcessorValueTreeState* stateTree, std::string parameter)
{
    attachment.reset(new SliderAttachment(*stateTree, parameter, slider));
    label.updateText(&slider);
}