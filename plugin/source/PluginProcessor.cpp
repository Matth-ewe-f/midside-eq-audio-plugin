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
	midLowPass(dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000)),
	sideLowPass(dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000))
{ 
	// default
	lastSampleRate = 48000;
    midFreq = new juce::AudioParameterFloat(
		"mid-freq", "Frequency (Middle)", 20, 20000, 20000
	);
    sideFreq = new juce::AudioParameterFloat(
		"side-freq", "Frequency (Side)", 20, 20000, 20000
	);
}

PluginProcessor::~PluginProcessor() { }

// === Plugin Information =====================================================
bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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
	midLowPass.prepare(spec);
	midLowPass.reset();
	sideLowPass.prepare(spec);
	sideLowPass.reset();
}

void PluginProcessor::releaseResources() { }

void PluginProcessor::processBlock
(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
	juce::ignoreUnused(midiMessages);
	auto numInputChannels = getTotalNumInputChannels();
	auto numOutputChannels = getTotalNumOutputChannels();
	if (numOutputChannels < 2)
	{
		return;
	}
	// zeroes out any unused outputs (if there are any)
	for (auto i = numInputChannels; i < numOutputChannels; i++)
		buffer.clear(i, 0, buffer.getNumSamples());
	// split into mid and side signals
	size_t length = (size_t) buffer.getNumSamples();
	float* mid = (float*) malloc(sizeof(float) * length);
	float* side = (float*) malloc(sizeof(float) * length);
	float* left = buffer.getWritePointer(0);
	float* right = buffer.getWritePointer(1);
	for (size_t i = 0;i < length;i++)
	{
		mid[i] = (left[i] + right[i]) / 2;
		side[i] = (left[i] - right[i]) / 2;
	}
	// process the audio
	dsp::AudioBlock<float> midBlock(&mid, 1, length);
	dsp::AudioBlock<float> sideBlock(&side, 1, length);
	midLowPass.process(dsp::ProcessContextReplacing<float>(midBlock));
	sideLowPass.process(dsp::ProcessContextReplacing<float>(sideBlock));
	// recombine the mid and side signals into left and right
	for (size_t i = 0;i < length;i++)
	{
		left[i] = clampWithinOne(mid[i] + side[i]);
		right[i] = clampWithinOne(mid[i] - side[i]);
	}
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
	*midLowPass.state = *dsp::IIR::Coefficients<float>::makeLowPass(
		lastSampleRate, *midFreq
	);
	*sideLowPass.state = *dsp::IIR::Coefficients<float>::makeLowPass(
		lastSampleRate, *sideFreq
	);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	auto stream = juce::MemoryOutputStream(destData, true);
	stream.writeFloat(*midFreq);
	stream.writeFloat(*sideFreq);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	auto stream = juce::MemoryInputStream(data, (size_t)sizeInBytes, false);
	*midFreq = stream.readFloat();
	*sideFreq = stream.readFloat();
}

// === State ==================================================================
float PluginProcessor::clampWithinOne(float f)
{
	if (f > 1)
		return 1;
	else if (f < -1)
		return -1;
	else
		return f;
}
