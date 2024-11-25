#include "LowPassControl.h"

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

void LowPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
    int toggleW = 28;
    int toggleX = x + itemW + xPad + ((itemW - toggleW) / 2);
    onOff.setBounds(toggleX, y + 6, toggleW, 20);
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
(juce::AudioProcessorValueTreeState* stateTree, std::string freqParam,
std::string falloffParam, std::string resParam, std::string onOffParam)
{
    frequency.attachToParameter(stateTree, freqParam);
    falloff.attachToParameter(stateTree, falloffParam);
    resonance.attachToParameter(stateTree, resParam);
    onOff.attachToParameter(stateTree, onOffParam);
}