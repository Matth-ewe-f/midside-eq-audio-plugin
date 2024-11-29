#include "GainControl.h"

// === Lifecycle ==============================================================
GainControl::GainControl() : toggleAbove(true)
{
    gain.setSliderStyle(juce::Slider::LinearVertical);
    gain.label.setPostfix(" dB");
    gain.label.setMaxDecimals(1);
    gain.label.setShowPlusForPositive(true);
    onOff.toggle.setText("GAIN");
}

GainControl::~GainControl() { }

// === Settings ===============================================================
void GainControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    juce::ignoreUnused(xPad, yPad);
    int gainY;
    int toggleY;
    if (toggleAbove)
    {
        gainY = y + 28;
        toggleY = y;
    }
    else
    {
        gainY = y;
        toggleY = y + h - 20;
    }
    gain.setBounds(x, gainY, w, h - 28);
    onOff.setBounds(x + (w / 2) - 16, toggleY, 32, 20);
}

void GainControl::setAllColorOverrides(juce::Colour color)
{
    gain.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
}

void GainControl::setToggleAboveSlider(bool isAbove)
{
    toggleAbove = isAbove;
    gain.slider.repaint();
    gain.label.repaint();
    onOff.toggle.repaint();
}

void GainControl::attachToGain
(juce::AudioProcessorValueTreeState* tree, GainFilter* filter)
{
    gain.attachToParameter(tree, filter->getGainParameter());
    onOff.onToggle = [this] (bool toggled)
    {
        gain.slider.setEnabled(toggled);
    };
    onOff.attachToParameter(tree, filter->getOnOffParameter());
}