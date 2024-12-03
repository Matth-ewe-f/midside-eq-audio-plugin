#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "CtmSlider.h"
#include "SliderLabel.h"
#include "Linkable.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class ParameterControl :
    juce::AudioProcessorValueTreeState::Listener,
    Linkable<ParameterControl>
{
public:
    std::string parameterName;
    CtmSlider slider;
    SliderLabel label;
    std::unique_ptr<SliderAttachment> attachment;

    // === Lifecycle ==========================================================
    ParameterControl();
    ~ParameterControl() override;

    // === Settings ===========================================================
    void setBounds(juce::Rectangle<int>);
    void setBounds(int x, int y, int width, int height);
    inline juce::Rectangle<int> getBounds() { return bounds; }
    void attachToParameter(juce::AudioProcessorValueTreeState*, std::string);
    void setSliderStyle(juce::Slider::SliderStyle);

    // === Parameter Linking ==================================================
    void link(const ParameterControl*) override;
    void unlink(const ParameterControl*) override;
    void linkParameterBidirectional(ParameterControl*);
    void unlinkParameterBidirectional(ParameterControl*);
    void parameterChanged(const juce::String&, float) override;
private:
    juce::AudioProcessorValueTreeState* tree;
    std::vector<std::string> linkedParameters;
    juce::Rectangle<int> bounds;

    void copyValueToParameter(const std::string, float);
};