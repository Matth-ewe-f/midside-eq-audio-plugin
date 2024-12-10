#include "HighPassControl.h"

// === Lifecycle ==============================================================
HighPassControl::HighPassControl() : isShelf(false)
{
    cutFreq.label.setPostfix(" Hz");
    cutFreq.label.setMaxDecimals(1);
    shelfFreq.label.setPostfix(" Hz");
    shelfFreq.label.setMaxDecimals(1);
    falloff.label.setPostfix(" dB/oct");
    falloff.label.setTypeNegativeValues(true);
    shelfGain.label.setPostfix(" dB");
    shelfGain.label.setShowPlusForPositive(true);
    shelfGain.label.setMaxDecimals(1);
    cutRes.label.setPostfix(" res");
    cutRes.label.setMaxDecimals(2);
    shelfRes.label.setPostfix(" res");
    shelfRes.label.setMaxDecimals(2);
    onOff.toggle.setText("ON", "OFF");
    onOff.addOnToggleFunction([this] (bool toggled)
    {
        cutFreq.slider.setEnabled(toggled);
        shelfFreq.slider.setEnabled(toggled);
        falloff.slider.setEnabled(toggled);
        shelfGain.slider.setEnabled(toggled);
        cutRes.slider.setEnabled(toggled);
        shelfRes.slider.setEnabled(toggled);
        shelfToggle.toggle.setColorAsUntoggled(!toggled);
    });
    shelfToggle.toggle.setText("SHELF", "CUT");
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
    if (isShelf)
    {
        shelfFreq.setBounds(x, y, itemW, itemH);
        shelfGain.setBounds(x, y + itemH + yPad, itemW, itemH);
        shelfRes.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    }
    else
    {
        cutFreq.setBounds(x, y, itemW, itemH);
        falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
        cutRes.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    }
    int toggleW = 37;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y - 2, toggleW, 21);
    shelfToggle.setBounds(toggleX, y + 24, toggleW, 21);
}

void HighPassControl::setAllColorOverrides(juce::Colour color)
{
    cutFreq.slider.setColorOverride(color);
    shelfFreq.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    shelfGain.slider.setColorOverride(color);
    cutRes.slider.setColorOverride(color);
    shelfRes.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
    shelfToggle.toggle.setColorOverride(color);
}

void HighPassControl::attachToFilter
(juce::AudioProcessorValueTreeState* tree, HighPassFilter* filter)
{
    cutFreq.attachToParameter(tree, filter->getCutFrequencyParameter());
    shelfFreq.attachToParameter(tree, filter->getShelfFrequencyParameter());
    falloff.attachToParameter(tree, filter->getFalloffParameter());
    shelfGain.attachToParameter(tree, filter->getShelfGainParameter());
    cutRes.attachToParameter(tree, filter->getCutResonanceParameter());
    shelfRes.attachToParameter(tree, filter->getShelfResonanceParameter());
    shelfToggle.attachToParameter(tree, filter->getShelfModeParameter());
    onOff.attachToParameter(tree, filter->getOnOffParameter());
}

// === Private ================================================================
void HighPassControl::setIsShelf(bool b)
{
    if (isShelf && !b)
    {
        cutFreq.setBounds(shelfFreq.getBounds());
        shelfFreq.setBounds(0, 0, 0, 0);
        falloff.setBounds(shelfGain.getBounds());
        shelfGain.setBounds(0, 0, 0, 0);
        cutRes.setBounds(shelfRes.getBounds());
        shelfRes.setBounds(0, 0, 0, 0);
    }
    else if (!isShelf && b)
    {
        shelfFreq.setBounds(cutFreq.getBounds());
        cutFreq.setBounds(0, 0, 0, 0);
        shelfGain.setBounds(falloff.getBounds());
        falloff.setBounds(0, 0, 0, 0);
        shelfRes.setBounds(cutRes.getBounds());
        cutRes.setBounds(0, 0, 0, 0);
    }
    isShelf = b;
}