#include "ParameterControl.h"

// === Lifecycle ==============================================================
ParameterControl::ParameterControl()
    : parameterName(""), tree(nullptr), everAttached(false)
{
    bounds = juce::Rectangle<int>(0, 0, 0, 0);
    setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    label.listenTo(&slider);
    juce::FontOptions mainFont(12);
    label.setMainFont(mainFont);
    juce::FontOptions postfixFont(9);
    label.setPostfixFont(postfixFont);
    label.updateText(&slider);
}

ParameterControl::~ParameterControl()
{
    if (tree != nullptr)
        tree->removeParameterListener(parameterName, this);
}

// === Settings ===============================================================
void ParameterControl::setBounds(juce::Rectangle<int> b)
{
    setBounds(b.getX(), b.getY(), b.getWidth(), b.getHeight());
}

void ParameterControl::setBounds(int x, int y, int width, int height)
{
    bounds = juce::Rectangle<int>(x, y, width, height);
    slider.setBounds(x, y, width, height - 13);
    label.setBounds(x, y + height - 12, width, 12);
}

void ParameterControl::attachToParameter
(juce::AudioProcessorValueTreeState* stateTree, std::string param)
{
    SliderAttachment* old = attachment.release();
    delete old;
    tree = stateTree;
    parameterName = param;
    attachment.reset(new SliderAttachment(*stateTree, param, slider));
    if (!everAttached)
        label.updateText(&slider);
    everAttached = true;
}

void ParameterControl::setSliderStyle(juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
}

// === Parameter Linking ======================================================
void ParameterControl::link(const ParameterControl* other)
{
    // setup modification of other parameter on changes to this parameter
    if (linkedParameters.size() == 0)
        tree->addParameterListener(parameterName, this);
    linkedParameters.push_back(other->parameterName);
    // copy current value of this parameter to other parameter
    float value = *tree->getRawParameterValue(parameterName);
    copyValueToParameter(other->parameterName, value);
}

void ParameterControl::unlink(const ParameterControl* other)
{
    auto begin = linkedParameters.begin();
    auto end = linkedParameters.end();
    auto removal = std::find(begin, end, other->parameterName);
    if (removal != end)
    {
        linkedParameters.erase(removal);
        if (linkedParameters.size() == 0)
            tree->removeParameterListener(parameterName, this);
    }
}

void ParameterControl::linkParameterBidirectional(ParameterControl* other)
{
    link(other);
    other->link(this);
}

void ParameterControl::unlinkParameterBidirectional(ParameterControl* other)
{
    unlink(other);
    other->unlink(this);
}

void ParameterControl::parameterChanged(const juce::String& param, float value)
{
    juce::ignoreUnused(param);
    for (std::string otherName : linkedParameters)
    {
        copyValueToParameter(otherName, value);
    }
}

// === Private Functions ======================================================
void ParameterControl::copyValueToParameter
(const std::string other, float value)
{
    juce::RangedAudioParameter* param = tree->getParameter(other);
    juce::NormalisableRange<float> range = param->getNormalisableRange();
    if (abs(param->getValue() - value) >= range.interval / 2)
    {
        float v = range.convertTo0to1(value);
        param->setValueNotifyingHost(v);
    }
}