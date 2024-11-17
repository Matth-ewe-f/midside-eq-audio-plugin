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

    // === User Interaction ===================================================
    void sliderValueChanged(juce::Slider*) override;

    // === Private Helper =====================================================
    void setupSlider(juce::Slider*);
    void layoutSlider(juce::Slider*, int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
