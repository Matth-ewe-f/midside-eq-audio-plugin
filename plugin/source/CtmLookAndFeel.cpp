#include "CtmLookAndFeel.h"

CtmLookAndFeel::CtmLookAndFeel()
{
    // override default colors
    setColour(juce::ResizableWindow::backgroundColourId, darkBgColor);
    setColour(juce::TextEditor::backgroundColourId, darkBgColor);
    juce::Colour trans = juce::Colour::fromRGBA(255, 255, 255, 0);
    setColour(juce::TextEditor::ColourIds::outlineColourId, trans);
    setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, trans);
    // add custom colors
    setColour(CtmColourIds::darkBgColourId, darkBgColor);
    setColour(CtmColourIds::brightBgColourId, brightBgColor);
    setColour(CtmColourIds::darkOutlineColourId, darkOutlineColor);
    setColour(CtmColourIds::brightOutlineColourId, brightOutlineColor);
}

void CtmLookAndFeel::drawRotarySlider
(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
const float startAngle, const float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    // position information for drawing circles
    float cx = x + (width / 2.0f);
    float cy = y + (height / 2.0f);
    float r = (juce::jmin(width, height) / 2.0f) - (rotaryOutlineWidth / 2);
    float rx = cx - r;
    float ry = cy - r;
    float innerR = r - (rotaryOutlineWidth / 2);
    float innerRx = cx - innerR;
    float innerRy = cy - innerR;
    // draw the body of the rotary
    g.setColour(rotaryColor);
    g.fillEllipse(innerRx, innerRy, innerR * 2, innerR * 2);
    // draw the back of the fill meter
    juce::Path meter;
    meter.addArc(rx, ry, r * 2, r * 2, startAngle, endAngle, true);
    juce::PathStrokeType pathStroke(rotaryOutlineWidth);
    g.setColour(darkOutlineColor);
    g.strokePath(meter, pathStroke);
    // draw the front of the fill meter
    juce::Path meterFill;
    float fillAngle = startAngle + ((endAngle - startAngle) * sliderPos);
    meterFill.addArc(rx, ry, r * 2, r * 2, startAngle, fillAngle, true);
    g.setColour(meterFillColor);
    g.strokePath(meterFill, pathStroke);
    // draw a tick at the point to which the fill meter is filled
    juce::Line<float> line(x + (width / 2), y, x + (width / 2), y + 8);
    juce::Path tick;
    tick.addLineSegment(line, 3);
    tick.applyTransform(juce::AffineTransform::rotation(fillAngle, cx, cy));
    g.setColour(brightOutlineColor);
    g.fillPath(tick);
}