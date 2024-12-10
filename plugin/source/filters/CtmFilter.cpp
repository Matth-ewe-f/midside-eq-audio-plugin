#include "CtmFilter.h"
#include <chrono>
#include <format>

using Parameter = juce::AudioProcessorValueTreeState::Parameter;

// === Lifecycle ==============================================================
CtmFilter::CtmFilter
(std::string nameArg, std::string parameterText, std::string secondText)
    : name(nameArg), paramText(parameterText), secondaryParamText(secondText),
    timeAtLastProcess(0), stateTree(nullptr)
{ }

// === ValueTreeState Listener ================================================
void CtmFilter::setListenTo(juce::AudioProcessorValueTreeState* tree)
{
    std::vector<ParameterBlueprint> parameters;
    getParameters(parameters);
    for (ParameterBlueprint param : parameters)
    {
        tree->addParameterListener(getIdForParameter(&param), this);
    }
    stateTree = tree;
}

void CtmFilter::stopListeningTo(juce::AudioProcessorValueTreeState* tree)
{
    std::vector<ParameterBlueprint> parameters;
    getParameters(parameters);
    for (ParameterBlueprint param : parameters)
    {
        tree->removeParameterListener(getIdForParameter(&param), this);
    }
    stateTree = nullptr;
}

void CtmFilter::parameterChanged(const juce::String& s, float value)
{
    juce::String param = s.substring((int)name.length() + 1);
    onChangedParameter(param, value);
    for (CtmFilter* otherFilter : linked)
    {
        otherFilter->onChangedParameter(param, value);
        otherFilter->nofityListeners();
    }
    nofityListeners();
}

// === For EQ Displays ========================================================
void CtmFilter::addStateListener(FilterStateListener* listener)
{
    listeners.push_back(listener);
}

void CtmFilter::removeStateListener(FilterStateListener* listener)
{
    auto pos = std::find(listeners.begin(), listeners.end(), listener);
    if (pos != listeners.end())
        listeners.erase(pos);
}

// === Parameters =============================================================
void CtmFilter::addParameters(ParameterLayout* parameters)
{
    std::vector<ParameterBlueprint> blueprints;
    getParameters(blueprints);
    for (ParameterBlueprint param : blueprints)
    {
        std::string paramTextToUse;
        if (param.useSecondFilterName && secondaryParamText.compare("") != 0)
            paramTextToUse = secondaryParamText;
        else
            paramTextToUse = paramText;
        parameters->add(param.create(name, paramTextToUse));
    }
}

// === Linking ================================================================
void CtmFilter::link(CtmFilter* other)
{
    linked.push_back(other);
    other->setParamsOnLink(name);
    other->nofityListeners();
}

void CtmFilter::unlink(CtmFilter* other)
{
    linked.erase(
        std::remove(linked.begin(), linked.end(), other), linked.end()
    );
    other->setParamsOnLink(other->name);
    other->nofityListeners();
}

// === Process Audio ==========================================================
float CtmFilter::processSample(float sample)
{
    auto sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        sinceEpoch
    ).count();
    timeAtLastProcess.store(ms);
    return processSampleProtected(sample);
}

// === Protected & Private ====================================================
void CtmFilter::nofityListeners()
{
    for (FilterStateListener* listener : listeners)
    {
        listener->notify(this);
    }
}

bool CtmFilter::isProcessing()
{
    auto sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        sinceEpoch
    ).count();
    return timeAtLastProcess.load() >= ms - 2;
}

std::string CtmFilter::getIdForParameter(const ParameterBlueprint* param)
{
    return param->getIdWithFilterName(name);
}