#include "EqVisual.h"
#include "CtmLookAndFeel.h"

juce::NormalisableRange<float> EqVisual::freqRange = EqVisual::getFreqRange();

// === Graphics ===============================================================
void EqVisual::paint(juce::Graphics& g)
{
    // draw background
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
    g.drawText("dB", xStart - 40, cy - 8, 20, 14, justify);
    g.setColour(getColorForFreqLabels());
    g.setFont(12);
    g.drawText("Hz", getWidth() - xEnd + 8, cy, 20, 12, justify);
    // draw the frequency response
    drawFreqResponse(g);
}

// === Filter State Listener ==================================================
void EqVisual::listenTo(CtmFilter* filter)
{
    filter->addStateListener(this);
    filters.push_back(filter);
}

void EqVisual::notify(CtmFilter*)
{
    repaint();
}

// === Drawing Helper Functions ===============================================
void EqVisual::drawHorzLine(juce::Graphics& g, int y)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.2f);
    int start = xStart + minorLinesExtraPaddingX;
    int end = getWidth() - xEnd - minorLinesExtraPaddingX;
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 16);
    int gw = horzLineGradWidth;
    juce::ColourGradient grad1 = juce::ColourGradient::horizontal(
        trans, start, clr, start + gw
    );
    juce::ColourGradient grad2 = juce::ColourGradient::horizontal(
        clr, end - gw, trans, end
    );
    g.setGradientFill(grad1);
    g.drawLine(start, y, start + gw, y);
    g.setColour(clr);
    g.drawLine(start + gw, y, end - gw, y);
    g.setGradientFill(grad2);
    g.drawLine(end - gw, y, end, y);
}

void EqVisual::drawCentralHorzLine(juce::Graphics& g, int y)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.7f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 64);
    int gw = horzLineGradWidth / 2;
    juce::ColourGradient grad1 = juce::ColourGradient::horizontal(
        trans, xStart, clr, xStart + gw
    );
    juce::ColourGradient grad2 = juce::ColourGradient::horizontal(
        clr, getWidth() - xEnd - gw, trans, getWidth() - xEnd
    );
    g.setGradientFill(grad1);
    g.drawLine(xStart, y, xStart + gw, y);
    g.setColour(clr);
    g.drawLine(xStart + gw, y, getWidth() - xEnd - gw, y);
    g.setGradientFill(grad2);
    g.drawLine(getWidth() - xEnd - gw, y, getWidth() - xEnd, y);
}

void EqVisual::drawVertLine(juce::Graphics& g, int x)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.2f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 16);
    int bound = paddingY + minorLinesExtraPaddingY;
    int gh = vertLineGradHeight;
    juce::ColourGradient grad1 = juce::ColourGradient::vertical(
        trans, bound, clr, bound + gh
    );
    juce::ColourGradient grad2 = juce::ColourGradient::vertical(
        clr, getHeight() - bound - gh, trans, getHeight() - bound
    );
    g.setGradientFill(grad1);
    g.drawLine(x, bound, x, bound + gh);
    g.setColour(clr);
    g.drawLine(x, bound + gh, x, getHeight() - bound - gh);
    g.setGradientFill(grad2);
    g.drawLine(x, getHeight() - bound - gh, x, getHeight() - bound);
}

void EqVisual::drawMainVertLine(juce::Graphics& g, int x)
{
    auto clr = findColour(CtmColourIds::brightOutlineColourId).withAlpha(0.5f);
    auto trans = juce::Colour::fromRGBA(255, 255, 255, 32);
    int gh = vertLineGradHeight / 2;
    juce::ColourGradient grad1 = juce::ColourGradient::vertical(
        trans, paddingY, clr, paddingY + gh
    );
    juce::ColourGradient grad2 = juce::ColourGradient::vertical(
        clr, getHeight() - paddingY - gh, trans, getHeight() - paddingY
    );
    g.setGradientFill(grad1);
    g.drawLine(x, paddingY, x, paddingY + gh);
    g.setColour(clr);
    g.drawLine(x, paddingY + gh, x, getHeight() - paddingY - gh);
    g.setGradientFill(grad2);
    g.drawLine(x, getHeight() - paddingY - gh, x, getHeight() - paddingY);
}

void EqVisual::drawGainLabel(juce::Graphics& g, int cy, int gain)
{
    std::string gainString = std::to_string(gain);
    if (gain > 0)
        gainString = "+" + gainString;
    int x = xStart - 28;
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

void EqVisual::drawFreqResponse(juce::Graphics& g)
{
    // get the frequency repsonse from the filters
    if (filters.size() == 0)
        return;
    int start = xStart + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    int end = xEnd + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    const size_t width = (size_t)(getWidth() - start - end);
    double* freqs = new double[width];
    double* totals = new double[width];
    for (size_t i = 0;i < width;i++)
    {
        freqs[i] = getFrequencyForX(start + (int)i);
        totals[i] = 1;
    }
    double* magnitudes = new double[width];
    for (CtmFilter* filter : filters)
    {
        filter->getMagnitudes(freqs, magnitudes, width);
        for (size_t i = 0;i < width;i++)
        {
            totals[i] *= magnitudes[i];
        }
    }
    // draw the frequency response, point by point
    juce::Path p;
    float lastX = 0;
    float lastY = 0;
    for (size_t i = 0;i < width;i++)
    {
        float x = getXForFrequency((float)freqs[i]);
        float y = getYForGain(20 * log10((float)totals[i]));
        if (i != 0)
            p.addLineSegment(juce::Line<float>(lastX, lastY, x, y), 1.5f);
        lastX = x;
        lastY = y;
    }
    g.setColour(juce::Colours::blue);
    g.fillPath(p);
    // memory management
    delete[] freqs;
    delete[] totals;
    delete[] magnitudes;
}

// === Other Helper Functions =================================================
float EqVisual::getYForGain(float gain)
{
    int cy = getHeight() / 2;
    int maxHeight = cy - (paddingY + (vertLineGradHeight - 4));
    float maxGain = (numHorzLines / 2) * 6;
    return cy - ((gain / maxGain) * maxHeight);
}

float EqVisual::getXForFrequency(float freq)
{
    float p = freqRange.convertTo0to1(freq);
    int start = xStart + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    int end = xEnd + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    return start + (p * (getWidth() - start - end));
}

float EqVisual::getFrequencyForX(int x)
{
    int start = xStart + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    int end = xEnd + minorLinesExtraPaddingX + (horzLineGradWidth - 12);
    float p = ((float)x - start) / ((float)getWidth() - end - start);
    return freqRange.convertFrom0to1(p);
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

juce::NormalisableRange<float> EqVisual::getFreqRange()
{
    return juce::NormalisableRange<float>(
        20,
        20000,
        [](float min, float max, float normalized) {
            return min * pow(2, normalized * log2(max / min));
        },
        [](float min, float max, float value) {
            return log2(value / min) / log2(max / min);
        }
    );
}