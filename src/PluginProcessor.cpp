#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "signalsmith/signalsmith-stretch.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
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

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // 1. Prepare the Pitch Shifter (Signalsmith Stretch)
    int numChannels = getTotalNumInputChannels();
    
    // Configure for stereo (or mono if that's what you have)
    // Using a 'cheaper' preset for lower CPU usage, as reverb tails hide artifacts well.
    shimmerShifter.presetCheaper(numChannels, sampleRate);
    
    // Set Harmonic Interval: +12 Semitones (1 Octave)
    shimmerShifter.setTransposeSemitones(12); 

    // 2. Prepare the Reverb
    reverb.setSampleRate(sampleRate);
    
    // Configure Reverb "Lushness"
    reverbParams.roomSize = 0.9f;   // Large room for "infinite" feel
    reverbParams.damping = 0.3f;    // Low damping for bright shimmer
    reverbParams.wetLevel = 1.0f;   // We handle the mix manually
    reverbParams.dryLevel = 0.0f;   // Ensure reverb output is 100% wet
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    // 3. Prepare Intermediate Buffer
    // Resize wetBuffer to match the block size and channels
    wetBuffer.setSize(numChannels, samplesPerBlock);
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

    // --- HARMONIC REVERB PROCESSING ---

    // 1. Prepare the Wet Buffer
    // We copy the dry input to the wet buffer temporarily if needed, 
    // but the shifter will overwrite it anyway. 
    // However, we must ensure wetBuffer is big enough (handled in prepareToPlay).
    if (wetBuffer.getNumSamples() != numSamples)
        wetBuffer.setSize(totalNumInputChannels, numSamples, false, false, true);
    
    wetBuffer.clear();

    // 2. Interface with Signalsmith Stretch
    // The library uses float** style access, so we create channel pointers.
    std::vector<const float*> inputPtrs(totalNumInputChannels);
    std::vector<float*> wetPtrs(totalNumInputChannels);

    for (int c = 0; c < totalNumInputChannels; ++c)
    {
        inputPtrs[c] = buffer.getReadPointer(c);
        wetPtrs[c] = wetBuffer.getWritePointer(c);
    }

    // 3. Process Pitch Shift (Input -> WetBuffer)
    // This takes the dry input, shifts it up an octave, and stores it in wetBuffer.
    shimmerShifter.process(inputPtrs, numSamples, wetPtrs, numSamples);

    // 4. Process Reverb (WetBuffer -> WetBuffer)
    // Apply reverb to the pitch-shifted signal.
    if (totalNumInputChannels == 2)
    {
        reverb.processStereo(wetBuffer.getWritePointer(0), wetBuffer.getWritePointer(1), numSamples);
    }
    else if (totalNumInputChannels == 1)
    {
        reverb.processMono(wetBuffer.getWritePointer(0), numSamples);
    }

    // 5. Mix Wet Signal back into Main Buffer
    // We blend the original Dry signal (in 'buffer') with the Shimmer Reverb (in 'wetBuffer')
    float shimmerAmount = 0.5f; // Adjust this gain for more/less shimmer
    
    for (int c = 0; c < totalNumInputChannels; ++c)
    {
        // buffer = buffer + (wetBuffer * gain)
        buffer.addFrom(c, 0, wetBuffer, c, 0, numSamples, shimmerAmount);
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
