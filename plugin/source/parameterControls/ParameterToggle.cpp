#include "ParameterToggle.h"

// === Lifecycle ==============================================================
ParameterToggle::ParameterToggle() : parameterName(""), tree(nullptr) { }

// === Settings ===============================================================
void ParameterToggle::setBounds(int x, int y, int width, int height)
{
    toggle.setBounds(x, y, width, height);
}

void ParameterToggle::attachToParameter
(juce::AudioProcessorValueTreeState* stateTree, std::string parameter)
{
    tree = stateTree;
    parameterName = parameter;
    tree->addParameterListener(parameterName, this);
    attachment.reset(new ButtonAttachment(*stateTree, parameter, toggle));
    for (std::function<void(bool)> func : onToggle)
    {
        func(*stateTree->getRawParameterValue(parameter) >= 1);
    }
}

void ParameterToggle::addOnToggleFunction(std::function<void(bool)> func)
{
    onToggle.push_back(func);
}

// === ValueTreeState Listener ================================================
void ParameterToggle::parameterChanged(const juce::String& param, float value)
{
    juce::ignoreUnused(param);
    toggle.setToggleState(value >= 1, juce::sendNotification);
    for (std::function<void(bool)> func : onToggle)
    {
        func(value >= 1);
    }
    for (std::string otherName : linkedToggles)
    {
        copyValueToParameter(otherName, value);
    }
}

// === Linking ================================================================
void ParameterToggle::link(const ParameterToggle* other)
{
    // setup mirroring of parameter values
    linkedToggles.push_back(other->parameterName);
    // copy current parameter value
    float value = *tree->getRawParameterValue(parameterName);
    copyValueToParameter(other->parameterName, value);
}

void ParameterToggle::unlink(const ParameterToggle* other)
{
    auto begin = linkedToggles.begin();
    auto end = linkedToggles.end();
    auto removal = std::find(begin, end, other->parameterName);
    if (removal != end)
    {
        linkedToggles.erase(removal);
    }
}

void ParameterToggle::linkToggleBidirectional(ParameterToggle* other)
{
    link(other);
    other->link(this);
}

void ParameterToggle::unlinkToggleBidirectional(ParameterToggle* other)
{
    unlink(other);
    other->unlink(this);
}

// === Private Functions ======================================================
void ParameterToggle::copyValueToParameter
(const std::string other, float value)
{
    juce::RangedAudioParameter* param = tree->getParameter(other);
    if (abs(param->getValue() - value) >= 0.1f)
    {
        float v = param->getNormalisableRange().convertTo0to1(value);
        param->setValueNotifyingHost(v);
    }
}