#pragma once
#include "PluginProcessor.h"
#include "SliderLabel.h"
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
    juce::TextButton midSideButton;
    juce::TextButton leftRightButton;
    juce::Slider lowPassOneFreq;
    std::unique_ptr<SliderAttachment> lowPassOneFreqAttachment;
    SliderLabel lowPassOneFreqLabel;
    juce::Slider lowPassOneOrder;
    SliderLabel lowPassOneOrderLabel;
    juce::Slider lowPassTwoFreq;
    std::unique_ptr<SliderAttachment> lowPassTwoFreqAttachment;
    SliderLabel lowPassTwoFreqLabel;
    juce::Slider lowPassTwoOrder;
    SliderLabel lowPassTwoOrderLabel;
    
    // === Layout constants ===================================================
    const int headerHeight = 50;
    const int xStart = 40;
    const int xEnd = 8;
    const int yStart = 8;
    const int yEnd = 8;
    const int xPadding = 2;
    const int yPadding = 6;
    const int ySectionBreak = 16;
    const int itemWidth = 68;
    const int itemHeight = 58;
    const int maxRows = 4;
    const int maxCols = 4;
    const juce::Colour lineColor = juce::Colour::fromRGB(200, 200, 200);
    const juce::Colour brighterBgColor = juce::Colour::fromRGB(30, 62, 72);

    // === Private Helper =====================================================
    void setupFreqSlider(juce::Slider*, SliderLabel*);
    void setupOrderSlider(juce::Slider*, SliderLabel*);
    void layoutSlider(juce::Slider*, SliderLabel*, int, int);
    void layoutTest(juce::Graphics&, int, int);
    void drawSectionLabels(juce::Graphics&);
    int secondRowStart();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
