#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "LowPassFilter.h"
#include "Linkable.h"

class LowPassControl : public FilterControl<LowPassFilter>
{
public:
    ParameterControl cutFreq;
    ParameterControl shelfFreq;
    ParameterControl falloff;
    ParameterControl shelfGain;
    ParameterControl cutRes;
    ParameterControl shelfRes;
    ParameterToggle onOff;
    ParameterToggle shelfToggle;

    // === Lifecycle ==========================================================
    LowPassControl();
    ~LowPassControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void attachToFilter(juce::AudioProcessorValueTreeState*, LowPassFilter*)
        override;

private:
    bool isShelf;

    void setIsShelf(bool);
};