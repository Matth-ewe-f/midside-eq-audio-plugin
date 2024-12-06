#pragma once
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "GainFilter.h"
#include "Linkable.h"

class GainControl : public FilterControl, public Linkable<GainControl>
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

    // === Linking ============================================================
    void link(const GainControl*) override;
    void unlink(const GainControl*) override;

private:
    bool toggleAbove;
};