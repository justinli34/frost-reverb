#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "signalsmith/signalsmith-stretch.h"

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int numChannels = getTotalNumInputChannels();

    int blockSamples = 2048;
    int intervalSamples = blockSamples / 4;
    shifter1.configure(numChannels, blockSamples, intervalSamples, false);
    shifter2.configure(numChannels, blockSamples, intervalSamples, false);

    reverb.setSampleRate(sampleRate);
    reverbParams.roomSize = 0.9f;
    reverbParams.damping = 0.3f;
    reverbParams.wetLevel = 1.0f;
    reverbParams.dryLevel = 0.0f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    wetBuffer.setSize(numChannels, samplesPerBlock);
    tempBuffer.setSize(numChannels, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    // Pointers for signalsmith
    std::vector<const float*> sourcePtrs(totalNumInputChannels);
    std::vector<float*> shift1Ptrs(totalNumInputChannels);
    std::vector<float*> shift2Ptrs(totalNumInputChannels);

    for (int c = 0; c < totalNumInputChannels; ++c)
    {
        sourcePtrs[c] = buffer.getReadPointer(c);
        shift1Ptrs[c] = tempBuffer.getWritePointer(c);
        shift2Ptrs[c] = wetBuffer.getWritePointer(c);
    }

    // Pitch shift
    float shiftValue1 = *apvts.getRawParameterValue("shift1");
    float shiftValue2 = *apvts.getRawParameterValue("shift2");
    shifter1.setTransposeSemitones(shiftValue1);
    shifter2.setTransposeSemitones(shiftValue2);

    shifter1.process(sourcePtrs, numSamples, shift1Ptrs, numSamples);
    shifter2.process(sourcePtrs, numSamples, shift2Ptrs, numSamples);

    for (int c = 0; c < totalNumInputChannels; ++c)
    {
        wetBuffer.addFrom(c, 0, tempBuffer, c, 0, numSamples, 1.0f);
    }

    // Reverb
    if (totalNumInputChannels == 2)
    {
        reverb.processStereo(wetBuffer.getWritePointer(0), wetBuffer.getWritePointer(1), numSamples);
    }
    else if (totalNumInputChannels == 1)
    {
        reverb.processMono(wetBuffer.getWritePointer(0), numSamples);
    }

    // Mix wet signal into main buffer
    float mixAmount = *apvts.getRawParameterValue("mix");
    
    for (int c = 0; c < totalNumInputChannels; ++c)
    {
        buffer.addFrom(c, 0, wetBuffer, c, 0, numSamples, mixAmount);
    }
}

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "shift1", 
        "Shift 1", 
        juce::NormalisableRange<float>(-12.0f, 24.0f, 1.0f),
        12.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "shift2", 
        "Shift 2", 
        juce::NormalisableRange<float>(-12.0f, 24.0f, 1.0f),
        12.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix", 
        "Mix", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f),
        0.5f));

    return layout;
}