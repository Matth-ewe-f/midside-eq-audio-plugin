#include "PluginEditor.h"

// === Lifecycle ==============================================================
PluginEditor::PluginEditor (PluginProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&lookAndFeel);
    setWantsKeyboardFocus(true);
    // setup sub-components
    initializeIcons();
    initializeFilters();
    initializeLinkButtons();
    initializeGlobalControls();
    initializeEqVisual();
    // setup colors
    setColorOverrides();
    // calculate size
    int w = xStart + gainWidth + (cellWidth * maxCols)
        + (cellMarginX * (maxCols + 1)) + (cellPaddingX * (maxCols * 2))
        + globalsMarginL + globalsWidth + (globalsInset * 2) + xEnd;
    int h = headerHeight + yStart + (cellHeight * maxRows)
        + (cellMarginY * (maxRows - 1)) + (columnPaddingY * 2) + yEnd;
    setSize(w, h);
}

PluginEditor::~PluginEditor() { }

// === Graphics ===============================================================
void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
    // draw backgrounds for the global controls
    drawGlobalControlGrouping(g, 4, 3);
    drawGlobalControlGrouping(g, -1, 0);
    // draw backgrounds behind each filter's contollers
    drawGainBackground(g);
    for (int i = 0;i < maxCols;i++)
        drawFilterBackground(g, i);
    // draw lines and text seperating the mid and side sections
    drawSectionLabels(g);
}

void PluginEditor::paintOverChildren(juce::Graphics &g)
{
    // draw outlines around the whole component
    g.setColour(findColour(CtmColourIds::brightOutlineColourId));
    g.drawRect(0, 0, getWidth(), getHeight(), 1);
    g.drawLine(0, headerHeight, getWidth(), headerHeight);
}

void PluginEditor::resized()
{
    layoutGlobalControl(&midSideButton.toggle, 4);
    layoutGlobalControl(&leftRightButton.toggle, 3);
    layoutGlobalControl(&resetButton, 1.5f);
    layoutGlobalControl(&linkAllButton, 0);
    layoutGlobalControl(&unlinkAllButton, -1);
    layoutGain(&gainOne, 0);
    layoutGain(&gainTwo, 1);
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
    layoutLinkButton(&gainLink.toggle, -1);
    layoutLinkButton(&highPassLink.toggle, 0);
    layoutLinkButton(&peakOneTwoLink.toggle, 1);
    layoutLinkButton(&peakThreeFourLink.toggle, 2);
    layoutLinkButton(&peakFiveSixLink.toggle, 3);
    layoutLinkButton(&lowPassLink.toggle, 4);
    layoutFilterIcon(&hpfOneIcon, 0, 0);
    layoutFilterIcon(&hpfTwoIcon, 0, 1);
    layoutFilterIcon(&peakOneIcon, 1, 0);
    layoutFilterIcon(&peakTwoIcon, 1, 1);
    layoutFilterIcon(&peakThreeIcon, 2, 0);
    layoutFilterIcon(&peakFourIcon, 2, 1);
    layoutFilterIcon(&peakFiveIcon, 3, 0);
    layoutFilterIcon(&peakSixIcon, 3, 1);
    layoutFilterIcon(&lpfOneIcon, 4, 0);
    layoutFilterIcon(&lpfTwoIcon, 4, 1);
    eqVisual.setBounds(0, 0, getWidth(), headerHeight);
}

// === Initialization Functions ===============================================
void PluginEditor::initializeIcons()
{
    juce::AudioProcessorValueTreeState* stateTree = &processorRef.tree;
    setupFilterIcon(&hpfOneIcon, Icon::Type::HighPass);
    hpfOneIcon.attachToFilter(stateTree, &processorRef.highPassOne);
    setupFilterIcon(&hpfTwoIcon, Icon::Type::LowShelf);
    hpfTwoIcon.attachToFilter(stateTree, &processorRef.highPassTwo);
    setupFilterIcon(&peakOneIcon, Icon::Type::Peak);
    peakOneIcon.attachToFilter(stateTree, &processorRef.peakOne);
    setupFilterIcon(&peakTwoIcon, Icon::Type::Peak);
    peakTwoIcon.attachToFilter(stateTree, &processorRef.peakTwo);
    setupFilterIcon(&peakThreeIcon, Icon::Type::Peak);
    peakThreeIcon.attachToFilter(stateTree, &processorRef.peakThree);
    setupFilterIcon(&peakFourIcon, Icon::Type::Peak);
    peakFourIcon.attachToFilter(stateTree, &processorRef.peakFour);
    setupFilterIcon(&peakFiveIcon, Icon::Type::Peak);
    peakFiveIcon.attachToFilter(stateTree, &processorRef.peakFive);
    setupFilterIcon(&peakSixIcon, Icon::Type::Peak);
    peakSixIcon.attachToFilter(stateTree, &processorRef.peakSix);
    setupFilterIcon(&lpfOneIcon, Icon::Type::HighShelf);
    lpfOneIcon.attachToFilter(stateTree, &processorRef.lowPassOne);
    setupFilterIcon(&lpfTwoIcon, Icon::Type::LowPass);
    lpfTwoIcon.attachToFilter(stateTree, &processorRef.lowPassTwo);
}

