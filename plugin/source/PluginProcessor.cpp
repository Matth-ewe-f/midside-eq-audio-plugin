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
		// on-off parameters
		std::make_unique<Parameter>(
			"lpf1-on", "Low-Pass On/Off (M/L)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"lpf2-on", "Low-Pass On/Off (S/R)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak1-on", "Peak #1 On/Off (M/L)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak2-on", "Peak #1 On/Off (S/R)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak3-on", "Peak #2 On/Off (M/L)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak4-on", "Peak #2 On/Off (S/R)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak5-on", "Peak #3 On/Off (M/L)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"peak6-on", "Peak #3 On/Off (S/R)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"hpf1-on", "High-Pass On/Off (M/L)", onOffRange, 1
		),
		std::make_unique<Parameter>(
			"hpf2-on", "High-Pass On/Off (S/R)", onOffRange, 1
		),
		// frequency parameters
		std::make_unique<Parameter>(
			"hpf1-freq", "High-Pass Frequency (M/L)", freqRange, 20
		),
		std::make_unique<Parameter>(
			"hpf2-freq", "High-Pass Frequency (S/R)", freqRange, 20
		),
		std::make_unique<Parameter>(
			"peak1-freq", "Peak #1 Frequency (M/L)", freqRange, 200
		),
		std::make_unique<Parameter>(
			"peak2-freq", "Peak #1 Frequency (S/R)", freqRange, 200
		),
		std::make_unique<Parameter>(
			"peak3-freq", "Peak #2 Frequency (M/L)", freqRange, 1000
		),
		std::make_unique<Parameter>(
			"peak4-freq", "Peak #2 Frequency (S/R)", freqRange, 1000
		),
		std::make_unique<Parameter>(
			"peak5-freq", "Peak #3 Frequency (M/L)", freqRange, 6000
		),
		std::make_unique<Parameter>(
			"peak6-freq", "Peak #3 Frequency (S/R)", freqRange, 6000
		),
		std::make_unique<Parameter>(
			"lpf1-freq", "Low-Pass Frequency (M/L)", freqRange, 20000
		),
		std::make_unique<Parameter>(
			"lpf2-freq", "Low-Pass Frequency (S/R)", freqRange, 20000
		),
		// falloff (filter order) parameters
		std::make_unique<Parameter>(
			"hpf1-falloff", "High-Pass Falloff (M/L)", falloffRange, 6
		),
		std::make_unique<Parameter>(
			"hpf2-falloff", "High-Pass Falloff (S/R)", falloffRange, 6
		),
		std::make_unique<Parameter>(
			"lpf1-falloff", "Low-Pass Falloff (M/L)", falloffRange, 6
		),
		std::make_unique<Parameter>(
			"lpf2-falloff", "Low-Pass Falloff (S/R)", falloffRange, 6
		),
		// gain parameters (parametric EQ)
		std::make_unique<Parameter>(
			"peak1-gain", "Peak #1 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak2-gain", "Peak #1 Gain (S/R)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak3-gain", "Peak #2 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak4-gain", "Peak #2 Gain (S/R)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak5-gain", "Peak #3 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak6-gain", "Peak #3 Gain (S/R)", gainRange, 0
		),
		// resonance parameters
		std::make_unique<Parameter>(
			"hpf1-res", "High-Pass Resonance (M/L)", resRange, 0.71
		),
		std::make_unique<Parameter>(
			"hpf2-res", "High-Pass Resonance (S/R)", resRange, 0.71
		),
		std::make_unique<Parameter>(
			"lpf1-res", "Low-Pass Resonance (M/L)", resRange, 0.71
		),
		std::make_unique<Parameter>(
			"lpf2-res", "Low-Pass Resonance (S/R)", resRange, 0.71
		),
		// q-factor parameters
		std::make_unique<Parameter>(
			"peak1-q", "Peak #1 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak2-q", "Peak #1 Q (S/R)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak3-q", "Peak #2 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak4-q", "Peak #2 Q (S/R)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak5-q", "Peak #3 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak6-q", "Peak #3 Q (S/R)", qRange, 0.71
		),
		// configuration parameters
		std::make_unique<Parameter>(
			"mode", "Mode", juce::NormalisableRange<float>(0, 1, 1), 0
		)
	}),
	peakOne(200),
	peakTwo(200),
	peakThree(1000),
	peakFour(1000),
	peakFive(6000),
	peakSix(6000),
	lastSampleRate(48000) // default value
{
	// parameters
	highPassOne.setListenTo(&tree, "hpf1");
	highPassTwo.setListenTo(&tree, "hpf2");
	peakOne.setListenTo(&tree, "peak1");
	peakTwo.setListenTo(&tree, "peak2");
	peakThree.setListenTo(&tree, "peak3");
	peakFour.setListenTo(&tree, "peak4");
	peakFive.setListenTo(&tree, "peak5");
	peakSix.setListenTo(&tree, "peak6");
	lowPassOne.setListenTo(&tree, "lpf1");
	lowPassTwo.setListenTo(&tree, "lpf2");
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
	highPassOne.reset(sampleRate, samplesPerBlock);
	highPassOne.prepare(spec);
	highPassTwo.reset(sampleRate, samplesPerBlock);
	highPassTwo.prepare(spec);
	peakOne.reset(sampleRate, samplesPerBlock);
	peakOne.prepare(spec);
	peakTwo.reset(sampleRate, samplesPerBlock);
	peakTwo.prepare(spec);
	peakThree.reset(sampleRate, samplesPerBlock);
	peakThree.prepare(spec);
	peakFour.reset(sampleRate, samplesPerBlock);
	peakFour.prepare(spec);
	peakFive.reset(sampleRate, samplesPerBlock);
	peakFive.prepare(spec);
	peakSix.reset(sampleRate, samplesPerBlock);
	peakSix.prepare(spec);
	lowPassOne.reset(sampleRate, samplesPerBlock);
	lowPassOne.prepare(spec);
	lowPassTwo.reset(sampleRate, samplesPerBlock);
	lowPassTwo.prepare(spec);
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
	// process the audio
	float* left = buffer.getWritePointer(0);
	float* right = buffer.getWritePointer(1);
	size_t length = (size_t) buffer.getNumSamples();
	for (size_t i = 0;i < length;i++)
	{
		float sampleOne;
		float sampleTwo;
		if (isMidSide())
		{
			sampleOne = (left[i] + right[i]) / 2;
			sampleTwo = (left[i] - right[i]) / 2;
		}
		else
		{
			sampleOne = left[i];
			sampleTwo = right[i];
		}
		sampleOne = processSampleChannelOne(sampleOne);
		sampleTwo = processSampleChannelTwo(sampleTwo);
		if (isMidSide())
		{
			left[i] = clampWithinOne(sampleOne + sampleTwo);
			right[i] = clampWithinOne(sampleOne - sampleTwo);
		}
		else
		{
			left[i] = sampleOne;
			right[i] = sampleTwo;
		}
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
float PluginProcessor::processSampleChannelOne(float sample)
{
	sample = highPassOne.processSample(sample);
	sample = peakOne.processSample(sample);
	sample = peakThree.processSample(sample);
	sample = peakFive.processSample(sample);
	sample = lowPassOne.processSample(sample);
	return sample;
}

float PluginProcessor::processSampleChannelTwo(float sample)
{
	sample = highPassTwo.processSample(sample);
	sample = peakTwo.processSample(sample);
	sample = peakFour.processSample(sample);
	sample = peakSix.processSample(sample);
	sample = lowPassTwo.processSample(sample);
	return sample;
}

void PluginProcessor::addParameterListener(ParameterListener* listener)
{
	paramListeners.push_front(listener);
	tree.addParameterListener(listener->parameter, listener);
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
