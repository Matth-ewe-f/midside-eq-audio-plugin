#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class FilterControl
{
public:
    virtual void setBounds(int x, int y, int w, int h, int xPad, int yPad) = 0;
    virtual void setAllColorOverrides(juce::Colour) = 0;
};