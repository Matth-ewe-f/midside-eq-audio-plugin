#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "ParameterControl.h"
#include "CtmToggle.h"
#include "FilterControl.h"
#include "HighPassControl.h"
#include "PeakFilterControl.h"
#include "LowPassControl.h"
#include "Linkable.h"
#include "Icon.h"
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
    void setupGain
    (ParameterControl*, juce::AudioProcessorValueTreeState*, std::string);
    template<linkable T>
    void setupLinkButton(ParameterToggle*, T*, T*);
    void setupFilterIcon(Icon*, Icon::Type);

private:
    /// === Private Variables =================================================
    PluginProcessor& processorRef;
    CtmLookAndFeel lookAndFeel;
    // buttons
    ParameterToggle midSideButton;
    ParameterToggle leftRightButton;
    ParameterToggle highPassLink;
    ParameterToggle peakOneTwoLink;
    ParameterToggle peakThreeFourLink;
    ParameterToggle peakFiveSixLink;
    ParameterToggle lowPassLink;
    // channel gains
    ParameterControl gainOne;
    ParameterControl gainTwo;
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
    // filter icons
    Icon hpfOneIcon;
    Icon hpfTwoIcon;
    Icon peakOneIcon;
    Icon peakTwoIcon;
    Icon peakThreeIcon;
    Icon peakFourIcon;
    Icon peakFiveIcon;
    Icon peakSixIcon;
    Icon lpfOneIcon;
    Icon lpfTwoIcon;
    
    // === Layout constants ===================================================
    inline static const int headerHeight { 50 };
    inline static const int xStart { 16 };
    inline static const int xEnd { 16 };
    inline static const int yStart { 10 };
    inline static const int yEnd { 10 };
    inline static const int gainWidth { 64 };
    inline static const int cellWidth { 104 };
    inline static const int cellHeight { 102 };
    inline static const int cellMarginX { 12 };
    inline static const int cellMarginY { 48 };
    inline static const int cellPaddingX { 6 };
    inline static const int columnPaddingY { 40 };
    inline static const int columnBgCurvature { 24 };
    inline static const int intraCellPaddingX { 0 };
    inline static const int intraCellPaddingY { 8 };
    inline static const int maxRows { 2 };
    inline static const int maxCols { 5 };

    // === Color Constants ====================================================
    inline static const Colour midColor { Colour::fromRGB(252, 32, 250) };
    inline static const Colour sideColor { Colour::fromRGB(12, 208, 255) };
    inline static const Colour leftColor { Colour::fromRGB(250, 50, 40) };
    inline static const Colour rightColor { Colour::fromRGB(60, 230, 10) };

    // === Drawing and Layout Helper Functions ================================
    void layoutFilter(FilterControl*, int xIndex, int yIndex);
    void layoutGain(ParameterControl*, int yIndex);
    void layoutLinkButton(CtmToggle*, int);
    void layoutFilterIcon(Icon*, int xIndex, int yIndex);
    void layoutTest(juce::Graphics&, int, int);
    void drawSectionLabels(juce::Graphics&);
    void drawFilterBackground(juce::Graphics&, int);
    void drawGainBackground(juce::Graphics&);
    void drawGainLabels(juce::Graphics&);

    // === Other Helper Functions =============================================
    void setColorOverrides();
    juce::Colour getColorOne();
    juce::Colour getColorTwo();
    int secondSectionStart();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};