#include "LowPassControl.h"

// === Lifecycle ==============================================================
LowPassControl::LowPassControl()
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
    shelfToggle.toggle.setText("SHELF", "FILTER");
    shelfToggle.toggle.setDisplayAlwaysUp(true);
}

LowPassControl::~LowPassControl() { }

// === Settings ===============================================================
void LowPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
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

void LowPassControl::setAllColorOverrides(juce::Colour color)
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

void LowPassControl::attachToFilter
(juce::AudioProcessorValueTreeState* stateTree, LowPassFilter* filter)
{
    frequency.attachToParameter(stateTree, filter->getFrequencyParameter());
    falloff.attachToParameter(stateTree, filter->getFalloffParameter());
    shelfGain.attachToParameter(stateTree, filter->getShelfGainParameter());
    resonance.attachToParameter(stateTree, filter->getResonanceParameter());
    shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        setIsShelf(toggled);
    });
    shelfToggle.attachToParameter(stateTree, filter->getShelfModeParameter());
    onOff.addOnToggleFunction([this] (bool toggled)
    {
        frequency.slider.setEnabled(toggled);
        falloff.slider.setEnabled(toggled);
        shelfGain.slider.setEnabled(toggled);
        resonance.slider.setEnabled(toggled);
        shelfToggle.toggle.setColorAsUntoggled(!toggled);
    });
    onOff.attachToParameter(stateTree, filter->getOnOffParameter());
}

// === Private ================================================================
void LowPassControl::setIsShelf(bool b)
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