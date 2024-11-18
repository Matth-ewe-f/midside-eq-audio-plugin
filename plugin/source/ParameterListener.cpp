#include "ParameterListener.h"
#include <functional>

ParameterListener::ParameterListener(juce::String param) :
    parameter(param),
    onChange([](const juce::String& s, float f) {})
{ }

ParameterListener::ParameterListener
(juce::String s, std::function<void(const juce::String&, float)> function) :
    parameter(s),
    onChange(function)
{ }

void ParameterListener::parameterChanged
(const juce::String& parameter, float value)
{
    onChange(parameter, value);
}