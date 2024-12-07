#pragma once
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "GainFilter.h"
#include "Linkable.h"

class GainControl : public FilterControl<GainFilter>
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
    void attachToFilter(juce::AudioProcessorValueTreeState*, GainFilter*)
        override;

private:
    bool toggleAbove;
};