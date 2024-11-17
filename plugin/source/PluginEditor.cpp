#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(290, 260);
    freqSlider.setSliderStyle(juce::Slider::LinearVertical);
    freqSlider.setRange(20, 20000, 0.1f);
    freqSlider.setSkewFactorFromMidPoint(2000);
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.addListener(this);
    addAndMakeVisible(freqSlider);
}

PluginEditor::~PluginEditor() { }

// === Graphics ===============================================================
void PluginEditor::paint(juce::Graphics &g)
{
    auto colorId = juce::ResizableWindow::backgroundColourId;
    g.fillAll(getLookAndFeel().findColour(colorId));
}

void PluginEditor::resized()
{
    freqSlider.setBounds(100, 30, 90, getHeight() - 60);
}

// === User Interaction =======================================================
void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &freqSlider)
    {
        *processorRef.freq = (float) slider->getValue();
        processorRef.updateFilterState();
    }
}