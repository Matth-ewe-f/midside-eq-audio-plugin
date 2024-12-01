#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class EqVisual : public juce::Component
{
public:
    // === Graphics Functions =================================================
    void paint(juce::Graphics&) override;

private:
    // === Color Constants ====================================================
    inline static const juce::Colour bgColor
        { juce::Colour::fromRGB(8, 20, 32) };

    // === Layout Constants ===================================================
    inline static const int xStart { 72 };
    inline static const int xEnd { 48 };
    inline static const int paddingY { 10 };
    inline static const int numHorzLines { 9 };
    inline static const int horzLineGradWidth { 48 };
    inline static const int vertLineGradHeight { 18 };
    inline static const int minorLinesExtraPaddingX { 8 };
    inline static const int minorLinesExtraPaddingY { 4 };
    static juce::NormalisableRange<float> freqRange;

    // === Drawing Helper Functions ===========================================
    void drawHorzLine(juce::Graphics&, int);
    void drawCentralHorzLine(juce::Graphics&, int);
    void drawVertLine(juce::Graphics&, int);
    void drawMainVertLine(juce::Graphics&, int);
    void drawGainLabel(juce::Graphics&, int, int);
    void drawFreqLabel(juce::Graphics&, int, int);

    // === Other Helper Functions =============================================
    int getXForFrequency(float);
    bool shouldDrawFreqLabel(int);
    juce::Colour getColorForGainLabels();
    juce::Colour getColorForFreqLabels();
    static juce::NormalisableRange<float> getFreqRange();
};