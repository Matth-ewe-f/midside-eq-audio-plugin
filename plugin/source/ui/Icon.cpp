#include "Icon.h"
#include "CtmLookAndFeel.h"

// === Lifecycle ==============================================================
Icon::Icon() : color(juce::Colours::white), enabled(true)
{
    lastRect = juce::Rectangle<int>(0, 0, 0, 0);
}

Icon::~Icon()
{
    tree->removeParameterListener(parameterName, this);
}

// === Graphics ===============================================================
void Icon::setType(Type type)
{
    int x = lastRect.getX();
    int y = lastRect.getY();
    int w = lastRect.getWidth();
    int h = lastRect.getHeight();
    setType(type, x, y, w, h);
}

void Icon::setType(Type type, int x, int y, int w, int h)
{
    if (type == Type::LowPass)
    {
        path = getLowPassFilterIcon(x, y, w, h);
    }
    else if (type == Type::HighShelf)
    {
        path = getHighShelfIcon(x, y, w, h);
    }
    else if (type == Type::Peak)
    {
        path = getPeakFilterIcon(x, y, w, h);
    }
    else if (type == Type::HighPass)
    {
        path = getHighPassFilterIcon(x, y, w, h);
    }
    else if (type == Type::LowShelf)
    {
        path = getLowShelfIcon(x, y, w, h);
    }
    lastRect = juce::Rectangle<int>(x, y, w, h);
    repaint();
}

void Icon::setColor(juce::Colour c)
{
    color = c;
    repaint();
}

void Icon::setEnabled(bool isEnabled)
{
    enabled = isEnabled;
    repaint();
}

void Icon::paint(juce::Graphics& g)
{
    int x = getLocalBounds().getX();
    int y = getLocalBounds().getY();
    path.applyTransform(juce::AffineTransform::translation(x, y));
    g.setColour(enabled ? color : findColour(CtmColourIds::untoggledColourId));
    g.fillPath(path);
    path.applyTransform(juce::AffineTransform::translation(-x, -y));
}

// === Listen to Parameters ===================================================
void Icon::attachToFilter
(juce::AudioProcessorValueTreeState* stateTree, CtmFilter* filter)
{
    parameterName = filter->getOnOffParameter();
    tree = stateTree;
    stateTree->addParameterListener(parameterName, this);
    setEnabled(*stateTree->getRawParameterValue(parameterName) >= 1);
}

void Icon::parameterChanged(const juce::String& parameter, float value)
{
    juce::ignoreUnused(parameter);
    setEnabled(value >= 1);
}

// === Factory Functions for Paths ============================================
juce::Path Icon::getHighPassFilterIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[2];
    lines[0] = juce::Line<float>(0, 1, 0.33f, 0.17f);
    lines[1] = juce::Line<float>(0.33f, 0.17f, 1, 0.17f);
    for (int i = 0;i < 2;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1.5f);
    }
    return path;
}

juce::Path Icon::getLowPassFilterIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[2];
    lines[0] = juce::Line<float>(0, 0.17f, 0.67f, 0.17f);
    lines[1] = juce::Line<float>(0.67f, 0.17f, 1, 1);
    for (int i = 0;i < 2;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1.5f);
    }
    return path;
}

juce::Path Icon::getPeakFilterIcon(int x, int y, int w, int h)
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
        path.addLineSegment(lines[i], 1.5f);
    }
    return path;
}

juce::Path Icon::getLowShelfIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[5];
    lines[0] = juce::Line<float>(0.1f, 0.87f, 0.45f, 0.87f);
    lines[1] = juce::Line<float>(0.1f, 0.13f, 0.45f, 0.13f);
    lines[2] = juce::Line<float>(0.45f, 0.87f, 0.7f, 0.5f);
    lines[3] = juce::Line<float>(0.45f, 0.13f, 0.7f, 0.5f);
    lines[4] = juce::Line<float>(0.7f, 0.5f, 0.9f, 0.5f);
    for (int i = 0;i < 5;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1.5f);
    }
    return path;
}

juce::Path Icon::getHighShelfIcon(int x, int y, int w, int h)
{
    juce::Path path;
    juce::Line<float> lines[5];
    lines[0] = juce::Line<float>(0.1f, 0.5f, 0.3f, 0.5f);
    lines[1] = juce::Line<float>(0.3f, 0.5f, 0.55f, 0.87f);
    lines[2] = juce::Line<float>(0.3f, 0.5f, 0.55f, 0.13f);
    lines[3] = juce::Line<float>(0.55f, 0.87f, 0.9f, 0.87f);
    lines[4] = juce::Line<float>(0.55f, 0.13f, 0.9f, 0.13f);
    for (int i = 0;i < 5;i++)
    {
        lines[i].applyTransform(juce::AffineTransform::scale(w, h));
        lines[i].applyTransform(juce::AffineTransform::translation(x, y));
        path.addLineSegment(lines[i], 1.5f);
    }
    return path;
}