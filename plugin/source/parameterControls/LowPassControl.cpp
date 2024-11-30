#include "LowPassControl.h"

// === Lifecycle ==============================================================
LowPassControl::LowPassControl()
{
    frequency.label.setPostfix(" Hz");
    frequency.label.setMaxDecimals(1);
    falloff.label.setPostfix(" dB/oct");
    falloff.label.setTypeNegativeValues(true);
    resonance.label.setPostfix(" res");
    resonance.label.setMaxDecimals(2);
    onOff.toggle.setText("ON", "OFF");
}

LowPassControl::~LowPassControl() { }

// === Settings ===============================================================
void LowPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    int toggleW = 37;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y + 6, toggleW, 21);
}

void LowPassControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    resonance.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
}

void LowPassControl::attachToLowPass
(juce::AudioProcessorValueTreeState* stateTree, LowPassFilter* filter)
{
    frequency.attachToParameter(stateTree, filter->getFrequencyParameter());
    falloff.attachToParameter(stateTree, filter->getFalloffParameter());
    resonance.attachToParameter(stateTree, filter->getResonanceParameter());
    onOff.onToggle = [this] (bool toggled)
    {
        frequency.slider.setEnabled(toggled);
        falloff.slider.setEnabled(toggled);
        resonance.slider.setEnabled(toggled);
    };
    onOff.attachToParameter(stateTree, filter->getOnOffParameter());
}

// === Linking ================================================================
void LowPassControl::link(const LowPassControl* other)
{
    frequency.link(&other->frequency);
    falloff.link(&other->falloff);
    resonance.link(&other->resonance);
    onOff.link(&other->onOff);
}

void LowPassControl::unlink(const LowPassControl* other)
{
    frequency.unlink(&other->frequency);
    falloff.unlink(&other->falloff);
    resonance.unlink(&other->resonance);
    onOff.unlink(&other->onOff);
}