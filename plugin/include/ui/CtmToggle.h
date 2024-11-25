#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class CtmToggle : public juce::ToggleButton
{
public:
    // === Lifecycle ==========================================================
    CtmToggle();

    // === Setters ============================================================
    void setText(std::string s);
    void setText(std::string toggleText, std::string untoggleText);
    void setFixedFontSize(float size);
    void setColorOverride(juce::Colour color);

    // === Graphics ===========================================================
    void paintButton(juce::Graphics& g, bool hover, bool click) override;
private:
    juce::Colour fillColor;
    std::string toggledText;
    std::string untoggledText;
    float fontSize;

    // === Private Helper =====================================================
    juce::Colour getHighlightColor();
    juce::Colour getShadowColor();
};