#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "PeakFilter.h"
#include "Linkable.h"

class PeakFilterControl :
    public FilterControl,
    public Linkable<PeakFilterControl>
{
public:
    ParameterControl frequency;
    ParameterControl gain;
    ParameterControl qFactor;
    ParameterToggle onOff;

    // === Lifecycle ==========================================================
    PeakFilterControl();
    ~PeakFilterControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void attachToPeakFilter(juce::AudioProcessorValueTreeState*, PeakFilter*);

    // === Linking ============================================================
    void link(const PeakFilterControl*) override;
    void unlink(const PeakFilterControl*) override;
};