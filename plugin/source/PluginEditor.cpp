#include "PluginEditor.h"
#include "PluginProcessor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&lookAndFeel);
    setWantsKeyboardFocus(true);
    // calculate size
    int w = xStart + (cellWidth * maxCols) + (cellMarginX * (maxCols - 1))
        + (cellPaddingX * (maxCols * 2)) + xEnd;
    int h = headerHeight + yStart + (cellHeight * maxRows)
        + (cellMarginY * (maxRows - 1)) + (columnPaddingY * 2) + yEnd;
    setSize(w, h);
    // setup sliders
    highPassOne.addToEditor(this);
    highPassTwo.addToEditor(this);
    parametricOne.addToEditor(this);
    parametricTwo.addToEditor(this);
    parametricThree.addToEditor(this);
    parametricFour.addToEditor(this);
    parametricFive.addToEditor(this);
    parametricSix.addToEditor(this);
    lowPassOne.attachToLowPass(
        &processorRef, "lpf1-freq", "lpf1-falloff", "lpf1-res"
    );
    lowPassOne.addToEditor(this);
    lowPassTwo.attachToLowPass(
        &processorRef, "lpf2-freq", "lpf2-falloff", "lpf2-res"
    );
    lowPassTwo.addToEditor(this);
    setColorOverrides();
    // setup buttons
    midSideButton.setButtonText("Mid-Side");
    midSideButton.setRadioGroupId(0, juce::dontSendNotification);
    midSideButton.onClick = [this] { 
        processorRef.tree.getParameter("mode")->setValue(0);
        setColorOverrides();
        repaint(); 
    };
    addAndMakeVisible(midSideButton);
    leftRightButton.setButtonText("Left-Right");
    leftRightButton.setRadioGroupId(0, juce::dontSendNotification);
    leftRightButton.onClick = [this] { 
        processorRef.tree.getParameter("mode")->setValue(1);
        setColorOverrides();
        repaint();
    };
    addAndMakeVisible(leftRightButton);
}

PluginEditor::~PluginEditor() { }

// === Graphics ===============================================================
void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
    // draw backgrounds behind each filter's contollers
    for (int i = 0;i < maxCols;i++)
        drawFilterBackground(g, i);
    // draw lines seperating the mid and side sections
    drawSectionLabels(g);
    // draw outlines around sections and whole component
    g.setColour(findColour(CtmColourIds::brightOutlineColourId));
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    g.drawLine(0, headerHeight, getWidth(), headerHeight);
    g.drawLine(0, secondSectionStart(), getWidth(), secondSectionStart());
}

void PluginEditor::resized()
{
    layoutFilter(&highPassOne, 0, 0);
    layoutFilter(&highPassTwo, 0, 1);
    layoutFilter(&parametricOne, 1, 0);
    layoutFilter(&parametricTwo, 1, 1);
    layoutFilter(&parametricThree, 2, 0);
    layoutFilter(&parametricFour, 2, 1);
    layoutFilter(&parametricFive, 3, 0);
    layoutFilter(&parametricSix, 3, 1);
    layoutFilter(&lowPassOne, 4, 0);
    layoutFilter(&lowPassTwo, 4, 1);
    int middle = getWidth() / 2;
    midSideButton.setBounds(middle - 80, 10, 70, 30);
    leftRightButton.setBounds(middle + 10, 10, 70, 30);
}

// === Private Helper Functions ===============================================
void PluginEditor::layoutFilter
(FilterControl* lowPass, int xIndex, int yIndex)
{
    int x = xStart + ((cellWidth + cellMarginX) * xIndex)
        + (cellPaddingX * ((2 * xIndex) + 1));
    int y = headerHeight + yStart + columnPaddingY 
        + ((cellHeight + cellMarginY) * yIndex);
    lowPass->setBounds(
        x, y, cellWidth, cellHeight, intraCellPaddingX, intraCellPaddingY
    );
}

