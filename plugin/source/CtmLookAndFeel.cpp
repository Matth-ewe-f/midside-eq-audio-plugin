#include "CtmLookAndFeel.h"

CtmLookAndFeel::CtmLookAndFeel()
{
    // override default colors
    setColour(juce::ResizableWindow::backgroundColourId, normalBgColor);
    juce::Colour trans = juce::Colour::fromRGBA(255, 255, 255, 0);
    setColour(juce::TextEditor::backgroundColourId, trans);
    setColour(juce::TextEditor::outlineColourId, trans);
    setColour(juce::TextEditor::focusedOutlineColourId, trans);
    setColour(juce::Slider::backgroundColourId, rotaryColor);
    // add custom colors
    setColour(CtmColourIds::normalBgColourId, normalBgColor);
    setColour(CtmColourIds::darkBgColourId, darkBgColor);
    setColour(CtmColourIds::brightBgColourId, brightBgColor);
    setColour(CtmColourIds::darkOutlineColourId, darkOutlineColor);
    setColour(CtmColourIds::brightOutlineColourId, brightOutlineColor);
    setColour(CtmColourIds::meterFillColourId, meterFillColor);
    setColour(CtmColourIds::toggledColourId, toggledColor);
    setColour(CtmColourIds::untoggledColourId, untoggledColor);
}

void CtmLookAndFeel::drawRotarySlider
(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
const float startAngle, const float endAngle, juce::Slider& slider)
{
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
    g.setColour(slider.findColour(juce::Slider::backgroundColourId));
    g.fillEllipse(innerRx, innerRy, innerR * 2, innerR * 2);
    // draw the back of the fill meter
    juce::Path meter;
    meter.addArc(rx, ry, r * 2, r * 2, startAngle, endAngle, true);
    juce::PathStrokeType pathStroke(rotaryOutlineWidth);
    g.setColour(slider.findColour(CtmColourIds::darkOutlineColourId));
    g.strokePath(meter, pathStroke);
    // draw the front of the fill meter
    juce::Path meterFill;
    float fillAngle = startAngle + ((endAngle - startAngle) * sliderPos);
    meterFill.addArc(rx, ry, r * 2, r * 2, startAngle, fillAngle, true);
    g.setColour(slider.findColour(CtmColourIds::meterFillColourId));
    g.strokePath(meterFill, pathStroke);
    // draw a tick at the point to which the fill meter is filled
    juce::Line<float> line(x + (width / 2), y, x + (width / 2), y + 8);
    juce::Path tick;
    tick.addLineSegment(line, 3);
    tick.applyTransform(juce::AffineTransform::rotation(fillAngle, cx, cy));
    g.setColour(slider.findColour(CtmColourIds::brightOutlineColourId));
    g.fillPath(tick);
}