void PluginEditor::initializeFilters()
{
    juce::AudioProcessorValueTreeState* stateTree = &processorRef.tree;
    addGainControl(&gainOne);
    addGainControl(&gainTwo);
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
    gainOne.attachToGain(stateTree, &processorRef.gainOne);
    gainTwo.attachToGain(stateTree, &processorRef.gainTwo);
    highPassOne.shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        auto iconType = toggled ? Icon::Type::LowShelf : Icon::Type::HighPass;
        hpfOneIcon.setType(iconType);
    });
    highPassOne.attachToHighPass(stateTree, &processorRef.highPassOne);
    highPassTwo.shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        auto iconType = toggled ? Icon::Type::LowShelf : Icon::Type::HighPass;
        hpfTwoIcon.setType(iconType);
    });
    highPassTwo.attachToHighPass(stateTree, &processorRef.highPassTwo);
    peakOne.attachToPeakFilter(stateTree, &processorRef.peakOne);
    peakTwo.attachToPeakFilter(stateTree, &processorRef.peakTwo);
    peakThree.attachToPeakFilter(stateTree, &processorRef.peakThree);
    peakFour.attachToPeakFilter(stateTree, &processorRef.peakFour);
    peakFive.attachToPeakFilter(stateTree, &processorRef.peakFive);
    peakSix.attachToPeakFilter(stateTree, &processorRef.peakSix);
    lowPassOne.shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        auto iconType = toggled ? Icon::Type::HighShelf : Icon::Type::LowPass;
        lpfOneIcon.setType(iconType);
    });
    lowPassOne.attachToLowPass(stateTree, &processorRef.lowPassOne);
    lowPassTwo.shelfToggle.addOnToggleFunction([this] (bool toggled)
    {
        auto iconType = toggled ? Icon::Type::HighShelf : Icon::Type::LowPass;
        lpfTwoIcon.setType(iconType);
    });
    lowPassTwo.attachToLowPass(stateTree, &processorRef.lowPassTwo);
}

void PluginEditor::initializeLinkButtons()
{
    juce::AudioProcessorValueTreeState* stateTree = &processorRef.tree;
    setupLinkButton(&gainLink, &gainOne, &gainTwo);
    gainLink.attachToParameter(stateTree, "gain-linked");
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
}

void PluginEditor::initializeGlobalControls()
{
    juce::AudioProcessorValueTreeState* stateTree = &processorRef.tree;
    midSideButton.toggle.setText("M-S");
    midSideButton.toggle.setRadioGroupId(1, juce::dontSendNotification);
    midSideButton.toggle.onClick = [this]
    { 
        processorRef.tree.getParameter("mode")->setValue(0);
        setColorOverrides();
        repaint(); 
    };
    addAndMakeVisible(midSideButton.toggle);
    leftRightButton.toggle.setText("Stereo");
    leftRightButton.toggle.setRadioGroupId(1, juce::dontSendNotification);
    leftRightButton.toggle.onClick = [this]
    { 
        processorRef.tree.getParameter("mode")->setValue(1);
        setColorOverrides();
        repaint();
    };
    addAndMakeVisible(leftRightButton.toggle);
    linkAllButton.setText("Link All");
    linkAllButton.onClick = [this, stateTree]
    {
        stateTree->getParameter("gain-linked")->setValue(1);
        stateTree->getParameter("hpf-linked")->setValue(1);
        stateTree->getParameter("peak12-linked")->setValue(1);
        stateTree->getParameter("peak34-linked")->setValue(1);
        stateTree->getParameter("peak56-linked")->setValue(1);
        stateTree->getParameter("lpf-linked")->setValue(1);
        checkGlobalLinkButtonState();
        
    };
    addAndMakeVisible(linkAllButton);
    unlinkAllButton.setText("Unlink");
    unlinkAllButton.onClick = [this, stateTree]
    {
        stateTree->getParameter("gain-linked")->setValue(0);
        stateTree->getParameter("hpf-linked")->setValue(0);
        stateTree->getParameter("peak12-linked")->setValue(0);
        stateTree->getParameter("peak34-linked")->setValue(0);
        stateTree->getParameter("peak56-linked")->setValue(0);
        stateTree->getParameter("lpf-linked")->setValue(0);
        checkGlobalLinkButtonState();
    };
    addAndMakeVisible(unlinkAllButton);
    resetButton.setText("Reset");
    resetButton.setColorOverride(resetColor);
    resetButton.setDisplayAlwaysUp(true);
    resetButton.onClick = [this]
    {
        processorRef.resetAllParams();
    };
    addAndMakeVisible(resetButton);
    auto notif = juce::sendNotification;
    midSideButton.toggle.setToggleState(processorRef.isMidSide(), notif);
    leftRightButton.toggle.setToggleState(!processorRef.isMidSide(), notif);
}

