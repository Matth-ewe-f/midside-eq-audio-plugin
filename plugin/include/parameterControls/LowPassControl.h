#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"

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
    void setAllColorOverrides(juce::Colour) override;
    void attachToLowPass
    (juce::AudioProcessorValueTreeState*, std::string freqParam,
    std::string falloffParam, std::string resParam, std::string onOffParam);
};