#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    // Shift Slider 1
    shiftSlider1.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    shiftSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(shiftSlider1);

    shiftLabel1.setText("Pitch 1", juce::NotificationType::dontSendNotification);
    shiftLabel1.setJustificationType(juce::Justification::centred);
    shiftLabel1.attachToComponent(&shiftSlider1, false);
    addAndMakeVisible(shiftLabel1);

    shiftAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "shift1", shiftSlider1);

    // Shift Toggle 1
    shift1Toggle.setButtonText("On/Off");
    addAndMakeVisible(shift1Toggle);

    shift1ToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "shift1On", shift1Toggle);

    // Shift Slider 2
    shiftSlider2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    shiftSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(shiftSlider2);

    shiftLabel2.setText("Pitch 2", juce::NotificationType::dontSendNotification);
    shiftLabel2.setJustificationType(juce::Justification::centred);
    shiftLabel2.attachToComponent(&shiftSlider2, false);
    addAndMakeVisible(shiftLabel2);

    shiftAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "shift2", shiftSlider2);

    // Shift Toggle 2
    shift2Toggle.setButtonText("On/Off");
    addAndMakeVisible(shift2Toggle);

    shift2ToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "shift2On", shift2Toggle);

    // Mix Slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(mixSlider);

    mixLabel.setText("Mix", juce::NotificationType::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixLabel);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixSlider);

    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(125, 175, 250));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    shiftSlider1.setBounds(getLocalBounds().getCentreX() - 100,
                              getLocalBounds().getCentreY() - 100,
                              100, 100);

    shift1Toggle.setBounds(getLocalBounds().getCentreX() - 200,
                              getLocalBounds().getCentreY() - 100,
                              100, 100);

    shiftSlider2.setBounds(getLocalBounds().getCentreX(),
                              getLocalBounds().getCentreY() - 100,
                              100, 100);

    shift2Toggle.setBounds(getLocalBounds().getCentreX() + 100,
                              getLocalBounds().getCentreY() - 100,
                              100, 100);

    mixSlider.setBounds(getLocalBounds().getCentreX() - 50,
                              getLocalBounds().getCentreY() + 20,
                              100, 100);
}
