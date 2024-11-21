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
    addParametricEqControl(&parametricOne);
    addParametricEqControl(&parametricTwo);
    addParametricEqControl(&parametricThree);
    addParametricEqControl(&parametricFour);
    addParametricEqControl(&parametricFive);
    addParametricEqControl(&parametricSix);
    addLowPassControl(&lowPassOne);
    addLowPassControl(&lowPassTwo);
    highPassOne.attachToHighPass(
        &processorRef.tree, "hpf1-freq", "hpf1-falloff", "hpf1-res"
    );
    highPassTwo.attachToHighPass(
        &processorRef.tree, "hpf2-freq", "hpf2-falloff", "hpf2-res"
    );
    parametricOne.attachToParametricEq(
        &processorRef.tree, "para1-freq", "para1-gain", "para1-q"
    );
    parametricTwo.attachToParametricEq(
        &processorRef.tree, "para2-freq", "para2-gain", "para2-q"
    );
    parametricThree.attachToParametricEq(
        &processorRef.tree, "para3-freq", "para3-gain", "para3-q"
    );
    parametricFour.attachToParametricEq(
        &processorRef.tree, "para4-freq", "para4-gain", "para4-q"
    );
    parametricFive.attachToParametricEq(
        &processorRef.tree, "para5-freq", "para5-gain", "para5-q"
    );
    parametricSix.attachToParametricEq(
        &processorRef.tree, "para6-freq", "para6-gain", "para6-q"
    );
    lowPassOne.attachToLowPass(
        &processorRef.tree, "lpf1-freq", "lpf1-falloff", "lpf1-res"
    );
    lowPassTwo.attachToLowPass(
        &processorRef.tree, "lpf2-freq", "lpf2-falloff", "lpf2-res"
    );
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
    // draw the icons for the filters
    drawFilterIcons(g);
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

// === Functions for Custom Components ========================================
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
}

void PluginEditor::addParametricEqControl(ParametricEqControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->gain);
    addParameterControl(&control->qFactor);
}

void PluginEditor::addLowPassControl(LowPassControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->falloff);
    addParameterControl(&control->resonance);
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
        g.fillRect(cx - 22, cy - 16, 44, 32);
        juce::Path icon;
        if (i == 0)
            icon = getHighPassFilterIcon(cx - 12, cy - 8, 24, 16);
        else if (i == maxCols - 1)
            icon = getLowPassFilterIcon(cx - 12, cy - 8, 24, 16);
        else
            icon = getParametricEqIcon(cx - 14, cy - 11, 28, 22);
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

juce::Path PluginEditor::getParametricEqIcon(int x, int y, int w, int h)
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