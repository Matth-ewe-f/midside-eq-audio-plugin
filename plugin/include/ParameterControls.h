#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmToggle.h"
#include "CtmSlider.h"
#include "SliderLabel.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

// Class for controlling a toggleable parameter (i.e. an on/off switch)
class ParameterToggle
{
public:
    CtmToggle toggle;
    std::unique_ptr<ButtonAttachment> attachment;

    ParameterToggle();
    ~ParameterToggle();

    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);
};

// General class for controlling a single parameter
class ParameterControl
{
public:
    CtmSlider slider;
    std::unique_ptr<SliderAttachment> attachment;
    SliderLabel label;

    ParameterControl();
    ~ParameterControl();

    void setBounds(int x, int y, int width, int height);
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);
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
    ParameterToggle onOff;

    LowPassControl();
    ~LowPassControl();

    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToLowPass
    (juce::AudioProcessorValueTreeState*, std::string freqParam,
    std::string falloffParam, std::string resParam, std::string onOffParam);
};

// Group of parameters for controlling a parametric EQ filter
class PeakFilterControl : public FilterControl
{
public:
    ParameterControl frequency;
    ParameterControl gain;
    ParameterControl qFactor;
    ParameterToggle onOff;

    PeakFilterControl();
    ~PeakFilterControl();

    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToPeakFilter
    (juce::AudioProcessorValueTreeState*, std::string freqParam,
    std::string gainParam, std::string resParam, std::string onOffParam);
};

// Group of parameters for controlling a high pass filter
class HighPassControl : public FilterControl
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl resonance;
    ParameterToggle onOff;

    HighPassControl();
    ~HighPassControl();

    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour);
    void attachToHighPass
    (juce::AudioProcessorValueTreeState*, std::string freqParam,
    std::string falloffParam, std::string resParam, std::string onOffParam);
};