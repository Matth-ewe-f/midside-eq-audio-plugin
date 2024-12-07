#include "EqVisual.h"
#include "CtmLookAndFeel.h"

// === Lifecycle ==============================================================
EqVisual::~EqVisual()
{
    for (auto kvp : responseOne)
    {
        kvp.first->removeStateListener(this);
    }
    for (auto kvp : responseTwo)
    {
        kvp.first->removeStateListener(this);
    }
}

// === Graphics ===============================================================
void EqVisual::paint(juce::Graphics& g)
{
    g.setOpacity(1);
    g.drawImageAt(bgImage, 0, 0);
    // draw the frequency response
    drawFreqResponse(g, responseTwo, freqResponseColorTwo);
    drawFreqResponse(g, responseOne, freqResponseColorOne);
}

void EqVisual::resized()
{
    // reset the background image
    bgImage = juce::Image(juce::Image::RGB, getWidth(), getHeight(), false);
    juce::Graphics g(bgImage);
    drawBackground(g);
    // reset the array of frequencies
    int width = getWidth() - xStart - xEnd + (2 * freqResponseExtension);
    numDisplayFreqs = static_cast<size_t>(width);
    displayFreqs.resize(numDisplayFreqs);
    for (int i = 0;i < width;i++)
    {
        int x = xStart - freqResponseExtension + i;
        displayFreqs[(size_t) i] = getFrequencyForX(x);
    }
    // reset the frequency responses of all the filters
    updateResponseOnResize(responseOne);
    updateResponseOnResize(responseTwo);
}

// === Filter State Listener ==================================================
void EqVisual::addToFirstResponse(CtmFilter* filter)
{
    filter->addStateListener(this);
    updateOrAddFilterResponse(filter, responseOne);
}

void EqVisual::addToSecondResponse(CtmFilter* filter)
{
    filter->addStateListener(this);
    updateOrAddFilterResponse(filter, responseTwo);
}

void EqVisual::notify(CtmFilter* filter)
{
    if (responseOne.contains(filter))
        updateOrAddFilterResponse(filter, responseOne);
    else if (responseTwo.contains(filter))
        updateOrAddFilterResponse(filter, responseTwo);
    repaint();
}

// === Parameters =============================================================
void EqVisual::setFrequencyResponseColors(juce::Colour c1, juce::Colour c2)
{
    freqResponseColorOne = c1;
    freqResponseColorTwo = c2;
}

// === Drawing Helper Functions ===============================================
void EqVisual::drawBackground(juce::Graphics& g)
{
    // draw background color
    g.setColour(bgColor);
    g.fillRect(0, 0, getWidth(), getHeight());
    // draw horizontal gridlines and labels
    for (int i = 0;i < numHorzLines;i++)
    {
        int gainValue = ((numHorzLines / 2) - i) * 6;
        int y = (int)getYForGain(gainValue);
        if (i == numHorzLines / 2)
            drawCentralHorzLine(g, y);
        else
            drawHorzLine(g, y);
        drawGainLabel(g, y, gainValue);
    }
    // draw vertical gridlines
    int f = 20;
    while (f <= 20000)
    {
        int x = (int)getXForFrequency(f);
        if (f == 20 || f == 100 || f == 1000 || f == 20000)
            drawMainVertLine(g, x);
        else
            drawVertLine(g, x);
        if (shouldDrawFreqLabel(f))
            drawFreqLabel(g, x, f);
        if (f < 100)
            f += 10;
        else if (f < 1000)
            f += 100;
        else if (f < 10000)
            f += 1000;
        else
            f += 10000;
    }
    // draw unit text for axes (dB and Hz)
    juce::Justification justify = juce::Justification::left;
    int cy = (getHeight() / 2);
    g.setColour(getColorForGainLabels());
    g.setFont(14);
    int x1 = xStart - horzLineExtension - majorHorzLineExtraExtension - 40;
    g.drawText("dB", x1, cy - 8, 20, 14, justify);
    g.setColour(getColorForFreqLabels());
    g.setFont(12);
    int x2 = getWidth() - xEnd + horzLineExtension
        + majorHorzLineExtraExtension + 8;
    g.drawText("Hz", x2, cy, 20, 12, justify);
}

