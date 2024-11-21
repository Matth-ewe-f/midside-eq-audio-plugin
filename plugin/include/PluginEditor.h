#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
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

private:
    /// === Private Variables =================================================
    PluginProcessor& processorRef;
    CtmLookAndFeel lookAndFeel;
    // buttons
    juce::TextButton midSideButton;
    juce::TextButton leftRightButton;
    // filters
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
    const int cellMarginY = 24;
    const int cellPaddingX = 6;
    const int columnPaddingY = 12;
    const int columnBgCurvature = 24;
    const int intraCellPaddingX = 0;
    const int intraCellPaddingY = 8;
    const int maxRows = 2;
    const int maxCols = 3;

    // === Private Helper =====================================================
    void layoutLowPass(LowPassControl*, int xIndex, int yIndex);
    void layoutTest(juce::Graphics&, int, int);
    void drawSectionLabels(juce::Graphics&);
    void drawFilterBackground(juce::Graphics&, int);
    int secondSectionStart();
    juce::Colour findColour(int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};