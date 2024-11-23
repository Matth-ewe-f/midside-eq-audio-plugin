#include "ParameterControls.h"

// === ParameterToggle ========================================================

ParameterToggle::ParameterToggle() { }

ParameterToggle::~ParameterToggle() { }

void ParameterToggle::setBounds(int x, int y, int width, int height)
{
    toggle.setBounds(x, y, width, height);
}

void ParameterToggle::attachToParameter
(juce::AudioProcessorValueTreeState* tree, std::string parameter)
{
    attachment.reset(new ButtonAttachment(*tree, parameter, toggle));
}

// === ParameterControl =======================================================
ParameterControl::ParameterControl()
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    label.listenTo(&slider);
    juce::FontOptions mainFont(12);
    label.setMainFont(mainFont);
    juce::FontOptions postfixFont(9);
    label.setPostfixFont(postfixFont);
    label.updateText(&slider);
}

ParameterControl::~ParameterControl() { }

void ParameterControl::setBounds(int x, int y, int width, int height)
{
    slider.setBounds(x, y, width, height - 13);
    label.setBounds(x, y + height - 12, width, 12);
}

void ParameterControl::attachToParameter
(juce::AudioProcessorValueTreeState* stateTree, std::string parameter)
{
    attachment.reset(new SliderAttachment(*stateTree, parameter, slider));
    label.updateText(&slider);
}

// === LowPassControl =========================================================
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

// === ParametricEqControl ====================================================
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
    onOff.setBounds(toggleX, y + 6, toggleW, 20);
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
(juce::AudioProcessorValueTreeState* stateTree, std::string freqParam,
std::string gainParam, std::string qParam, std::string onOffParam)
{
    frequency.attachToParameter(stateTree, freqParam);
    gain.attachToParameter(stateTree, gainParam);
    qFactor.attachToParameter(stateTree, qParam);
    onOff.attachToParameter(stateTree, onOffParam);
}

// === HighPassControl ========================================================
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

void HighPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
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
(juce::AudioProcessorValueTreeState* stateTree, std::string freqParam,
std::string falloffParam, std::string resParam, std::string onOffParam)
{
    frequency.attachToParameter(stateTree, freqParam);
    falloff.attachToParameter(stateTree, falloffParam);
    resonance.attachToParameter(stateTree, resParam);
    onOff.attachToParameter(stateTree, onOffParam);
}