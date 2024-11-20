#include <juce_audio_processors/juce_audio_processors.h>
#include "SliderLabel.h"
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment; 

struct ParameterControl
{
    juce::Slider slider;
    std::unique_ptr<SliderAttachment> attachment;
    SliderLabel label;
};