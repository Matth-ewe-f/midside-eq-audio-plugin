#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    int w = (itemSize * maxCols) + (xPadding * (maxCols - 1)) + (2 * xBound);
    int h = (itemSize * maxRows) + (yPadding * (maxRows - 1)) + yStart + yEnd;
    setSize(w, h);
    setupSlider(&midFreqSlider);
    midFreqSlider.setValue(*processorRef.midFreq);
    setupSlider(&sideFreqSlider);
    sideFreqSlider.setValue(*processorRef.sideFreq);
}

PluginEditor::~PluginEditor() { }

// === Graphics ===============================================================
void PluginEditor::paint(juce::Graphics &g)
{
    auto colorId = juce::ResizableWindow::backgroundColourId;
    g.fillAll(getLookAndFeel().findColour(colorId));
    // draw lines seperating the mid and side sections
    int secondRowStart = yStart + itemSize + (yPadding / 2);
    g.setColour(juce::Colour::fromRGB(200, 200, 200));
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    g.drawLine(0, secondRowStart, getWidth(), secondRowStart);
    g.drawLine(0, 20, 30, 20);
    g.drawLine(30, 20, 36, 0);
    g.drawLine(0, secondRowStart + 20, 34, secondRowStart + 20);
    g.drawLine(34, secondRowStart + 20, 40, secondRowStart);
    // draw text showing mid and side sections
    g.setFont(16);
    g.setColour(juce::Colours::white);
    g.drawText("Mid", 4, 1, 40, 18, false);
    g.drawText("Side", 4, secondRowStart + 1, 40, 18, false);
}

void PluginEditor::resized()
{
    layoutSlider(&midFreqSlider, 0, 0);
    layoutSlider(&sideFreqSlider, 0, 1);

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
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setRange(20, 20000, 0.1f);
    slider->setSkewFactorFromMidPoint(1500);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    slider->setTextValueSuffix(" Hz");
    slider->addListener(this);
    addAndMakeVisible(slider);
}

void PluginEditor::layoutSlider(juce::Slider* slider, int xIndex, int yIndex)
{
    int x = xBound + ((itemSize + xPadding) * xIndex);
    int y = yStart + ((itemSize + yPadding) * yIndex);
    slider->setBounds(x, y, itemSize, itemSize);
}