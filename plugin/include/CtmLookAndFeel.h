#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class CtmLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CtmLookAndFeel();

    void drawRotarySlider
    (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float startAngle, const float endAngle, juce::Slider&) override;

private:
    const float rotaryOutlineWidth = 3;
    const juce::Colour bgColor = juce::Colour::fromRGB(38, 48, 52);
    const juce::Colour darkOutlineColor = juce::Colour::fromRGB(26, 34, 37);
    const juce::Colour brightOutlineColor 
        = juce::Colour::fromRGB(200, 200, 200);
    const juce::Colour rotaryColor = juce::Colour::fromRGB(40, 80, 100);
    const juce::Colour meterFillColor = juce::Colour::fromRGB(57, 125, 227);
};