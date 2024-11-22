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
		// frequency parameters
		std::make_unique<Parameter>(
			"hpf1-freq", "High-Pass Frequency (M/L)", freqRange, 20
		),
		std::make_unique<Parameter>(
			"hpf2-freq", "High-Pass Frequency (S/R)", freqRange, 20
		),
		std::make_unique<Parameter>(
			"peak1-freq", "Parametric #1 Frequency (M/L)", freqRange, 200
		),
		std::make_unique<Parameter>(
			"peak2-freq", "Parametric #1 Frequency (S/R)", freqRange, 200
		),
		std::make_unique<Parameter>(
			"peak3-freq", "Parametric #2 Frequency (M/L)", freqRange, 1000
		),
		std::make_unique<Parameter>(
			"peak4-freq", "Parametric #2 Frequency (S/R)", freqRange, 1000
		),
		std::make_unique<Parameter>(
			"peak5-freq", "Parametric #3 Frequency (M/L)", freqRange, 6000
		),
		std::make_unique<Parameter>(
			"peak6-freq", "Parametric #3 Frequency (S/R)", freqRange, 6000
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
			"peak1-gain", "Parametric #1 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak2-gain", "Parametric #1 Gain (S/R)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak3-gain", "Parametric #2 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak4-gain", "Parametric #2 Gain (S/R)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak5-gain", "Parametric #3 Gain (M/L)", gainRange, 0
		),
		std::make_unique<Parameter>(
			"peak6-gain", "Parametric #3 Gain (S/R)", gainRange, 0
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
			"peak1-q", "Parametric #1 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak2-q", "Parametric #1 Q (S/R)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak3-q", "Parametric #2 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak4-q", "Parametric #2 Q (S/R)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak5-q", "Parametric #3 Q (M/L)", qRange, 0.71
		),
		std::make_unique<Parameter>(
			"peak6-q", "Parametric #3 Q (S/R)", qRange, 0.71
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
	addParameterListener(new ParameterListener(
		"lpf1-freq", [this](float value) {
			lowPassOne.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"lpf1-falloff", [this](float value) {
			lowPassOne.setOrder((int)value / 6);
		}
	));
	addParameterListener(new ParameterListener(
		"lpf2-freq", [this](float value) {
			lowPassTwo.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"lpf2-falloff", [this](float value) {
			lowPassTwo.setOrder((int)value / 6);
		}
	));
	addParameterListener(new ParameterListener(
		"peak1-freq", [this](float value) {
			peakOne.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak1-gain", [this](float value) {
			peakOne.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak1-q", [this](float value) {
			peakOne.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak2-freq", [this](float value) {
			peakTwo.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak2-gain", [this](float value) {
			peakTwo.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak2-q", [this](float value) {
			peakTwo.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak3-freq", [this](float value) {
			peakThree.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak3-gain", [this](float value) {
			peakThree.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak3-q", [this](float value) {
			peakThree.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak4-freq", [this](float value) {
			peakFour.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak4-gain", [this](float value) {
			peakFour.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak4-q", [this](float value) {
			peakFour.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak5-freq", [this](float value) {
			peakFive.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak5-gain", [this](float value) {
			peakFive.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak5-q", [this](float value) {
			peakFive.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak6-freq", [this](float value) {
			peakSix.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"peak6-gain", [this](float value) {
			peakSix.setGain((float) pow(10, value / 20));
		}
	));
	addParameterListener(new ParameterListener(
		"peak6-q", [this](float value) {
			peakSix.setQFactor(value);
		}
	));
	addParameterListener(new ParameterListener(
		"hpf1-freq", [this](float value) {
			highPassOne.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"hpf1-falloff", [this](float value) {
			highPassOne.setOrder((int)value / 6);
		}
	));
	addParameterListener(new ParameterListener(
		"hpf2-freq", [this](float value) {
			highPassTwo.setFrequency(value);
		}
	));
	addParameterListener(new ParameterListener(
		"hpf2-falloff", [this](float value) {
			highPassTwo.setOrder((int)value / 6);
		}
	));
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
