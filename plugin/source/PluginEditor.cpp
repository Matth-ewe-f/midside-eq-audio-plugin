#include "PluginEditor.h"

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
    addHighPassControl(&highPassOne);
    addHighPassControl(&highPassTwo);
    addPeakFilterControl(&peakOne);
    addPeakFilterControl(&peakTwo);
    addPeakFilterControl(&peakThree);
    addPeakFilterControl(&peakFour);
    addPeakFilterControl(&peakFive);
    addPeakFilterControl(&peakSix);
    addLowPassControl(&lowPassOne);
    addLowPassControl(&lowPassTwo);
    juce::AudioProcessorValueTreeState* stateTree = &processorRef.tree;
    highPassOne.attachToHighPass(stateTree, &processorRef.highPassOne);
    highPassTwo.attachToHighPass(stateTree, &processorRef.highPassTwo);
    peakOne.attachToPeakFilter(stateTree, &processorRef.peakOne);
    peakTwo.attachToPeakFilter(stateTree, &processorRef.peakTwo);
    peakThree.attachToPeakFilter(stateTree, &processorRef.peakThree);
    peakFour.attachToPeakFilter(stateTree, &processorRef.peakFour);
    peakFive.attachToPeakFilter(stateTree, &processorRef.peakFive);
    peakSix.attachToPeakFilter(stateTree, &processorRef.peakSix);
    lowPassOne.attachToLowPass(stateTree, &processorRef.lowPassOne);
    lowPassTwo.attachToLowPass(stateTree, &processorRef.lowPassTwo);
    // setup link buttons
    setupLinkButton(&highPassLink, &highPassOne, &highPassTwo);
    highPassLink.attachToParameter(stateTree, "hpf-linked");
    setupLinkButton(&peakOneTwoLink, &peakOne, &peakTwo);
    peakOneTwoLink.attachToParameter(stateTree, "peak12-linked");
    setupLinkButton(&peakThreeFourLink, &peakThree, &peakFour);
    peakThreeFourLink.attachToParameter(stateTree, "peak34-linked");
    setupLinkButton(&peakFiveSixLink, &peakFive, &peakSix);
    peakFiveSixLink.attachToParameter(stateTree, "peak56-linked");
    setupLinkButton(&lowPassLink, &lowPassOne, &lowPassTwo);
    lowPassLink.attachToParameter(stateTree, "lpf-linked");
    // setup mode buttons
    midSideButton.toggle.setText("Mid-Side");
    midSideButton.onToggle = [this] (bool b) { 
        processorRef.tree.getParameter("mode")->setValue(0);
        setColorOverrides();
        repaint(); 
    };
    addAndMakeVisible(midSideButton.toggle);
    leftRightButton.toggle.setText("Left-Right");
    leftRightButton.onToggle = [this] (bool b) { 
        processorRef.tree.getParameter("mode")->setValue(1);
        setColorOverrides();
        repaint();
    };
    addAndMakeVisible(leftRightButton.toggle);
    // setup colors
    setColorOverrides();
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
    // draw the icons for the filters
    drawFilterIcons(g);
}

void PluginEditor::resized()
{
    layoutFilter(&highPassOne, 0, 0);
    layoutFilter(&highPassTwo, 0, 1);
    layoutFilter(&peakOne, 1, 0);
    layoutFilter(&peakTwo, 1, 1);
    layoutFilter(&peakThree, 2, 0);
    layoutFilter(&peakFour, 2, 1);
    layoutFilter(&peakFive, 3, 0);
    layoutFilter(&peakSix, 3, 1);
    layoutFilter(&lowPassOne, 4, 0);
    layoutFilter(&lowPassTwo, 4, 1);
    layoutLinkButton(&highPassLink.toggle, 0);
    layoutLinkButton(&peakOneTwoLink.toggle, 1);
    layoutLinkButton(&peakThreeFourLink.toggle, 2);
    layoutLinkButton(&peakFiveSixLink.toggle, 3);
    layoutLinkButton(&lowPassLink.toggle, 4);
    int middle = getWidth() / 2;
    midSideButton.setBounds(middle - 110, 5, 100, 40);
    leftRightButton.setBounds(middle + 10, 5, 100, 40);
}

// === Functions for Custom Components ========================================
template <linkable T>
void PluginEditor::setupLinkButton
(ParameterToggle* linkButton, T* item1, T* item2)
{
    linkButton->toggle.setText("LINK");
    linkButton->onToggle = [item1, item2] (bool toggled)
    {
        if (toggled)
        {
            item1->link(item2);
            item2->link(item1);
        }
        else
        {
            item1->unlink(item2);
            item2->unlink(item1);
        }
    };
    addAndMakeVisible(&linkButton->toggle);
}

