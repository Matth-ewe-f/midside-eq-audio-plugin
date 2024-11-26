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
    attachment.reset(new ButtonAttachment(*stateTree, parameter, toggle));
}

// === Linking ================================================================
void ParameterToggle::linkToggle(ParameterToggle* other)
{
    // setup mirroring of parameter values
    if (linkedToggles.size() == 0)
        tree->addParameterListener(parameterName, this);
    linkedToggles.push_back(other->parameterName);
    // copy current parameter value
    float value = *tree->getRawParameterValue(parameterName);
    copyValueToParameter(other->parameterName, value);
}

void ParameterToggle::unlinkToggle(ParameterToggle* other)
{
    auto begin = linkedToggles.begin();
    auto end = linkedToggles.end();
    auto removal = std::find(begin, end, other->parameterName);
    if (removal != end)
    {
        linkedToggles.erase(removal);
        if (linkedToggles.size() == 0)
            tree->removeParameterListener(parameterName, this);
    }
}

void ParameterToggle::linkToggleBidirectional(ParameterToggle* other)
{
    linkToggle(other);
    other->linkToggle(this);
}

void ParameterToggle::unlinkToggleBidirectional(ParameterToggle* other)
{
    unlinkToggle(other);
    other->unlinkToggle(this);
}

void ParameterToggle::parameterChanged(const juce::String& param, float value)
{
    juce::ignoreUnused(param);
    for (std::string otherName : linkedToggles)
    {
        copyValueToParameter(otherName, value);
    }
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