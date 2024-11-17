#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_dsp/juce_dsp.h>
namespace dsp = juce::dsp;

// === Lifecycle ==============================================================
PluginProcessor::PluginProcessor()
	: AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	),
	lowPass(dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000))
{ 
	// default
	lastSampleRate = 48000;
    freq = new juce::AudioParameterFloat("freq", "Frequency", 20, 20000, 0.1f);
}

PluginProcessor::~PluginProcessor() { }

// === Plugin Information =====================================================
bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
		&& layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono())
		return false;

#if !JucePlugin_IsSynth
	// Check if the input matches the output
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif
	return true;
#endif
}

// === Process Audio ==========================================================
void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	lastSampleRate = sampleRate;
	dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = (unsigned) samplesPerBlock;
	spec.numChannels = (unsigned) getTotalNumOutputChannels();
	lowPass.prepare(spec);
	lowPass.reset();
}

void PluginProcessor::releaseResources() { }

void PluginProcessor::processBlock
(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
	juce::ignoreUnused(midiMessages);
	auto numInputChannels = getTotalNumInputChannels();
	auto numOutputChannels = getTotalNumOutputChannels();
	// zeroes out any unused outputs (if there are any)
	for (auto i = numInputChannels; i < numOutputChannels; i++)
		buffer.clear(i, 0, buffer.getNumSamples());
	// process the audio
	dsp::AudioBlock<float> block(buffer);
	lowPass.process(dsp::ProcessContextReplacing<float>(block));
}

// === Factory Functions ======================================================
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
	return new PluginProcessor();
}

juce::AudioProcessorEditor *PluginProcessor::createEditor()
{
	return new PluginEditor(*this);
}

// === State ==================================================================
void PluginProcessor::updateFilterState()
{
	*lowPass.state = *dsp::IIR::Coefficients<float>::makeLowPass(
		lastSampleRate, *freq
	);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	juce::ignoreUnused(destData);
	// you can store state like this
	// auto stream = juce::MemoryOutputStream(destData, true);
	// stream.writeFloat(...);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	juce::ignoreUnused(data, sizeInBytes);
	// you can load state like this
	// auto stream = juce::MemoryInputStream(data, (size_t)sizeInBytes, false);
	// ... = stream.readFloat();
}