void PluginEditor::layoutTest(juce::Graphics& g, int xIndex, int yIndex)
{
    int x = xStart + ((cellWidth + cellMarginX) * xIndex)
        + (cellPaddingX * ((2 * xIndex) - 1));
    int y = headerHeight + yStart + columnPaddingY 
        + ((cellHeight + cellMarginY) * yIndex);
    g.setColour(juce::Colours::red);
    g.fillRect(x, y, cellWidth, cellHeight);
}

void PluginEditor::drawSectionLabels(juce::Graphics& g)
{
    // define the position and area of the labels
    bool ms = processorRef.isMidSide();
    int secondSectionY = secondSectionStart();
    int x1 = ms ? 30 : 31;
    int y1 = headerHeight + 20;
    int x2 = ms ? 33 : 40;
    int y2 = secondSectionY + 20;
    // fill the labels with background color
    g.setColour(findColour(CtmColourIds::brightBgColourId));
    g.fillRect(0, y1 - 20, x1, 20);
    juce::Path triangle1;
    triangle1.addTriangle(x1, y1 - 20, x1 + 4, y1 - 20, x1, y1);
    g.fillPath(triangle1);
    g.fillRect(0, y2 - 20, x2, 20);
    juce::Path triangle2;
    triangle2.addTriangle(x2, y2 - 20, x2 + 4, y2 - 20, x2, y2);
    g.fillPath(triangle2);
    // draw the outlines around the labels
    g.setColour(findColour(CtmColourIds::brightOutlineColourId));
    g.drawLine(0, y1, x1, y1);
    g.drawLine(x1, y1, x1 + 4, y1 - 20);
    g.drawLine(0, y2, x2, y2);
    g.drawLine(x2, y2, x2 + 4, y2 - 20);
    // draw text of the labels
    g.setFont(16);
    g.setColour(juce::Colours::white.interpolatedWith(getColorOne(), 0.7f));
    g.drawText(ms ? "Mid" : "Left", 4, headerHeight + 1, 40, 18, false);
    g.setColour(juce::Colours::white.interpolatedWith(getColorTwo(), 0.7f));
    g.drawText(ms ? "Side" : "Right", 4, secondSectionY + 1, 40, 18, false);
}

void PluginEditor::drawFilterBackground(juce::Graphics& g, int index)
{
    juce::Path p;
    float x = xStart + ((cellWidth + (cellPaddingX*2) + cellMarginX) * index);
    float y = headerHeight + yStart;
    float w = cellWidth + (cellPaddingX * 2);
    float h = (cellHeight * maxRows) + (cellMarginY * (maxRows - 1))
        + (columnPaddingY * 2);
    p.addRoundedRectangle(x, y, w, h, columnBgCurvature);
    g.setColour(findColour(CtmColourIds::darkBgColourId));
    g.fillPath(p);
}

void PluginEditor::setColorOverrides()
{
    highPassOne.setAllColorOverrides(getColorOne());
    parametricOne.setAllColorOverrides(getColorOne());
    parametricThree.setAllColorOverrides(getColorOne());
    parametricFive.setAllColorOverrides(getColorOne());
    lowPassOne.setAllColorOverrides(getColorOne());
    highPassTwo.setAllColorOverrides(getColorTwo());
    parametricTwo.setAllColorOverrides(getColorTwo());
    parametricFour.setAllColorOverrides(getColorTwo());
    parametricSix.setAllColorOverrides(getColorTwo());
    lowPassTwo.setAllColorOverrides(getColorTwo());
}

juce::Colour PluginEditor::getColorOne()
{
    return processorRef.isMidSide() ? midColor : leftColor;
}

juce::Colour PluginEditor::getColorTwo()
{
    return processorRef.isMidSide() ? sideColor : rightColor;
}

int PluginEditor::secondSectionStart()
{
    return headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2);
}

juce::Colour PluginEditor::findColour(int colourId)
{
    return getLookAndFeel().findColour(colourId);
}