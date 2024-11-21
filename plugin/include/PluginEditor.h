#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "SliderLabel.h"
#include "ParameterControls.h"
#include "CtmLookAndFeel.h"
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment; 

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
    void addParametricEqControl(ParametricEqControl*);
    void addLowPassControl(LowPassControl*);

private:
    /// === Private Variables =================================================
    PluginProcessor& processorRef;
    CtmLookAndFeel lookAndFeel;
    // buttons
    juce::TextButton midSideButton;
    juce::TextButton leftRightButton;
    // filters
    HighPassControl highPassOne;
    HighPassControl highPassTwo;
    ParametricEqControl parametricOne;
    ParametricEqControl parametricTwo;
    ParametricEqControl parametricThree;
    ParametricEqControl parametricFour;
    ParametricEqControl parametricFive;
    ParametricEqControl parametricSix;
    LowPassControl lowPassOne;
    LowPassControl lowPassTwo;
    
    // === Layout constants ===================================================
    const int headerHeight = 50;
    const int xStart = 48;
    const int xEnd = 16;
    const int yStart = 10;
    const int yEnd = 10;
    const int cellWidth = 104;
    const int cellHeight = 102;
    const int cellMarginX = 12;
    const int cellMarginY = 40;
    const int cellPaddingX = 6;
    const int columnPaddingY = 12;
    const int columnBgCurvature = 24;
    const int intraCellPaddingX = 0;
    const int intraCellPaddingY = 8;
    const int maxRows = 2;
    const int maxCols = 5;

    // === Color Constants ====================================================
    const juce::Colour midColor = juce::Colour::fromRGB(252, 32, 250);
    const juce::Colour sideColor = juce::Colour::fromRGB(12, 208, 255);
    const juce::Colour leftColor = juce::Colour::fromRGB(250, 50, 40);
    const juce::Colour rightColor = juce::Colour::fromRGB(60, 230, 10);

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
    juce::Path getParametricEqIcon(int x, int y, int w, int h);
    juce::Path getLowPassFilterIcon(int x, int y, int w, int h);
    int secondSectionStart();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};