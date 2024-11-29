#pragma once
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "GainFilter.h"

class GainControl : public FilterControl
{
public:
    ParameterControl gain;
    ParameterToggle onOff;

    // === Lifecycle ==========================================================
    GainControl();
    ~GainControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void setToggleAboveSlider(bool);
    void attachToGain
    (juce::AudioProcessorValueTreeState*, GainFilter*);
private:
    bool toggleAbove;
};