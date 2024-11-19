#include "PluginProcessor.h"
#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // calculate size
    int w = (itemWidth * maxCols) + (xPadding * (maxCols - 1)) + xStart + xEnd;
    int h = (itemHeight * maxRows) + (yPadding * (maxRows - 2)) + yStart + yEnd
        + ySectionBreak + headerHeight;
    setSize(w, h);
    // setup sliders
    setupFreqSlider(&lowPassOneFreq, &lowPassOneFreqLabel);
    lowPassOneFreqAttachment.reset(
        new SliderAttachment(processorRef.tree, "freq-one", lowPassOneFreq)
    );
    setupFreqSlider(&lowPassTwoFreq, &lowPassTwoFreqLabel);
    lowPassTwoFreqAttachment.reset(
        new SliderAttachment(processorRef.tree, "freq-two", lowPassTwoFreq)
    );
    setupOrderSlider(&lowPassOneOrder, &lowPassOneOrderLabel);
    setupOrderSlider(&lowPassTwoOrder, &lowPassTwoOrderLabel);
    // setup buttons
    midSideButton.setButtonText("Mid-Side");
    midSideButton.setRadioGroupId(0, juce::dontSendNotification);
    midSideButton.onClick = [this] { 
        processorRef.tree.getParameter("mode")->setValue(0);
        repaint(); 
    };
    addAndMakeVisible(midSideButton);
    leftRightButton.setButtonText("Left-Right");
    leftRightButton.setRadioGroupId(0, juce::dontSendNotification);
    leftRightButton.onClick = [this] { 
        processorRef.tree.getParameter("mode")->setValue(1);
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
    g.setColour(lineColor);
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    g.drawLine(0, headerHeight, getWidth(), headerHeight);
    g.drawLine(0, secondRowStart(), getWidth(), secondRowStart());
}

void PluginEditor::resized()
{
    layoutSlider(&lowPassOneFreq, &lowPassOneFreqLabel, 1, 0);
    layoutSlider(&lowPassOneOrder, &lowPassOneOrderLabel, 1, 1);
    layoutSlider(&lowPassTwoFreq, &lowPassTwoFreqLabel, 0, 2);
    layoutSlider(&lowPassTwoOrder, &lowPassTwoOrderLabel, 1, 2);
    int middle = getWidth() / 2;
    midSideButton.setBounds(middle - 80, 10, 70, 30);
    leftRightButton.setBounds(middle + 10, 10, 70, 30);
}

// === Private Helper Functions ===============================================
void PluginEditor::setupFreqSlider(juce::Slider* slider, SliderLabel* label)
{
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider->addListener(label);
    addAndMakeVisible(slider);
    label->setPostfix(" Hz");
    label->updateText(slider);
    addAndMakeVisible(label);
}

void PluginEditor::setupOrderSlider(juce::Slider* slider, SliderLabel* label)
{
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider->setRange(6, 36, 6);
    slider->addListener(label);
    addAndMakeVisible(slider);
    label->setPrefix("-");
    label->setPostfix(" dB/oct");
    label->updateText(slider);
    addAndMakeVisible(label);
}

void PluginEditor::layoutSlider
(juce::Slider* slider, SliderLabel* label, int xIndex, int yIndex)
{
    int x = xStart + ((itemWidth + xPadding) * xIndex);
    int y = headerHeight + yStart + ((itemHeight + yPadding) * yIndex);
    if (yIndex >= 2)
    {
        y += ySectionBreak;
        y -= yPadding;
    }
    slider->setBounds(x, y - 9, itemWidth, itemHeight - 2);
    label->setBounds(x, y + itemHeight - 17, itemWidth, 16);
}

void PluginEditor::layoutTest(juce::Graphics& g, int xIndex, int yIndex)
{
    int x = xStart + ((itemWidth + xPadding) * xIndex);
    int y = headerHeight + yStart + ((itemHeight + yPadding) * yIndex);
    if (yIndex >= 2)
    {
        y += ySectionBreak;
        y -= yPadding;
    }
    g.setColour(juce::Colours::red);
    g.fillRect(x, y, itemWidth, itemHeight);
}

void PluginEditor::drawSectionLabels(juce::Graphics& g)
{
    // define the position and area of the labels
    bool ms = processorRef.isMidSide();
    int x1 = ms ? 30 : 31;
    int y1 = headerHeight + 20;
    int x2 = ms ? 33 : 40;
    int y2 = secondRowStart() + 20;
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
    g.drawText(ms ? "Side" : "Right", 4, secondRowStart() + 1, 40, 18, false);
}

int PluginEditor::secondRowStart()
{
    return (headerHeight + yStart + (itemHeight * 2) + yPadding
        + (ySectionBreak / 2));
}