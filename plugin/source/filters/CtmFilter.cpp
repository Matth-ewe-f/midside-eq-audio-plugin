#include "CtmFilter.h"
#include <format>

using Parameter = juce::AudioProcessorValueTreeState::Parameter;

// === Lifecycle ==============================================================
CtmFilter::CtmFilter(std::string nameArg, std::string parameterText)
    : name(nameArg), paramText(parameterText)
{ }

// === Listener ===============================================================
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