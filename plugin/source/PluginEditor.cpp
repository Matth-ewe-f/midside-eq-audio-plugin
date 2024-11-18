#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(290, 260);
    setupSlider(&midFreqSlider);
    setupSlider(&sideFreqSlider);
    setupLabel(&midLabel, "Mid");
    setupLabel(&sideLabel, "Side");
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
    layoutSlider(&midFreqSlider, 0);
    layoutSlider(&sideFreqSlider, 1);
    layoutLabel(&midLabel, 0);
    layoutLabel(&sideLabel, 1);
}

// === User Interaction =======================================================
void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &midFreqSlider)
    {
        *processorRef.midFreq = (float) slider->getValue();
        processorRef.updateFilterState();
    }
    else if (slider == &sideFreqSlider)
    {
        *processorRef.sideFreq = (float) slider->getValue();
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
    addAndMakeVisible(slider);
}

void PluginEditor::setupLabel(juce::Label* label, std::string text)
{
    label->setText(text, juce::dontSendNotification);
    label->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void PluginEditor::layoutSlider(juce::Slider* slider, int index)
{
    int start = 50;
    int width = 120;
    slider->setBounds(start + (width * index), 30, 90, getHeight() - 90);
}

void PluginEditor::layoutLabel(juce::Label* label, int index)
{
    int start = 50;
    int width = 120;
    label->setBounds(start + (width * index), getHeight() - 50, 90, 20);
}