void EqVisual::drawHorzLine(juce::Graphics& g, int y)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.2f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 16);
    int gw = horzLineExtension;
    juce::ColourGradient gradient = juce::ColourGradient::horizontal(
        trans, xStart - gw, trans, getWidth() - xEnd + gw
    );
    int length = getWidth() - xStart - xEnd + (2 * gw);
    gradient.addColour((double)gw / length, clr);
    gradient.addColour((double)(length - gw) / length, clr);
    g.setGradientFill(gradient);
    g.drawLine(xStart - gw, y, getWidth() - xEnd + gw, y);
}

void EqVisual::drawCentralHorzLine(juce::Graphics& g, int y)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.7f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 32);
    int extension = horzLineExtension + majorHorzLineExtraExtension;
    juce::ColourGradient gradient = juce::ColourGradient::horizontal(
        trans, xStart - extension, trans, getWidth() - xEnd + extension
    );
    int gw = horzLineExtension / 2;
    int length = getWidth() - xStart - xEnd + (2 * extension);
    gradient.addColour((double)gw / length, clr);
    gradient.addColour((double)(length - gw) / length, clr);
    g.setGradientFill(gradient);
    g.drawLine(xStart - extension, y, getWidth() - xEnd + extension, y);
}

void EqVisual::drawVertLine(juce::Graphics& g, int x)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.2f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 16);
    int gh = vertLineExtension;
    juce::ColourGradient gradient = juce::ColourGradient::vertical(
        trans, paddingY - gh, trans, getHeight() - paddingY + gh
    );
    int length = getHeight() - (2 * paddingY) + (2 * gh);
    gradient.addColour((double)gh / length, clr);
    gradient.addColour((double)(length - gh) / length, clr);
    g.setGradientFill(gradient);
    g.drawLine(x, paddingY - gh, x, getHeight() - paddingY + gh);
}

void EqVisual::drawMainVertLine(juce::Graphics& g, int x)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.5f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 32);
    int extension = vertLineExtension + majorVertLineExtraExtension;
    juce::ColourGradient gradient = juce::ColourGradient::vertical(
        trans, paddingY - extension, trans, getHeight() - paddingY + extension
    );
    int gh = vertLineExtension / 2;
    int length = getHeight() - (2 * paddingY) + (2 * extension);
    gradient.addColour((double)gh / length, clr);
    gradient.addColour((double)(length - gh) / length, clr);
    g.setGradientFill(gradient);
    g.drawLine(x, paddingY - extension, x, getHeight() - paddingY + extension);
}

void EqVisual::drawGainLabel(juce::Graphics& g, int cy, int gain)
{
    std::string gainString = std::to_string(gain);
    if (gain > 0)
        gainString = "+" + gainString;
    int x = xStart - horzLineExtension - majorHorzLineExtraExtension - 32;
    g.setColour(getColorForGainLabels());
    g.setFont(11);
    g.drawText(gainString, x, cy - 7, 24, 12, juce::Justification::right);
}

void EqVisual::drawFreqLabel(juce::Graphics& g, int cx, int freq)
{
    std::string freqString;
    if (freq < 1000)
        freqString = std::to_string(freq);
    else
        freqString = std::to_string(freq / 1000) + "k";
    int y = (getHeight() / 2) + 1;
    juce::Colour backingColor = bgColor.withAlpha(0.5f);
    g.setColour(backingColor);
    g.fillRect(cx - 6, y, 12, 10);
    juce::ColourGradient grad = juce::ColourGradient::vertical(
        backingColor, y + 10, backingColor.withAlpha(0.0f), y + 12
    );
    g.setGradientFill(grad);
    g.fillRect(cx - 6, y + 10, 12, 2);
    g.setColour(getColorForFreqLabels());
    g.setFont(10);
    g.drawText(freqString, cx - 10, y, 20, 12, juce::Justification::centred);
}

