#include "CtmSlider.h"
#include "CtmLookAndFeel.h"

CtmSlider::CtmSlider() : useColorOverride(false) { }

void CtmSlider::paint(juce::Graphics& g)
{
    if (useColorOverride)
    {
        // temporarily set the look and feel color to the override color
        auto id = CtmColourIds::meterFillColourId;
        juce::Colour oldColour = getLookAndFeel().findColour(id);
        getLookAndFeel().setColour(id, colorOverride);
        // paint the component
        Slider::paint(g);
        // reset the look and feel colour
        getLookAndFeel().setColour(id, oldColour);
    }
    else
    {
        Slider::paint(g);
    }
}

void CtmSlider::setColorOverride(juce::Colour color)
{
    useColorOverride = true;
    colorOverride = color;
}