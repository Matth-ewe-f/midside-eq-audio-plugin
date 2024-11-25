#include "ParameterToggle.h"

ParameterToggle::ParameterToggle() { }

ParameterToggle::~ParameterToggle() { }

void ParameterToggle::setBounds(int x, int y, int width, int height)
{
    toggle.setBounds(x, y, width, height);
}

void ParameterToggle::attachToParameter
(juce::AudioProcessorValueTreeState* tree, std::string parameter)
{
    attachment.reset(new ButtonAttachment(*tree, parameter, toggle));
}