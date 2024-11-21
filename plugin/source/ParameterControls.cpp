#include "ParameterControls.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"

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

void ParameterControl::addToEditor(PluginEditor* editor)
{
    editor->addAndMakeVisible(slider);
    editor->addAndMakeVisible(label);
}

void ParameterControl::setBounds(int x, int y, int width, int height)
{
    slider.setBounds(x, y, width, height - 13);
    label.setBounds(x, y + height - 12, width, 12);
}

void ParameterControl::attachToParameter
(PluginProcessor* processor, std::string parameter)
{
    attachment.reset(
        new SliderAttachment(processor->tree, parameter, slider)
    );
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
}

LowPassControl::~LowPassControl() { }

void LowPassControl::addToEditor(PluginEditor* editor)
{
    frequency.addToEditor(editor);
    falloff.addToEditor(editor);
    resonance.addToEditor(editor);
}

void LowPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
}

void LowPassControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    resonance.slider.setColorOverride(color);
}

void LowPassControl::attachToLowPass
(PluginProcessor* processor, std::string freqParam, std::string falloffParam,
std::string resParam)
{
    frequency.attachToParameter(processor, freqParam);
    falloff.attachToParameter(processor, falloffParam);
    resonance.attachToParameter(processor, resParam);
}

// === ParametricEqControl ====================================================
ParametricEqControl::ParametricEqControl()
{
    frequency.label.setPostfix(" Hz");
    frequency.label.setMaxDecimals(1);
    gain.label.setPostfix(" dB");
    gain.label.setMaxDecimals(1);
    qFactor.label.setPostfix(" Q");
    qFactor.label.setMaxDecimals(2);
}

ParametricEqControl::~ParametricEqControl() { }

void ParametricEqControl::addToEditor(PluginEditor* editor)
{
    frequency.addToEditor(editor);
    gain.addToEditor(editor);
    qFactor.addToEditor(editor);
}

void ParametricEqControl::setBounds
(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    gain.setBounds(x, y + itemH + yPad, itemW, itemH);
    qFactor.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
}

void ParametricEqControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    gain.slider.setColorOverride(color);
    qFactor.slider.setColorOverride(color);
}

void ParametricEqControl::attachToParametricEq
(PluginProcessor* processor, std::string freqParam, std::string gainParam,
std::string qParam)
{
    frequency.attachToParameter(processor, freqParam);
    gain.attachToParameter(processor, gainParam);
    qFactor.attachToParameter(processor, qParam);
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
}

HighPassControl::~HighPassControl() { }

void HighPassControl::addToEditor(PluginEditor* editor)
{
    frequency.addToEditor(editor);
    falloff.addToEditor(editor);
    resonance.addToEditor(editor);
}

void HighPassControl::setBounds(int x, int y, int w, int h, int xPad, int yPad)
{
    int itemW = (w - xPad) / 2;
    int itemH = (h - yPad) / 2;
    frequency.setBounds(x, y, itemW, itemH);
    falloff.setBounds(x, y + itemH + yPad, itemW, itemH);
    resonance.setBounds(x + itemW + xPad, y + itemH + yPad, itemW, itemH);
}

void HighPassControl::setAllColorOverrides(juce::Colour color)
{
    frequency.slider.setColorOverride(color);
    falloff.slider.setColorOverride(color);
    resonance.slider.setColorOverride(color);
}

void HighPassControl::attachToHighPass
(PluginProcessor* processor, std::string freqParam, std::string falloffParam,
std::string resParam)
{
    frequency.attachToParameter(processor, freqParam);
    falloff.attachToParameter(processor, falloffParam);
    resonance.attachToParameter(processor, resParam);
}