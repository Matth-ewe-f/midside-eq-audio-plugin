#include "HighPassControl.h"

// === Lifecycle ==============================================================
HighPassControl::HighPassControl()
{
    frequency.label.setPostfix(" Hz");
    frequency.label.setMaxDecimals(1);
    falloff.label.setPostfix(" dB/oct");
    falloff.label.setTypeNegativeValues(true);
    resonance.label.setPostfix(" res");
    resonance.label.setMaxDecimals(2);
    onOff.toggle.setText("ON", "OFF");
}

HighPassControl::~HighPassControl() { }

// === Settings ===============================================================
void HighPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    int toggleW = 28;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y + 6, toggleW, 21);
}

void HighPassControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    resonance.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
}

void HighPassControl::attachToHighPass
(juce::AudioProcessorValueTreeState* tree, HighPassFilter* filter)
{
    frequency.attachToParameter(tree, filter->getFrequencyParameter());
    falloff.attachToParameter(tree, filter->getFalloffParameter());
    resonance.attachToParameter(tree, filter->getResonanceParameter());
    onOff.attachToParameter(tree, filter->getOnOffParameter());
}

// === Linking ================================================================
void HighPassControl::link(const HighPassControl* other)
{
    frequency.link(&other->frequency);
    falloff.link(&other->falloff);
    resonance.link(&other->resonance);
    onOff.link(&other->onOff);
}

void HighPassControl::unlink(const HighPassControl* other)
{
    frequency.unlink(&other->frequency);
    falloff.unlink(&other->falloff);
    resonance.unlink(&other->resonance);
    onOff.unlink(&other->onOff);
}