#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
using Colour = juce::Colour;

class CtmLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // === Lifecycle ==========================================================
    CtmLookAndFeel();

    // === Component Drawer Overrides =========================================
    void drawRotarySlider
    (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float startAngle, const float endAngle, juce::Slider&) override;

private:
    // === Internal Layout Constants ==========================================
    const float rotaryOutlineWidth = 4;
    const Colour darkBgColor = Colour::fromRGB(36, 46, 52);
    const Colour normalBgColor = Colour::fromRGB(44, 54, 60);
    const Colour brightBgColor = juce::Colour::fromRGB(54, 72, 82);
    const Colour darkOutlineColor = Colour::fromRGB(22, 28, 30);
    const Colour brightOutlineColor = Colour::fromRGB(200, 200, 200);
    const Colour rotaryColor = Colour::fromRGB(40, 80, 100);
    const Colour meterFillColor = Colour::fromRGB(57, 125, 227);
};

enum CtmColourIds
{
    normalBgColourId,
    darkBgColourId,
    brightBgColourId,
    darkOutlineColourId,
    brightOutlineColourId,
    meterFillColourId,
};