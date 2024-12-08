#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "FilterStateListener.h"
#include "CtmFilter.h"
#include "PeakFilter.h"

class EqVisual : public juce::Component, public FilterStateListener
{
public:
    /// === Lifecycle =========================================================
    EqVisual();
    ~EqVisual() override;

    // === Graphics Functions =================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // === Filter State Listener ==============================================
    void addToFirstResponse(CtmFilter*);
    void addToSecondResponse(CtmFilter*);
    void notify(CtmFilter*) override;

    // === Parameters =========================================================
    void setFrequencyResponseColors(juce::Colour, juce::Colour);

private:
    std::vector<CtmFilter*> filtersForResponseOne;
    std::vector<CtmFilter*> filtersForResponseTwo;
    juce::Colour freqResponseColorOne;
    juce::Colour freqResponseColorTwo;
    juce::Image bgImage;
    bool bgImageCached;
    std::vector<double> displayFreqs;
    size_t numDisplayFreqs;
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
    void drawBackground(juce::Graphics&);
    void drawHorzLine(juce::Graphics&, int);
    void drawCentralHorzLine(juce::Graphics&, int);
    void drawVertLine(juce::Graphics&, int);
    void drawMainVertLine(juce::Graphics&, int);
    void drawGainLabel(juce::Graphics&, int, int);
    void drawFreqLabel(juce::Graphics&, int, int);
    void drawFreqResponse
    (juce::Graphics&, std::vector<CtmFilter*>&, juce::Colour);

    // === Other Helper Functions =============================================
    float getYForGain(float);
    float getXForFrequency(float);
    float getFrequencyForX(int);
    bool shouldDrawFreqLabel(int);
    juce::Colour getColorForGainLabels();
    juce::Colour getColorForFreqLabels();
};