void PluginEditor::initializeEqVisual()
{
    eqVisual.addToFirstResponse(&processorRef.gainOne);
    eqVisual.addToFirstResponse(&processorRef.highPassOne);
    eqVisual.addToFirstResponse(&processorRef.peakOne);
    eqVisual.addToFirstResponse(&processorRef.peakThree);
    eqVisual.addToFirstResponse(&processorRef.peakFive);
    eqVisual.addToFirstResponse(&processorRef.lowPassOne);
    eqVisual.addToSecondResponse(&processorRef.gainTwo);
    eqVisual.addToSecondResponse(&processorRef.highPassTwo);
    eqVisual.addToSecondResponse(&processorRef.peakTwo);
    eqVisual.addToSecondResponse(&processorRef.peakFour);
    eqVisual.addToSecondResponse(&processorRef.peakSix);
    eqVisual.addToSecondResponse(&processorRef.lowPassTwo);
    addAndMakeVisible(eqVisual);
}

// === Initialization Helper Functions ========================================
void PluginEditor::addParameterControl(ParameterControl* control)
{
    addAndMakeVisible(control->slider);
    addAndMakeVisible(control->label);
}

void PluginEditor::addGainControl(GainControl* gain)
{
    addParameterControl(&gain->gain);
    addAndMakeVisible(&gain->onOff.toggle);
}

void PluginEditor::addHighPassControl(HighPassControl* control)
{
    addParameterControl(&control->frequency);
    addParameterControl(&control->falloff);
    addParameterControl(&control->shelfGain);
    addParameterControl(&control->resonance);
    addAndMakeVisible(&control->onOff.toggle);
    addAndMakeVisible(&control->shelfToggle.toggle);
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
    addParameterControl(&control->shelfGain);
    addParameterControl(&control->resonance);
    addAndMakeVisible(&control->onOff.toggle);
    addAndMakeVisible(&control->shelfToggle.toggle);
}

template <linkable T>
void PluginEditor::setupLinkButton
(ParameterToggle* linkButton, T* item1, T* item2)
{
    linkButton->toggle.setText("LINK");
    linkButton->addOnToggleFunction([this, item1, item2] (bool toggled)
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
        checkGlobalLinkButtonState();
    });
    addAndMakeVisible(&linkButton->toggle);
}

void PluginEditor::setupFilterIcon(Icon* icon, Icon::Type type)
{
    int cx = cellWidth / 2;
    int cy = columnPaddingY / 2;
    int w = type == Icon::Type::Peak ? 28 : 24;
    int h = type == Icon::Type::Peak ? 22 : 16;
    icon->setType(type, cx - (w / 2), cy - (h / 2), w, h);
    addAndMakeVisible(icon);
}

// === Layout Helper Functions ================================================
void PluginEditor::layoutGlobalControl(CtmToggle* control, float yPos)
{
    int x = xStart + gainWidth + (cellWidth * maxCols)
        + (cellMarginX * (maxCols + 1)) + (cellPaddingX * (maxCols * 2))
        + globalsMarginL + globalsInset;
    int cy = headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2) - (globalsHeight / 2);
    int y = cy - (int)((globalsHeight + globalsMarginY) * yPos);
    control->setBounds(x, y, globalsWidth, globalsHeight);
}

