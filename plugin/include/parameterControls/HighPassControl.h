#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "HighPassFilter.h"
#include "Linkable.h"

class HighPassControl : public FilterControl, public Linkable<HighPassControl>
{
public:
    ParameterControl frequency;
    ParameterControl falloff;
    ParameterControl shelfGain;
    ParameterControl resonance;
    ParameterToggle onOff;
    ParameterToggle shelfToggle;

    // === Lifecycle ==========================================================
    HighPassControl();
    ~HighPassControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void attachToHighPass
    (juce::AudioProcessorValueTreeState*, HighPassFilter*);

    // === Linking ============================================================
    void link(const HighPassControl*) override;
    void unlink(const HighPassControl*) override;
    
private:
    bool isShelf;

    void setIsShelf(bool);
};