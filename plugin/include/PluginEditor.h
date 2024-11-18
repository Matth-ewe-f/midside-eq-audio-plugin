#pragma once
#include "PluginProcessor.h"

class PluginEditor final :
    public juce::AudioProcessorEditor,
    private juce::Slider::Listener
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
    juce::Slider midFreqSlider;
    juce::Slider sideFreqSlider;
    // === Layout constants ===================================================
    const int xBound = 10;
    const int yStart = 10;
    const int yEnd = 20;
    const int xPadding = 10;
    const int yPadding = 30;
    const int itemSize = 100;
    const int maxRows = 2;
    const int maxCols = 2;

    // === User Interaction ===================================================
    void sliderValueChanged(juce::Slider*) override;

    // === Private Helper =====================================================
    void setupSlider(juce::Slider*);
    void layoutSlider(juce::Slider*, int, int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