void PluginEditor::layoutFilter
(FilterControl* lowPass, int xIndex, int yIndex)
{
    int x = xStart + gainWidth + (cellWidth * xIndex)
        + (cellMarginX * (xIndex + 1)) + (cellPaddingX * ((2 * xIndex) + 1));
    int y = headerHeight + yStart + columnPaddingY 
        + ((cellHeight + cellMarginY) * yIndex);
    lowPass->setBounds(
        x, y, cellWidth, cellHeight, intraCellPaddingX, intraCellPaddingY
    );
}

void PluginEditor::layoutGain(GainControl* gain, int yIndex)
{
    int y = headerHeight + yStart + (int)(columnPaddingY * 0.33f);
    y += yIndex * (cellHeight + cellMarginY + (int)(columnPaddingY * 0.67f));
    gain->setToggleAboveSlider(yIndex == 0);
    int h = cellHeight + (int)(columnPaddingY * 0.67f);
    gain->setBounds(xStart, y, gainWidth, h, 0, 0);
}

void PluginEditor::layoutLinkButton(CtmToggle* linkButton, int index)
{
    int cx;
    if (index == -1)
    {
        cx = xStart + (gainWidth / 2);
    }
    else
    {
        cx = xStart + gainWidth + (cellWidth * index)
            + (cellMarginX * (index + 1)) + (cellPaddingX * ((index * 2) + 1))
            + (cellWidth / 2);
    }
    int cy = headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2);
    linkButton->setBounds(cx - 18, cy - 10, 36, 21);
}

void PluginEditor::layoutFilterIcon(Icon* icon, int xIndex, int yIndex)
{
    int x = xStart + gainWidth + (cellWidth * xIndex)
        + (cellMarginX * (xIndex + 1)) + (cellPaddingX * ((xIndex * 2) + 1));
    int y = headerHeight + yStart;
    if (yIndex > 0)
        y += (cellHeight * maxRows) + (cellMarginY * (maxRows - 1))
            + columnPaddingY;
    icon->setBounds(x, y, cellWidth, columnPaddingY);
}

// === Drawing Helper Functions ===============================================
void PluginEditor::drawGlobalControlGrouping
(juce::Graphics& g, float start, float end)
{
    int x = xStart + gainWidth + (cellWidth * maxCols)
        + (cellMarginX * (maxCols + 1)) + (cellPaddingX * (maxCols * 2))
        + globalsMarginL;
    int cy = headerHeight + yStart + columnPaddingY + cellHeight
        + (cellMarginY / 2) - (globalsHeight / 2);
    int y1 = cy - (int)((globalsHeight + globalsMarginY) * start);
    int y2 = cy - (int)((globalsHeight + globalsMarginY) * end);
    int y = y1 < y2 ? y1 : y2;
    int w = globalsWidth + (globalsInset * 2);
    int h = y1 < y2 ? y2 - y1 : y1 - y2;
    h += globalsHeight + (globalsInset * 2);
    g.setColour(findColour(CtmColourIds::darkBgColourId));
    g.fillRoundedRectangle(x, y - globalsInset, w, h, globalsInsetCorner);
}

void PluginEditor::drawSectionLabels(juce::Graphics& g)
{
    // define the position and area of the labels
    bool ms = processorRef.isMidSide();
    int x1 = ms ? 30 : 31;
    int y1 = headerHeight + 20;
    int x2 = ms ? 33 : 40;
    int y2 = getHeight();
    // fill the labels with background color
    g.setColour(findColour(CtmColourIds::brightBgColourId));
    g.fillRect(0, y1 - 20, x1, 20);
    juce::Path triangle1;
    triangle1.addTriangle(x1, y1 - 20, x1 + 4, y1 - 20, x1, y1);
    g.fillPath(triangle1);
    g.fillRect(0, y2 - 20, x2, 20);
    juce::Path triangle2;
    triangle2.addTriangle(x2, y2, x2 + 4, y2, x2, y2 - 20);
    g.fillPath(triangle2);
    // draw the outlines around the labels
    g.setColour(findColour(CtmColourIds::brightOutlineColourId));
    g.drawLine(0, y1, x1, y1);
    g.drawLine(x1, y1, x1 + 4, y1 - 20);
    g.drawLine(0, y2 - 20, x2, y2 - 20);
    g.drawLine(x2, y2 - 20, x2 + 4, y2);
    // draw text of the labels
    g.setFont(16);
    g.setColour(juce::Colours::white.interpolatedWith(getColorOne(), 0.7f));
    g.drawText(ms ? "Mid" : "Left", 4, headerHeight + 1, 40, 18, false);
    g.setColour(juce::Colours::white.interpolatedWith(getColorTwo(), 0.7f));
    g.drawText(ms ? "Side" : "Right", 4, getHeight() - 20, 40, 18, false);
    // draw the line between the sections
    int globalX = xStart + gainWidth + (cellWidth * maxCols)
        + (cellMarginX * (maxCols + 1)) + (cellPaddingX * (maxCols * 2))
        + (globalsMarginL / 2) - 4;
    g.setColour(findColour(CtmColourIds::brightOutlineColourId));
    g.drawLine(0, secondSectionStart(), globalX + 40, secondSectionStart());
    // draw the line seperating the global controls section
    g.drawLine(globalX, headerHeight, globalX, getHeight());
}

