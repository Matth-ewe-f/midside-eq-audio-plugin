#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(290, 260);
    setupSlider(&midFreqSlider);
    setupSlider(&sideFreqSlider);
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
    layoutSlider(&sideFreqSlider, 0);
    layoutSlider(&midFreqSlider, 1);
}

// === User Interaction =======================================================
void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &midFreqSlider)
    {
        *processorRef.freq = (float) slider->getValue();
        processorRef.updateFilterState();
    }
}

// === Private Helper Functions ===============================================
void PluginEditor::setupSlider(juce::Slider* slider)
{
    slider->setSliderStyle(juce::Slider::LinearVertical);
    slider->setRange(20, 20000, 0.1f);
    slider->setSkewFactorFromMidPoint(1500);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
    slider->setTextValueSuffix(" Hz");
    slider->addListener(this);
    addAndMakeVisible(*slider);
}

void PluginEditor::layoutSlider(juce::Slider* slider, int index)
{
    int start = 50;
    int width = 120;
    slider->setBounds(start + (width * index), 30, 90, getHeight() - 60);
}