#pragma once

#include "PluginProcessor.h"

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::Slider shiftSlider1;
    juce::Label shiftLabel1;
    juce::Slider shiftSlider2;
    juce::Label shiftLabel2;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shiftAttachment1;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shiftAttachment2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
