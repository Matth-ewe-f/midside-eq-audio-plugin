#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FilterStateListener.h"
#include "CtmFilter.h"
#include "PeakFilter.h"

class EqVisual : public juce::Component, public FilterStateListener
{
public:
    // === Graphics Functions =================================================
    void paint(juce::Graphics&) override;

    // === Filter State Listener ==============================================
    void listenTo(CtmFilter*);
    void notify(CtmFilter*) override;

    // === Parameters =========================================================
    void setFrequencyResponseColor(juce::Colour);

private:
    std::vector<CtmFilter*> filters;
    juce::Colour freqResponseColor;
    // === Color Constants ====================================================
    inline static const juce::Colour bgColor
        { juce::Colour::fromRGB(8, 20, 32) };

    // === Layout Constants ===================================================
    inline static const int xStart { 108 };
    inline static const int xEnd { 80 };
    inline static const int paddingY { 28 };
    inline static const int numHorzLines { 9 };
    inline static const int horzLineExtension { 32 };
    inline static const int vertLineExtension { 16 };
    inline static const int majorHorzLineExtraExtension { 8 };
    inline static const int majorVertLineExtraExtension { 4 };
    inline static const int freqResponseExtension { 24 };

    // === Drawing Helper Functions ===========================================
    void drawHorzLine(juce::Graphics&, int);
    void drawCentralHorzLine(juce::Graphics&, int);
    void drawVertLine(juce::Graphics&, int);
    void drawMainVertLine(juce::Graphics&, int);
    void drawGainLabel(juce::Graphics&, int, int);
    void drawFreqLabel(juce::Graphics&, int, int);
    void drawFreqResponse(juce::Graphics&);

    // === Other Helper Functions =============================================
    float getYForGain(float);
    float getXForFrequency(float);
    float getFrequencyForX(int);
    bool shouldDrawFreqLabel(int);
    juce::Colour getColorForGainLabels();
    juce::Colour getColorForFreqLabels();
};