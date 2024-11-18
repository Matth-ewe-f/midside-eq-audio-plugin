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
	lowPassOne(dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000)),
	lowPassTwo(dsp::IIR::Coefficients<float>::makeLowPass(48000, 20000))
{ 
	// default
	lastSampleRate = 48000;
    freqOne = new juce::AudioParameterFloat(
		"freq-one", "Frequency (Mid/Left)", 20, 20000, 20000
	);
    freqTwo = new juce::AudioParameterFloat(
		"freq-two", "Frequency (Side/Right)", 20, 20000, 20000
	);
	isMidSide = new juce::AudioParameterBool("mode", "Mode", true);
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
	lowPassOne.prepare(spec);
	lowPassOne.reset();
	lowPassTwo.prepare(spec);
	lowPassTwo.reset();
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
	float* left = buffer.getWritePointer(0);
	float* right = buffer.getWritePointer(1);
	size_t length = (size_t) buffer.getNumSamples();
	if (*isMidSide)
	{
		// split into mid and side signals
		float* mid = (float*) malloc(sizeof(float) * length);
		float* side = (float*) malloc(sizeof(float) * length);
		for (size_t i = 0;i < length;i++)
		{
			mid[i] = (left[i] + right[i]) / 2;
			side[i] = (left[i] - right[i]) / 2;
		}
		// process the audio
		dsp::AudioBlock<float> midBlock(&mid, 1, length);
		dsp::AudioBlock<float> sideBlock(&side, 1, length);
		lowPassOne.process(dsp::ProcessContextReplacing<float>(midBlock));
		lowPassTwo.process(dsp::ProcessContextReplacing<float>(sideBlock));
		// recombine the mid and side signals into left and right
		for (size_t i = 0;i < length;i++)
		{
			left[i] = clampWithinOne(mid[i] + side[i]);
			right[i] = clampWithinOne(mid[i] - side[i]);
		}
	}
	else
	{
		dsp::AudioBlock<float> leftBlock(&left, 1, length);
		dsp::AudioBlock<float> rightBlock(&right, 1, length);
		lowPassOne.process(dsp::ProcessContextReplacing<float>(leftBlock));
		lowPassTwo.process(dsp::ProcessContextReplacing<float>(rightBlock));
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
	*lowPassOne.coefficients = *dsp::IIR::Coefficients<float>::makeLowPass(
		lastSampleRate, *freqOne
	);
	*lowPassTwo.coefficients = *dsp::IIR::Coefficients<float>::makeLowPass(
		lastSampleRate, *freqTwo
	);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	auto stream = juce::MemoryOutputStream(destData, true);
	stream.writeFloat(*freqOne);
	stream.writeFloat(*freqTwo);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	auto stream = juce::MemoryInputStream(data, (size_t)sizeInBytes, false);
	*freqOne = stream.readFloat();
	*freqTwo = stream.readFloat();
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
