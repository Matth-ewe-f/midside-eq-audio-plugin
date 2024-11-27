#include "PeakFilterControl.h"

// === Lifecycle ==============================================================
PeakFilterControl::PeakFilterControl()
{
    frequency.label.setPostfix(" Hz");
    frequency.label.setMaxDecimals(1);
    gain.label.setPostfix(" dB");
    gain.label.setMaxDecimals(1);
    qFactor.label.setPostfix(" Q");
    qFactor.label.setMaxDecimals(2);
    onOff.toggle.setText("ON", "OFF");
}

PeakFilterControl::~PeakFilterControl() { }

// === Settings ===============================================================
void PeakFilterControl::setBounds
(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    gain.setBounds(x, y + itemH + yPad, itemW, itemH);
    qFactor.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    int toggleW = 28;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y + 6, toggleW, 21);
}

void PeakFilterControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    gain.slider.setColorOverride(color);
    qFactor.slider.setColorOverride(color);
    color = color.withMultipliedSaturation(0.8f);
    color = color.withMultipliedBrightness(0.8f);
    onOff.toggle.setColorOverride(color);
}

void PeakFilterControl::attachToPeakFilter
(juce::AudioProcessorValueTreeState* stateTree, PeakFilter* filter)
{
    frequency.attachToParameter(stateTree, filter->getFrequencyParameter());
    gain.attachToParameter(stateTree, filter->getGainParameter());
    qFactor.attachToParameter(stateTree, filter->getQFactorParameter());
    onOff.onToggle = [this] (bool toggled)
    {
        frequency.slider.setEnabled(toggled);
        gain.slider.setEnabled(toggled);
        qFactor.slider.setEnabled(toggled);
    };
    onOff.attachToParameter(stateTree, filter->getOnOffParameter());
}

// === Linking ================================================================
void PeakFilterControl::link(const PeakFilterControl* other)
{
    frequency.link(&other->frequency);
    gain.link(&other->gain);
    qFactor.link(&other->qFactor);
    onOff.link(&other->onOff);
}

void PeakFilterControl::unlink(const PeakFilterControl* other)
{
    frequency.unlink(&other->frequency);
    gain.unlink(&other->gain);
    qFactor.unlink(&other->qFactor);
    onOff.unlink(&other->onOff);
}