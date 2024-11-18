#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // calculate size
    int w = (itemSize * maxCols) + (xPadding * (maxCols - 1)) + (2 * xBound);
    int h = (itemSize * maxRows) + (yPadding * (maxRows - 1)) + yStart + yEnd;
    h += headerHeight;
    setSize(w, h);
    // setup gui components
    setupSlider(&midFreqSlider);
    midFreqSlider.setValue(*processorRef.freqOne);
    setupSlider(&sideFreqSlider);
    sideFreqSlider.setValue(*processorRef.freqTwo);
    midSideButton.setButtonText("Mid-Side");
    midSideButton.setRadioGroupId(0, juce::dontSendNotification);
    midSideButton.onClick = [this] { 
        *processorRef.isMidSide = true;
        repaint(); 
    };
    addAndMakeVisible(midSideButton);
    leftRightButton.setButtonText("Left-Right");
    leftRightButton.setRadioGroupId(0, juce::dontSendNotification);
    leftRightButton.onClick = [this] { 
        *processorRef.isMidSide = false;
        repaint();
    };
    addAndMakeVisible(leftRightButton);
}

PluginEditor::~PluginEditor() { }

// === Graphics ===============================================================
void PluginEditor::paint(juce::Graphics &g)
{
    auto colorId = juce::ResizableWindow::backgroundColourId;
    g.fillAll(getLookAndFeel().findColour(colorId));
    // draw lines seperating the mid and side sections
    drawSectionLabels(g);
    int secondRowStart = headerHeight + yStart + itemSize + (yPadding / 2);
    g.setColour(lineColor);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    g.drawLine(0, headerHeight, getWidth(), headerHeight);
    g.drawLine(0, secondRowStart, getWidth(), secondRowStart);
}

void PluginEditor::resized()
{
    layoutSlider(&midFreqSlider, 0, 0);
    layoutSlider(&sideFreqSlider, 0, 1);
    int middle = getWidth() / 2;
    midSideButton.setBounds(middle - 80, 10, 70, 30);
    leftRightButton.setBounds(middle + 10, 10, 70, 30);
}

// === User Interaction =======================================================
void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &midFreqSlider)
    {
        *processorRef.freqOne = (float) slider->getValue();
        processorRef.updateFilterState();
    }
    else if (slider == &sideFreqSlider)
    {
        *processorRef.freqTwo = (float) slider->getValue();
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
    int y = headerHeight + yStart + ((itemSize + yPadding) * yIndex);
    slider->setBounds(x, y, itemSize, itemSize);
}

void PluginEditor::drawSectionLabels(juce::Graphics& g)
{
    // define the position and area of the labels
    bool ms = *processorRef.isMidSide;
    int secondRowStart = headerHeight + yStart + itemSize + (yPadding / 2);
    int x1 = ms ? 30 : 31;
    int y1 = headerHeight + 20;
    int x2 = ms ? 33 : 40;
    int y2 = secondRowStart + 20;
    // fill the labels with background color
    g.setColour(brighterBgColor);
    g.fillRect(0, y1 - 20, x1, 20);
    juce::Path triangle1;
    triangle1.addTriangle(x1, y1 - 20, x1 + 4, y1 - 20, x1, y1);
    g.fillPath(triangle1);
    g.fillRect(0, y2 - 20, x2, 20);
    juce::Path triangle2;
    triangle2.addTriangle(x2, y2 - 20, x2 + 4, y2 - 20, x2, y2);
    g.fillPath(triangle2);
    // draw the outlines around the labels
    g.setColour(lineColor);
    g.drawLine(0, y1, x1, y1);
    g.drawLine(x1, y1, x1 + 4, y1 - 20);
    g.drawLine(0, y2, x2, y2);
    g.drawLine(x2, y2, x2 + 4, y2 - 20);
    // draw text of the labels
    g.setFont(16);
    g.setColour(juce::Colours::white);
    g.drawText(ms ? "Mid" : "Left", 4, headerHeight + 1, 40, 18, false);
    g.drawText(ms ? "Side" : "Right", 4, secondRowStart + 1, 40, 18, false);
}