#include "ParameterListener.h"

ParameterListener::ParameterListener(juce::String param) :
    parameter(param),
    onChange([](const juce::String& s, float f) {
        juce::ignoreUnused(s, f);
    })
{ }

ParameterListener::ParameterListener
(juce::String param, std::function<void(float)> function) :
    parameter(param),
    onChange([function](const juce::String& s, float f) {
        juce::ignoreUnused(s);
        function(f);
    })
{ }

ParameterListener::ParameterListener
(juce::String s, std::function<void(const juce::String&, float)> function) :
    parameter(s),
    onChange(function)
{ }

void ParameterListener::parameterChanged
(const juce::String& param, float value)
{
    onChange(param, value);
}