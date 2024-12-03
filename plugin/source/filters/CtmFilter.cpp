#include "CtmFilter.h"
#include <chrono>
#include <format>

using Parameter = juce::AudioProcessorValueTreeState::Parameter;

// === Lifecycle ==============================================================
CtmFilter::CtmFilter(std::string nameArg, std::string parameterText)
    : name(nameArg), paramText(parameterText), timeAtLastProcess(0)
{ }

// === ValueTreeState Listener ================================================
void CtmFilter::setListenTo(juce::AudioProcessorValueTreeState* tree)
{
    std::vector<ParameterFields> parameters;
    getParameters(parameters);
    for (ParameterFields fields : parameters)
    {
        tree->addParameterListener(name + "-" + fields.idPostfix, this);
    }
}

void CtmFilter::stopListeningTo(juce::AudioProcessorValueTreeState* tree)
{
    std::vector<ParameterFields> parameters;
    getParameters(parameters);
    for (ParameterFields fields : parameters)
    {
        tree->removeParameterListener(name + "-" + fields.idPostfix, this);
    }
}

void CtmFilter::parameterChanged(const juce::String& s, float value)
{
    onChangedParameter(s, value);
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
    std::vector<ParameterFields> parameterFields;
    getParameters(parameterFields);
    for (ParameterFields fields : parameterFields)
    {
        parameters->add(std::make_unique<Parameter>(
            name + "-" + fields.idPostfix,
            std::vformat(paramText, std::make_format_args(fields.displayName)),
            fields.range,
            fields.defaultValue
        ));
    }
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