void EqVisual::drawFreqResponse
(juce::Graphics& g, std::map<CtmFilter*, std::vector<double>>& response,
juce::Colour color)
{
    // create the total frequency response of all filters in the map
    if (response.size() == 0)
        return;
    double* totals = new double[numDisplayFreqs];
    for (size_t i = 0;i < numDisplayFreqs;i++)
    {
        totals[i] = 1;
    }
    for (auto kvp : response)
    {
        double* filterResponse = kvp.second.data();
        for (size_t i = 0;i < numDisplayFreqs;i++)
        {
            totals[i] *= filterResponse[i];
        }
    }
    // create the frequency response path, point by point
    juce::Path p;
    for (size_t i = 0;i < numDisplayFreqs;i += 2)
    {
        float x = getXForFrequency((float)displayFreqs[i]);
        float y = getYForGain(20 * log10((float)totals[i]));
        if (i == 0)
            p.startNewSubPath(x, y);
        else
            p.lineTo(x, y);
    }
    delete[] totals;
    // draw the path itself
    int start = xStart - freqResponseExtension;
    int end = getWidth() - xEnd + freqResponseExtension;
    juce::Colour trans = color.withAlpha(0.0f);
    juce::ColourGradient lineGradient = juce::ColourGradient::horizontal(
        trans, start, trans, end
    );
    double gradProp1 = (double)(xStart - start) / (end - start);
    double gradProp2 = (double)(getWidth() - start - xEnd) / (end - start);
    lineGradient.addColour(gradProp1, color.withAlpha(0.7f));
    lineGradient.addColour(gradProp2, color.withAlpha(0.7f));
    g.setGradientFill(lineGradient);
    g.strokePath(p, juce::PathStrokeType(2));
    // fill the path
    juce::ColourGradient fillGradient = juce::ColourGradient::horizontal(
        trans, start, trans, end
    );
    fillGradient.addColour(gradProp1, color.withAlpha(0.3f));
    fillGradient.addColour(gradProp2, color.withAlpha(0.3f));
    p.lineTo(end, getHeight() / 2);
    p.lineTo(start, getHeight() / 2);
    p.closeSubPath();
    g.setGradientFill(fillGradient);
    g.fillPath(p);
}

// === Other Helper Functions =================================================
void EqVisual::updateOrAddFilterResponse
(CtmFilter* filter, std::map<CtmFilter*, std::vector<double>>& response)
{
    if (!response.contains(filter))
    {
        response[filter] = std::vector<double>(numDisplayFreqs);
    }
    filter->getMagnitudes(
        displayFreqs.data(), response[filter].data(), numDisplayFreqs
    );
}

void EqVisual::updateResponseOnResize
(std::map<CtmFilter*, std::vector<double>>& response)
{
    for (auto it = response.begin();it != response.end();it++)
    {
        it->second.clear();
        it->second.resize(numDisplayFreqs);
        it->first->getMagnitudes(
            displayFreqs.data(), it->second.data(), numDisplayFreqs
        );
    }
}

float EqVisual::getYForGain(float gain)
{
    int cy = getHeight() / 2;
    float maxGain = (numHorzLines / 2) * 6;
    return cy - ((gain / maxGain) * ((getHeight() - (2 * paddingY)) / 2));
}

float EqVisual::getXForFrequency(float freq)
{
    // normalize freq from 20-20000 on a log scale to 0-1 on a linear scale
    float p = log2(freq / 20) / log2(20000.0f / 20.0f);
    return xStart + (p * (getWidth() - xStart - xEnd));
}

float EqVisual::getFrequencyForX(int x)
{
    float p = ((float)x - xStart) / ((float)getWidth() - xEnd - xStart);
    // map freq from 0-1 on a linear scale to 20-20000 on a log scale
    return 20 * pow(2.0f, p * log2(20000.0f / 20.0f));
}

bool EqVisual::shouldDrawFreqLabel(int freq)
{
    if (freq < 100)
    {
        int d = freq / 10;
        return d != 7 && d != 9;
    }
    else if (freq < 1000)
    {
        int d = freq / 100;
        return d != 5 && d != 7 && d != 9;
    }
    else if (freq < 10000)
    {
        int d = freq / 1000;
        return d != 7 && d != 9;
    }
    return true;
}

juce::Colour EqVisual::getColorForGainLabels()
{
    return findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.8f);
}

juce::Colour EqVisual::getColorForFreqLabels()
{
    return findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.8f);
}