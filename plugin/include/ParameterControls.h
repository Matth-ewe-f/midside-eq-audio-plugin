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

// Abstract base class for filter controls
class FilterControl
{
public:
    virtual void setBounds(int x, int y, int w, int h, int xPad, int yPad) = 0;
};

// Group of parameters for controlling a low pass filter
class LowPassControl : public FilterControl
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl resonance;

    LowPassControl();
    ~LowPassControl();

    void addToEditor(PluginEditor*);
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToLowPass
    (PluginProcessor*, std::string freq, std::string falloff, std::string res);
};

// Group of parameters for controlling a parametric EQ filter
class ParametricEqControl : public FilterControl
{
public:
    ParameterControl frequency;
    ParameterControl gain;
    ParameterControl qFactor;

    ParametricEqControl();
    ~ParametricEqControl();

    void addToEditor(PluginEditor*);
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToParametricEq
    (PluginProcessor*, std::string freq, std::string gain, std::string res);
};

// Group of parameters for controlling a high pass filter
class HighPassControl : public FilterControl
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl resonance;

    HighPassControl();
    ~HighPassControl();

    void addToEditor(PluginEditor*);
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToHighPass
    (PluginProcessor*, std::string freq, std::string falloff, std::string res);
};