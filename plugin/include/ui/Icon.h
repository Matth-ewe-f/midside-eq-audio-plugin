#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmFilter.h"

class Icon :
    public juce::Component,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    enum class Type
    {
        LowPass,
        Peak,
        HighPass
    };

    // === Lifecycle ==========================================================
    Icon();
    ~Icon() override;

    // === Graphics ===========================================================
    void setType(Type, int x, int y, int w, int h);
    void setColor(juce::Colour);
    void setEnabled(bool);
    void paint(juce::Graphics&) override;

    // === Listen to Parameters ===============================================
    void attachToFilter(juce::AudioProcessorValueTreeState*, CtmFilter*);
    void parameterChanged(const juce::String&, float) override;

private:
    juce::Path path;
    juce::Colour color;
    bool enabled;
    juce::AudioProcessorValueTreeState* tree;
    std::string parameterName;

    // === Factory Functions for Paths ========================================
    static juce::Path getHighPassFilterIcon(int x, int y, int w, int h);
    static juce::Path getPeakFilterIcon(int x, int y, int w, int h);
    static juce::Path getLowPassFilterIcon(int x, int y, int w, int h);
};