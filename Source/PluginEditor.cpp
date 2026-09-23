/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VolumeCloudAudioProcessorEditor::VolumeCloudAudioProcessorEditor (VolumeCloudAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setRange(0.0, 2.0);
    
    volumeSlider.textFromValueFunction = [](double value)
    {
        double db = juce::Decibels::gainToDecibels(value, -100.0);
        return juce::String(db, 1) + " dB";
    };
    
    volumeSlider.setValue(1.0);
    addAndMakeVisible(&volumeSlider);
    volumeSlider.onValueChange = [this] { audioProcessor.rawVolume = volumeSlider.getValue(); };
}

VolumeCloudAudioProcessorEditor::~VolumeCloudAudioProcessorEditor()
{
}

//==============================================================================
void VolumeCloudAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void VolumeCloudAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    volumeSlider.setBounds(50, 125, 300, 50); //should be middle
}