void PluginEditor::drawFilterBackground(juce::Graphics& g, int index)
{
    juce::Path p;
    float x = xStart + gainWidth + ((cellWidth + (cellPaddingX*2)) * index)
        + (cellMarginX * (index + 1));
    float y = headerHeight + yStart;
    float w = cellWidth + (cellPaddingX * 2);
    float h = (cellHeight * maxRows) + (cellMarginY * (maxRows - 1))
        + (columnPaddingY * 2);
    p.addRoundedRectangle(x, y, w, h, columnBgCurvature);
    g.setColour(findColour(CtmColourIds::darkBgColourId));
    g.fillPath(p);
}

void PluginEditor::drawGainBackground(juce::Graphics& g)
{
    juce::Path p;
    float y = headerHeight + yStart;
    float h = (cellHeight * maxRows) + (cellMarginY * (maxRows - 1))
        + (columnPaddingY * 2);
    p.addRoundedRectangle(xStart, y, gainWidth, h, columnBgCurvature);
    g.setColour(findColour(CtmColourIds::darkBgColourId));
    g.fillPath(p);
}

// === Other Helper Functions =================================================
void PluginEditor::setColorOverrides()
{
    juce::Colour c1 = getColorOne();
    c1 = c1.withMultipliedBrightness(0.8f).withMultipliedSaturation(0.8f);
    juce::Colour c2 = getColorTwo();
    c2 = c2.withMultipliedBrightness(0.8f).withMultipliedSaturation(0.8f);
    midSideButton.toggle.setColorOverride(c1);
    leftRightButton.toggle.setColorOverride(c1);
    linkAllButton.setColorGradient(getColorOne(), getColorTwo());
    unlinkAllButton.setColorGradient(getColorOne(), getColorTwo());
    gainOne.setAllColorOverrides(getColorOne());
    gainTwo.setAllColorOverrides(getColorTwo());
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
    gainLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    highPassLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakOneTwoLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakThreeFourLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    peakFiveSixLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    lowPassLink.toggle.setColorGradient(getColorOne(), getColorTwo());
    hpfOneIcon.setColor(getColorOne());
    hpfTwoIcon.setColor(getColorTwo());
    peakOneIcon.setColor(getColorOne());
    peakTwoIcon.setColor(getColorTwo());
    peakThreeIcon.setColor(getColorOne());
    peakFourIcon.setColor(getColorTwo());
    peakFiveIcon.setColor(getColorOne());
    peakSixIcon.setColor(getColorTwo());
    lpfOneIcon.setColor(getColorOne());
    lpfTwoIcon.setColor(getColorTwo());
    eqVisual.setFrequencyResponseColors(getColorOne(), getColorTwo());
}

void PluginEditor::checkGlobalLinkButtonState()
{
    juce::AudioProcessorValueTreeState* tree = &processorRef.tree;
    bool linked = *tree->getRawParameterValue("hpf-linked") >= 1;
    bool unlinked = *tree->getRawParameterValue("hpf-linked") <= 0;
    linked = linked && *tree->getRawParameterValue("peak12-linked") >= 1;
    unlinked = unlinked && *tree->getRawParameterValue("peak12-linked") <= 0;
    linked = linked && *tree->getRawParameterValue("peak34-linked") >= 1;
    unlinked = unlinked && *tree->getRawParameterValue("peak34-linked") <= 0;
    linked = linked && *tree->getRawParameterValue("peak56-linked") >= 1;
    unlinked = unlinked && *tree->getRawParameterValue("peak56-linked") <= 0;
    linked = linked && *tree->getRawParameterValue("lpf-linked") >= 1;
    unlinked = unlinked && *tree->getRawParameterValue("lpf-linked") <= 0;
    linkAllButton.setToggleState(linked, juce::dontSendNotification);
    unlinkAllButton.setToggleState(unlinked, juce::dontSendNotification);
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