#pragma once
#include <list>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_data_structures/juce_data_structures.h>
#include <melatonin_perfetto/melatonin_perfetto.h>
#include "ParameterListener.h"
#include "HighPassFilter.h"
#include "PeakFilter.h"
#include "LowPassFilter.h"
#include "GainFilter.h"

namespace dsp = juce::dsp;

class PluginProcessor final : public juce::AudioProcessor
{
public:
    // === Public Variables ===================================================
    GainFilter gainOne;
    GainFilter gainTwo;
    HighPassFilter highPassOne;
    HighPassFilter highPassTwo;
    PeakFilter peakOne;
    PeakFilter peakTwo;
    PeakFilter peakThree;
    PeakFilter peakFour;
    PeakFilter peakFive;
    PeakFilter peakSix;
    LowPassFilter lowPassOne;
    LowPassFilter lowPassTwo;
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState tree;

    // === Lifecycle ==========================================================
    PluginProcessor();
    ~PluginProcessor() override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    // === Plugin Information =================================================
    inline const juce::String getName() const override
        { return "Split EQ"; }

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
    bool isMidSide();
    void resetAllParams();
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void notifyHostOfStateChange();

    // === Factory Functions ==================================================
    juce::AudioProcessorEditor* createEditor() override;

private:
    double lastSampleRate;
    std::list<ParameterListener*> paramListeners;
#if PERFETTO
    std::unique_ptr<perfetto::TracingSession> tracingSession;
#endif

    // === Parameter Ranges ===================================================
    inline static const juce::NormalisableRange<float> onOffRange
        { juce::NormalisableRange<float>(0, 1, 1) };
    inline static const juce::NormalisableRange<float> freqRange
        { juce::NormalisableRange<float>(20, 20000, 0.1f, 0.35f) };
    inline static const juce::NormalisableRange<float> falloffRange
        { juce::NormalisableRange<float>(6, 36, 6, 1) };
    inline static const juce::NormalisableRange<float> gainRange
        { juce::NormalisableRange<float>(-18, 18, 0.1f) };
    inline static const juce::NormalisableRange<float> resRange
        { juce::NormalisableRange<float>(0.5, 10, 0.01f, 0.7f) };
    inline static const juce::NormalisableRange<float> qRange
        { juce::NormalisableRange<float>(0.25, 10, 0.01f, 0.7f) };

    // === Other Helper Functions =============================================
    float processSampleChannelOne(float sample);
    float processSampleChannelTwo(float sample);
    void resetFilterParams(CtmFilter*);
    void addParameterListener(ParameterListener*);
    float clampWithinOne(float);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
