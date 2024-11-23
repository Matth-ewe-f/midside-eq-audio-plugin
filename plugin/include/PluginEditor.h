#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "SliderLabel.h"
#include "CtmToggle.h"
#include "ParameterControls.h"
#include "CtmLookAndFeel.h"
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using Colour = juce::Colour;

class PluginEditor final : public juce::AudioProcessorEditor
{
public:
    // === Lifecycle ==========================================================
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    // === Graphics ===========================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // === Functions for Custom Components ====================================
    void addParameterControl(ParameterControl*);
    void addHighPassControl(HighPassControl*);
    void addPeakFilterControl(PeakFilterControl*);
    void addLowPassControl(LowPassControl*);

private:
    /// === Private Variables =================================================
    PluginProcessor& processorRef;
    CtmLookAndFeel lookAndFeel;
    // buttons
    juce::TextButton midSideButton;
    juce::TextButton leftRightButton;
    CtmToggle toggleTestOne;
    CtmToggle toggleTestTwo;
    CtmToggle toggleTestThree;
    CtmToggle toggleTestFour;
    // filters
    HighPassControl highPassOne;
    HighPassControl highPassTwo;
    PeakFilterControl peakOne;
    PeakFilterControl peakTwo;
    PeakFilterControl peakThree;
    PeakFilterControl peakFour;
    PeakFilterControl peakFive;
    PeakFilterControl peakSix;
    LowPassControl lowPassOne;
    LowPassControl lowPassTwo;
    
    // === Layout constants ===================================================
    inline static const int headerHeight { 50 };
    inline static const int xStart { 48 };
    inline static const int xEnd { 16 };
    inline static const int yStart { 10 };
    inline static const int yEnd { 10 };
    inline static const int cellWidth { 104 };
    inline static const int cellHeight { 102 };
    inline static const int cellMarginX { 12 };
    inline static const int cellMarginY { 40 };
    inline static const int cellPaddingX { 6 };
    inline static const int columnPaddingY { 12 };
    inline static const int columnBgCurvature { 24 };
    inline static const int intraCellPaddingX { 0 };
    inline static const int intraCellPaddingY { 8 };
    inline static const int maxRows { 2 };
    inline static const int maxCols { 5 };

    // === Color Constants ====================================================
    inline static const Colour midColor{ Colour::fromRGB(252, 32, 250) };
    inline static const Colour sideColor { Colour::fromRGB(12, 208, 255) };
    inline static const Colour leftColor { Colour::fromRGB(250, 50, 40) };
    inline static const Colour rightColor { Colour::fromRGB(60, 230, 10) };

    // === Drawing and Layout Helper Functions ================================
    void layoutFilter(FilterControl*, int xIndex, int yIndex);
    void layoutTest(juce::Graphics&, int, int);
    void drawSectionLabels(juce::Graphics&);
    void drawFilterIcons(juce::Graphics&);
    void drawFilterBackground(juce::Graphics&, int);

    // === Other Helper Functions =============================================
    void setColorOverrides();
    juce::Colour getColorOne();
    juce::Colour getColorTwo();
    juce::Path getHighPassFilterIcon(int x, int y, int w, int h);
    juce::Path getPeakFilterIcon(int x, int y, int w, int h);
    juce::Path getLowPassFilterIcon(int x, int y, int w, int h);
    int secondSectionStart();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};