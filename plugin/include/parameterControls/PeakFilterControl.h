#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FilterControl.h"
#include "ParameterControl.h"
#include "ParameterToggle.h"
#include "PeakFilter.h"
#include "Linkable.h"

class PeakFilterControl : public FilterControl<PeakFilter>
{
public:
    ParameterControl frequency;
    ParameterControl gain;
    ParameterControl qFactor;
    ParameterToggle onOff;
    PeakFilter* attachedTo;
    juce::AudioProcessorValueTreeState* tree;

    // === Lifecycle ==========================================================
    PeakFilterControl();
    ~PeakFilterControl();

    // === Settings ===========================================================
    void setBounds(int x, int y, int w, int h, int xPad, int yPad) override;
    void setAllColorOverrides(juce::Colour) override;
    void attachToFilter(juce::AudioProcessorValueTreeState*, PeakFilter*)
        override;

private:
    PeakFilter* unlinkedAttachedTo;
    juce::AudioProcessorValueTreeState* unlinkedTree;
};