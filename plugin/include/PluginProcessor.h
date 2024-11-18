#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
namespace dsp = juce::dsp;

class PluginProcessor final : public juce::AudioProcessor
{
public:
    // === Plugin Parameters ==================================================
    juce::AudioParameterFloat* midFreq;
    juce::AudioParameterFloat* sideFreq;

    // === Lifecycle ==========================================================
    PluginProcessor();
    ~PluginProcessor() override;

    // === Plugin Information =================================================
    inline const juce::String getName() const override
        { return "Mid-Side EQ"; }

    inline bool hasEditor() const override { return true; }
    inline double getTailLengthSeconds() const override { return 0.0; }
    
    inline int getNumPrograms() override { return 1; } // should always be >= 1
    inline int getCurrentProgram() override { return 0; }
    inline void setCurrentProgram (int index) override
        { juce::ignoreUnused(index); }
    inline const juce::String getProgramName (int index) override
        { juce::ignoreUnused(index); return {}; }
    inline void changeProgramName (int i, const juce::String& name) override
        { juce::ignoreUnused(i, name); }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // === Midi Information ===================================================
    inline bool acceptsMidi() const override
#if JucePlugin_WantsMidiInput
        { return true; }
#else
        { return false; }
#endif

    inline bool producesMidi() const override
#if JucePlugin_ProducesMidiOutput
        { return true; }
#else
        { return false; }
#endif

    inline bool isMidiEffect() const override
#if JucePlugin_IsMidiEffect
        { return true; }
#else
        { return false; }
#endif

    // === Processing Audio ===================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // === State ==============================================================
    void updateFilterState();
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // === Factory Functions ==================================================
    juce::AudioProcessorEditor* createEditor() override;

private:
    dsp::ProcessorDuplicator<
        dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>
    > midLowPass;
    dsp::ProcessorDuplicator<
        dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>
    > sideLowPass;
    double lastSampleRate;

    // === Private Helper Functions ===========================================
    float clampWithinOne(float);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
