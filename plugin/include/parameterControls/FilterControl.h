#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmFilter.h"

template<typename T>
concept filter = std::derived_from<T, CtmFilter>;

template <filter T>
class FilterControl
{
public:
    virtual void setBounds(int x, int y, int w, int h, int xPad, int yPad) = 0;
    virtual void setAllColorOverrides(juce::Colour) = 0;
    virtual void attachToFilter(juce::AudioProcessorValueTreeState*, T*) = 0;
};