void PluginEditor::addParameterControl(ParameterControl* control)
{
    addAndMakeVisible(control->slider);
    addAndMakeVisible(control->label);
}

void PluginEditor::addHighPassControl(HighPassControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->falloff);
    addParameterControl(&control->resonance);
    addAndMakeVisible(&control->onOff.toggle);
}

void PluginEditor::addPeakFilterControl(PeakFilterControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->gain);
    addParameterControl(&control->qFactor);
    addAndMakeVisible(&control->onOff.toggle);
}

void PluginEditor::addLowPassControl(LowPassControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->falloff);
    addParameterControl(&control->resonance);
    addAndMakeVisible(&control->onOff.toggle);
}

// === Drawing and Layout Helper Functions ====================================
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

void PluginEditor::layoutLinkButton(CtmToggle* linkButton, int index)
{
    int x = xStart + ((cellMarginX + cellWidth) * index)
        + (cellPaddingX * ((index * 2) + 1)) + (cellWidth / 2);
    int cy = headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2);
    linkButton->setBounds(x - 32, cy - 10, 30, 21);
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

void PluginEditor::drawFilterIcons(juce::Graphics& g)
{
    int cy = headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2);
    g.setFont(14);
    for (int i = 0;i < maxCols;i++)
    {
        int cx = xStart + ((cellMarginX + cellWidth) * i)
            + (cellPaddingX * ((i * 2) + 1)) + (cellWidth / 2);
        g.setColour(findColour(CtmColourIds::darkBgColourId));
        g.fillRect(cx - 40, cy - 16, 80, 32);
        juce::Path icon;
        if (i == 0)
            icon = getHighPassFilterIcon(cx + 6, cy - 8, 24, 16);
        else if (i == maxCols - 1)
            icon = getLowPassFilterIcon(cx + 6, cy - 8, 24, 16);
        else
            icon = getPeakFilterIcon(cx + 4, cy - 11, 28, 22);
        g.setColour(juce::Colours::white);
        g.fillPath(icon);
    }
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

// === Other Helper Functions =================================================
void PluginEditor::setColorOverrides()
{
    highPassOne.setAllColorOverrides(getColorOne());
    peakOne.setAllColorOverrides(getColorOne());
    peakThree.setAllColorOverrides(getColorOne());
    peakFive.setAllColorOverrides(getColorOne());
    lowPassOne.setAllColorOverrides(getColorOne());
    highPassTwo.setAllColorOverrides(getColorTwo());
    peakTwo.setAllColorOverrides(getColorTwo());
    peakFour.setAllColorOverrides(getColorTwo());
    peakSix.setAllColorOverrides(getColorTwo());
    lowPassTwo.setAllColorOverrides(getColorTwo());
    highPassLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakOneTwoLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakThreeFourLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakFiveSixLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    lowPassLink.toggle.setColorGradient(getColorOne(), getColorTwo());
}

juce::Colour PluginEditor::getColorOne()
{
    return processorRef.isMidSide() ? midColor : leftColor;
}

juce::Colour PluginEditor::getColorTwo()
{
    return processorRef.isMidSide() ? sideColor : rightColor;
}

juce::Path PluginEditor::getHighPassFilterIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[2];
    lines[0] = juce::Line<float>(0, 1, 0.33f, 0.17f);
    lines[1] = juce::Line<float>(0.33f, 0.17f, 1, 0.17f);
    for (int i = 0;i < 2;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1);
    }
    return path;
}

juce::Path PluginEditor::getLowPassFilterIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[2];
    lines[0] = juce::Line<float>(0, 0.17f, 0.67f, 0.17f);
    lines[1] = juce::Line<float>(0.67f, 0.17f, 1, 1);
    for (int i = 0;i < 2;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1);
    }
    return path;
}

juce::Path PluginEditor::getPeakFilterIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[8];
    lines[0] = juce::Line<float>(0, 0.5f, 0.17f, 0.5f);
    lines[1] = juce::Line<float>(0.17f, 0.5f, 0.37f, 0.17f);
    lines[2] = juce::Line<float>(0.17f, 0.5f, 0.37f, 0.83f);
    lines[3] = juce::Line<float>(0.37f, 0.17f, 0.63f, 0.17f);
    lines[4] = juce::Line<float>(0.37f, 0.83f, 0.63f, 0.83f);
    lines[5] = juce::Line<float>(0.63f, 0.17f, 0.83f, 0.5f);
    lines[6] = juce::Line<float>(0.63f, 0.83f, 0.83f, 0.5f);
    lines[7] = juce::Line<float>(0.8f, 0.5f, 1, 0.5f);
    for (int i = 0;i < 8;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1);
    }
    return path;
}

int PluginEditor::secondSectionStart()
{
    return headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2);
}