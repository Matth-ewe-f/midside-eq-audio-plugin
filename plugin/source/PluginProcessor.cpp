#include "PluginProcessor.h"
#include "PluginEditor.h"

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
	gainOne("gain1", "Channel Gain {0} (M/L)"),
	gainTwo("gain2", "Channel Gain {0} (S/R)"),
	highPassOne("hpf1", "High-Pass {0} (M/L)"),
	highPassTwo("hpf2", "High-Pass {0} (S/R)"),
	peakOne("peak1", "Peak #1 {0} (M/L)", 200),
	peakTwo("peak2", "Peak #1 {0} (S/R)", 200),
	peakThree("peak3", "Peak #2 {0} (M/L)", 1000),
	peakFour("peak4", "Peak #2 {0} (S/R)", 1000),
	peakFive("peak5", "Peak #3 {0} (M/L)", 6000),
	peakSix("peak6", "Peak #4 {0} (S/R)", 6000),
	lowPassOne("lpf1", "Low-Pass {0} (M/L)"),
	lowPassTwo("lpf2", "Low-Pass {0} (S/R)"),
	tree(*this, &undoManager, "PARAMETERS", createParameters()),
	lastSampleRate(48000) // default value
{
#if PERFETTO
    MelatoninPerfetto::get().beginSession();
#endif
	gainOne.setListenTo(&tree);
	gainTwo.setListenTo(&tree);
	highPassOne.setListenTo(&tree);
	highPassTwo.setListenTo(&tree);
	peakOne.setListenTo(&tree);
	peakTwo.setListenTo(&tree);
	peakThree.setListenTo(&tree);
	peakFour.setListenTo(&tree);
	peakFive.setListenTo(&tree);
	peakSix.setListenTo(&tree);
	lowPassOne.setListenTo(&tree);
	lowPassTwo.setListenTo(&tree);
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
	highPassOne.stopListeningTo(&tree);
	highPassTwo.stopListeningTo(&tree);
#if PERFETTO
    MelatoninPerfetto::get().endSession();
#endif
}

juce::AudioProcessorValueTreeState::ParameterLayout
PluginProcessor::createParameters()
{
	juce::AudioProcessorValueTreeState::ParameterLayout parameters;
	gainOne.addParameters(&parameters);
	gainTwo.addParameters(&parameters);
	highPassOne.addParameters(&parameters);
	highPassTwo.addParameters(&parameters);
	peakOne.addParameters(&parameters);
	peakTwo.addParameters(&parameters);
	peakThree.addParameters(&parameters);
	peakFour.addParameters(&parameters);
	peakFive.addParameters(&parameters);
	peakSix.addParameters(&parameters);
	lowPassOne.addParameters(&parameters);
	lowPassTwo.addParameters(&parameters);
	parameters.add(std::make_unique<Parameter>(
		"mode", "Mode", juce::NormalisableRange<float>(0, 1, 1), 0
	));
	parameters.add(std::make_unique<Parameter>(
		"gain-linked",
		"Gain Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	parameters.add(std::make_unique<Parameter>(
		"hpf-linked",
		"High-Pass Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	parameters.add(std::make_unique<Parameter>(
		"peak12-linked",
		"Peak Filter #1 Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	parameters.add(std::make_unique<Parameter>(
		"peak34-linked",
		"Peak Filter #2 Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	parameters.add(std::make_unique<Parameter>(
		"peak56-linked",
		"Peak Filter #3 Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	parameters.add(std::make_unique<Parameter>(
		"lpf-linked",
		"Low-Pass Channels Linked",
		juce::NormalisableRange<float>(0, 1, 1),
		0
	));
	return parameters;
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
	gainOne.reset(samplesPerBlock);
	gainTwo.reset(samplesPerBlock);
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

void PluginProcessor::resetAllParams()
{
	resetFilterParams(&gainOne);
	resetFilterParams(&gainTwo);
	resetFilterParams(&highPassOne);
	resetFilterParams(&highPassTwo);
	resetFilterParams(&peakOne);
	resetFilterParams(&peakTwo);
	resetFilterParams(&peakThree);
	resetFilterParams(&peakFour);
	resetFilterParams(&peakFive);
	resetFilterParams(&peakSix);
	resetFilterParams(&lowPassOne);
	resetFilterParams(&lowPassTwo);
}

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

void PluginProcessor::notifyHostOfStateChange()
{
	updateHostDisplay(ChangeDetails().withNonParameterStateChanged(true));
}

// === Other Private Helper ===================================================
float PluginProcessor::processSampleChannelOne(float sample)
{
	sample = gainOne.processSample(sample);
	sample = highPassOne.processSample(sample);
	sample = peakOne.processSample(sample);
	sample = peakThree.processSample(sample);
	sample = peakFive.processSample(sample);
	sample = lowPassOne.processSample(sample);
	return sample;
}

float PluginProcessor::processSampleChannelTwo(float sample)
{
	sample = gainTwo.processSample(sample);
	sample = highPassTwo.processSample(sample);
	sample = peakTwo.processSample(sample);
	sample = peakFour.processSample(sample);
	sample = peakSix.processSample(sample);
	sample = lowPassTwo.processSample(sample);
	return sample;
}

void PluginProcessor::resetFilterParams(CtmFilter* filter)
{
	std::vector<ParameterFields> parameters;
	filter->getParameters(parameters);
	for (ParameterFields fields : parameters)
	{
		std::string name = filter->name + "-" + fields.idPostfix;
		juce::RangedAudioParameter* param = tree.getParameter(name);
		param->beginChangeGesture();
		param->setValueNotifyingHost(param->getDefaultValue());
		param->endChangeGesture();
	}
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
