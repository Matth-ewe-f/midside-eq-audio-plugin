#pragma once
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterBlueprint.h"
#include "FilterStateListener.h"

using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;

class CtmFilter : public juce::AudioProcessorValueTreeState::Listener
{
public:
    const std::string name;
    const std::string paramText;
    const std::string secondaryParamText;
    
    // === Lifecycle ==========================================================
    CtmFilter
    (std::string name, std::string paramText, std::string secondText = "");

    // === ValueTreeState Listener ============================================
    void setListenTo(juce::AudioProcessorValueTreeState*);
    void stopListeningTo(juce::AudioProcessorValueTreeState*);
    void parameterChanged(const juce::String&, float) override;
    
    // === For EQ Displays ====================================================
    void addStateListener(FilterStateListener*);
    void removeStateListener(FilterStateListener*);
    virtual void getMagnitudes(const double*, double*, size_t) = 0;

    // === Parameters =========================================================
    void addParameters(ParameterLayout*);
    virtual std::string getOnOffParameter() = 0;
    virtual void getParameters(std::vector<ParameterBlueprint>& container) = 0;

    // === Process Audio ======================================================
    void link(CtmFilter*);
    void unlink(CtmFilter*);
    virtual void setParamsOnLink(std::string) = 0;

    // === Process Audio ======================================================
    float processSample(float);

protected:
    std::atomic<long long> timeAtLastProcess;
    juce::AudioProcessorValueTreeState* stateTree;

    virtual float processSampleProtected(float) = 0;
    virtual void onChangedParameter(const juce::String&, float) = 0;
    bool isProcessing();
    std::string getIdForParameter(const ParameterBlueprint*);

private:
    std::vector<FilterStateListener*> listeners;
    std::vector<CtmFilter*> linked;
    void nofityListeners();
};