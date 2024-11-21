#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace dsp = juce::dsp;
using Parameter = juce::AudioProcessorValueTreeState::Parameter;

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
	tree(*this, nullptr, "PARAMETERS", {
		std::make_unique<Parameter>(
			"lpf1-freq", "Low-Pass Frequency (Mid/Left)",
			juce::NormalisableRange<float>(20, 20000, 0.1f, 0.35f), 20000
		),
		std::make_unique<Parameter>(
			"lpf1-falloff", "Low-Pass Falloff (Mid/Left)",
			juce::NormalisableRange<float>(6, 36, 6, 1), 6
		),
		std::make_unique<Parameter>(
			"lpf1-res", "Low-Pass Resonance (Mid/Left)",
			juce::NormalisableRange<float>(0.5, 10, 0.01f, 0.7f), 0.71
		),
		std::make_unique<Parameter>(
			"lpf2-freq", "Low-Pass Frequency (Side/Right)",
			juce::NormalisableRange<float>(20, 20000, 0.1f, 0.35f), 20000
		),
		std::make_unique<Parameter>(
			"lpf2-falloff", "Low-Pass Falloff (Side/Right)",
			juce::NormalisableRange<float>(6, 36, 6, 1), 6
		),
		std::make_unique<Parameter>(
			"lpf2-res", "Low-Pass Resonance (Side/Right)",
			juce::NormalisableRange<float>(0.5, 10, 0.01f, 0.7f), 0.71
		),
		std::make_unique<Parameter>(
			"mode", "Mode", juce::NormalisableRange<float>(0, 1, 1), 0
		)
	}),
	lastSampleRate(48000) // default value
{
	// parameters
	addParameterListener(new ParameterListener(
		"lpf1-freq", [this](float value) {
			lowPassOne.setCutoffFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"lpf2-freq", [this](float value) {
			lowPassTwo.setCutoffFrequency(value);
		}
	));
	// processors
	lowPassOne.setType(dsp::StateVariableTPTFilterType::lowpass);
	lowPassTwo.setType(dsp::StateVariableTPTFilterType::lowpass);
	updateFilterState();
}

PluginProcessor::~PluginProcessor() 
{
	while (paramListeners.size() > 0)
	{
		ParameterListener* listener = paramListeners.front();
		paramListeners.remove(listener);
		tree.removeParameterListener(listener->parameter, listener);
		delete listener;
	}
}

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
	if (isMidSide())
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
// warning can safely be ignored - float comparison involving no arithmetic
// is perfectly safe
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
bool PluginProcessor::isMidSide()
{
	return *tree.getRawParameterValue("mode") == 0;
}
#pragma GCC diagnostic pop

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
	auto state = tree.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml.get() != nullptr && xml->hasTagName(tree.state.getType()))
		tree.replaceState(juce::ValueTree::fromXml(*xml));
}

// === Private Helper =========================================================
void PluginProcessor::addParameterListener(ParameterListener* listener)
{
	paramListeners.push_front(listener);
	tree.addParameterListener(listener->parameter, listener);
}

void PluginProcessor::updateFilterState()
{
	lowPassOne.setCutoffFrequency(*tree.getRawParameterValue("lpf1-freq"));
	lowPassTwo.setCutoffFrequency(*tree.getRawParameterValue("lpf2-freq"));
}

float PluginProcessor::clampWithinOne(float f)
{
	if (f > 1)
		return 1;
	else if (f < -1)
		return -1;
	else
		return f;
}
