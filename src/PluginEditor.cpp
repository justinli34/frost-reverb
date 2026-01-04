#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    shiftSlider1.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    shiftSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(shiftSlider1);

    shiftLabel1.setText("Pitch (Semitones)", juce::NotificationType::dontSendNotification);
    shiftLabel1.setJustificationType(juce::Justification::centred);
    shiftLabel1.attachToComponent(&shiftSlider1, false); // Attach above slider
    addAndMakeVisible(shiftLabel1);

    shiftAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "shift1", shiftSlider1);

    shiftSlider2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    shiftSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(shiftSlider2);

    shiftLabel2.setText("Pitch (Semitones)", juce::NotificationType::dontSendNotification);
    shiftLabel2.setJustificationType(juce::Justification::centred);
    shiftLabel2.attachToComponent(&shiftSlider2, false); // Attach above slider
    addAndMakeVisible(shiftLabel2);

    shiftAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "shift2", shiftSlider2);

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
    shiftSlider1.setBounds(getLocalBounds().getCentreX() - 100, 
                              getLocalBounds().getCentreY() - 50, 
                              100, 100);

    shiftSlider2.setBounds(getLocalBounds().getCentreX(), 
                              getLocalBounds().getCentreY() - 50, 
                              100, 100);
}
