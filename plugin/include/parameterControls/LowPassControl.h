#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "LowPassFilter.h"
#include "Linkable.h"

class LowPassControl : public FilterControl, public Linkable<LowPassControl>
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl shelfGain;
    ParameterControl resonance;
    ParameterToggle onOff;
    ParameterToggle shelfToggle;

    // === Lifecycle ==========================================================
    LowPassControl();
    ~LowPassControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void attachToLowPass(juce::AudioProcessorValueTreeState*, LowPassFilter*);

    // === Linking ============================================================
    void link(const LowPassControl*) override;
    void unlink(const LowPassControl*) override;

private:
    bool isShelf;

    void setIsShelf(bool);
};