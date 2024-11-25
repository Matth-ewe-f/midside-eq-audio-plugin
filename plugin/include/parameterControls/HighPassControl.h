#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "HighPassFilter.h"

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
    void setAllColorOverrides(juce::Colour) override;
    void attachToHighPass
    (juce::AudioProcessorValueTreeState*, HighPassFilter*);
};