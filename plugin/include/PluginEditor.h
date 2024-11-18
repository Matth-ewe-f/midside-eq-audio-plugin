#pragma once
#include "PluginProcessor.h"
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment; 

class PluginEditor final :  public juce::AudioProcessorEditor
{
public:
    // === Lifecycle ==========================================================
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    // === Graphics ===========================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& processorRef;
    juce::Slider lowPassOneFreq;
    std::unique_ptr<SliderAttachment> lowPassOneFreqAttachment;
    juce::Slider lowPassTwoFreq;
    std::unique_ptr<SliderAttachment> lowPassTwoFreqAttachment;
    juce::TextButton midSideButton;
    juce::TextButton leftRightButton;
    
    // === Layout constants ===================================================
    const int headerHeight = 50;
    const int xBound = 10;
    const int yStart = 10;
    const int yEnd = 20;
    const int xPadding = 10;
    const int yPadding = 30;
    const int itemSize = 100;
    const int maxRows = 2;
    const int maxCols = 2;
    const juce::Colour lineColor = juce::Colour::fromRGB(200, 200, 200);
    const juce::Colour brighterBgColor = juce::Colour::fromRGB(30, 62, 72);

    // === Private Helper =====================================================
    void setupSlider(juce::Slider*);
    void layoutSlider(juce::Slider*, int, int);
    void drawSectionLabels(juce::Graphics&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
