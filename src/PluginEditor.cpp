#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    transposeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    transposeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(transposeSlider);

    transposeLabel.setText("Shift (Semi)", juce::NotificationType::dontSendNotification);
    transposeLabel.setJustificationType(juce::Justification::centred);
    transposeLabel.attachToComponent(&transposeSlider, false); // Attach above slider
    addAndMakeVisible(transposeLabel);

    // 3. Create Attachment
    // This links the slider to the "transpose" parameter ID we defined in the processor
    transposeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "transpose", transposeSlider);
    
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    transposeSlider.setBounds(getLocalBounds().getCentreX() - 50, 
                              getLocalBounds().getCentreY() - 50, 
                              100, 100);
}
