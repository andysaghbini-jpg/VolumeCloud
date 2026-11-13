#include "PluginProcessor.h"
#include "PluginEditor.h"

VolumeCloudAudioProcessor::VolumeCloudAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VolumeCloudAudioProcessor::~VolumeCloudAudioProcessor()
{
}

const juce::String VolumeCloudAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VolumeCloudAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VolumeCloudAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VolumeCloudAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VolumeCloudAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VolumeCloudAudioProcessor::getNumPrograms()
{
    return 1;
}

int VolumeCloudAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VolumeCloudAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String VolumeCloudAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void VolumeCloudAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void VolumeCloudAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    // 50ms linear ramp for smooth, click-free volume transitions
    rawVolume.reset (sampleRate, 0.05);
    rawVolume.setCurrentAndTargetValue (targetVolume);
}

void VolumeCloudAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VolumeCloudAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VolumeCloudAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (rawVolume.isSmoothing())
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float currentVolume = rawVolume.getNextValue();

            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                buffer.getWritePointer (channel)[sample] *= currentVolume;
            }
        }
    }
    else
    {
        buffer.applyGain (rawVolume.getCurrentValue());
    }
}

bool VolumeCloudAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VolumeCloudAudioProcessor::createEditor()
{
    return new VolumeCloudAudioProcessorEditor (*this);
}

void VolumeCloudAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save the current targetVolume when the DAW saves the project
    juce::MemoryOutputStream stream (destData, true);
    stream.writeFloat (targetVolume);
}

void VolumeCloudAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore the saved volume when reopening the DAW project
    juce::MemoryInputStream stream (data, static_cast<size_t> (sizeInBytes), false);
    if (stream.getNumBytesRemaining() >= sizeof (float))
    {
        targetVolume = stream.readFloat();
        rawVolume.setCurrentAndTargetValue (targetVolume);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VolumeCloudAudioProcessor();
}
