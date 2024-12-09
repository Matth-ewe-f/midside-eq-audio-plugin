#include "HighPassControl.h"

// === Lifecycle ==============================================================
HighPassControl::HighPassControl() : isShelf(false)
{
    frequency.label.setPostfix(" Hz");
    frequency.label.setMaxDecimals(1);
    falloff.label.setPostfix(" dB/oct");
    falloff.label.setTypeNegativeValues(true);
    shelfGain.label.setPostfix(" dB");
    shelfGain.label.setShowPlusForPositive(true);
    shelfGain.label.setMaxDecimals(1);
    resonance.label.setPostfix(" res");
    resonance.label.setMaxDecimals(2);
    onOff.toggle.setText("ON", "OFF");
    onOff.addOnToggleFunction([this] (bool toggled)
    {
        frequency.slider.setEnabled(toggled);
        falloff.slider.setEnabled(toggled);
        shelfGain.slider.setEnabled(toggled);
        resonance.slider.setEnabled(toggled);
        shelfToggle.toggle.setColorAsUntoggled(!toggled);
    });
    shelfToggle.toggle.setText("SHELF", "FILTER");
    shelfToggle.toggle.setDisplayAlwaysUp(true);
    shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        setIsShelf(toggled);
    });
}

HighPassControl::~HighPassControl() { }

// === Settings ===============================================================
void HighPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    if (isShelf)
        shelfGain.setBounds(x, y + itemH + yPad, itemW, itemH);
    else
        falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    int toggleW = 37;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y - 2, toggleW, 21);
    shelfToggle.setBounds(toggleX, y + 24, toggleW, 21);
}

void HighPassControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    shelfGain.slider.setColorOverride(color);
    resonance.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
    shelfToggle.toggle.setColorOverride(color);
}

void HighPassControl::attachToFilter
(juce::AudioProcessorValueTreeState* tree, HighPassFilter* filter)
{
    frequency.attachToParameter(tree, filter->getFrequencyParameter());
    falloff.attachToParameter(tree, filter->getFalloffParameter());
    shelfGain.attachToParameter(tree, filter->getShelfGainParameter());
    resonance.attachToParameter(tree, filter->getResonanceParameter());
    shelfToggle.attachToParameter(tree, filter->getShelfModeParameter());
    onOff.attachToParameter(tree, filter->getOnOffParameter());
}

// === Private ================================================================
void HighPassControl::setIsShelf(bool b)
{
    if (isShelf && !b)
    {
        falloff.setBounds(shelfGain.getBounds());
        shelfGain.setBounds(0, 0, 0, 0);
    }
    else if (!isShelf && b)
    {
        shelfGain.setBounds(falloff.getBounds());
        falloff.setBounds(0, 0, 0, 0);
    }
    isShelf = b;
}