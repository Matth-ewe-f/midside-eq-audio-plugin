#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"

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
    void setAllColorOverrides(juce::Colour) override;
    void attachToPeakFilter
    (juce::AudioProcessorValueTreeState*, std::string freqParam,
    std::string gainParam, std::string resParam, std::string onOffParam);
};