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
    juce::Label midLabel;
    juce::Label sideLabel;

    // === User Interaction ===================================================
    void sliderValueChanged(juce::Slider*) override;

    // === Private Helper =====================================================
    void setupSlider(juce::Slider*);
    void setupLabel(juce::Label*, std::string);
    void layoutSlider(juce::Slider*, int);
    void layoutLabel(juce::Label*, int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
