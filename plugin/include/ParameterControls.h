#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmSlider.h"
#include "SliderLabel.h"
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment; 

// forward declarations
class PluginEditor;
class PluginProcessor;

// General class for controlling a single parameter
class ParameterControl
{
public:
    CtmSlider slider;
    std::unique_ptr<SliderAttachment> attachment;
    SliderLabel label;

    ParameterControl();
    ~ParameterControl();

    void addToEditor(PluginEditor*);
    void setBounds(int x, int y, int width, int height);
    void attachToParameter(PluginProcessor* processor, std::string parameter);
};

// Group of parameters for controlling a low pass filter
class LowPassControl
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl resonance;

    LowPassControl();
    ~LowPassControl();

    void addToEditor(PluginEditor*);
    void setBounds(int x, int y, int width, int height, int xPad, int yPad);
    void attachToLowPass
    (PluginProcessor* processor, std::string freq, std::string falloff,
    std::string res);
};