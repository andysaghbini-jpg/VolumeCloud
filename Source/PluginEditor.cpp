#include "PluginProcessor.h"
#include "PluginEditor.h"

VolumeCloudAudioProcessorEditor::VolumeCloudAudioProcessorEditor (VolumeCloudAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (440, 290);

    volumeSlider.setLookAndFeel (&cottonCandyLnF);
    volumeSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 24);
    volumeSlider.setRange (0.0, 2.0, 0.001);

    volumeSlider.textFromValueFunction = [](double value)
    {
        double db = juce::Decibels::gainToDecibels (value, -100.0);
        if (db <= -100.0)
            return juce::String ("-inf dB");
        return (db >= 0.0 ? "+" : "") + juce::String (db, 1) + " dB";
    };

    volumeSlider.setValue (audioProcessor.targetVolume, juce::dontSendNotification);

    volumeSlider.onValueChange = [this]
    {
        const float newVol = static_cast<float> (volumeSlider.getValue());
        audioProcessor.targetVolume = newVol;
        audioProcessor.rawVolume.setTargetValue (newVol);
        repaint();
    };

    addAndMakeVisible (&volumeSlider);
}

VolumeCloudAudioProcessorEditor::~VolumeCloudAudioProcessorEditor()
{
    volumeSlider.setLookAndFeel (nullptr);
}

void VolumeCloudAudioProcessorEditor::paint (juce::Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    // Normalized volume (0.0 to 1.0) controls the atmospheric intensity
    const float intensity = juce::jlimit (0.0f, 1.0f, audioProcessor.targetVolume / 2.0f);

    // 1. Pink to Pastel Blue gradient
    juce::ColourGradient skyGrad (juce::Colour (252, 148, 172), w * 0.85f, h * 0.1f,   // Top-right warm pastel pink
                                  juce::Colour (152, 198, 248), w * 0.10f, h * 0.9f,   // Bottom-left pastel blue
                                  false);
    skyGrad.addColour (0.48, juce::Colour (226, 178, 228)); // Dreamy cotton-candy lavender transition
    g.setGradientFill (skyGrad);
    g.fillAll();

    // Reactive Atmospheric Clouds
    const float mistAlpha = 0.12f + (0.26f * intensity);
    const float mistScale = 0.85f + (0.35f * intensity);

    juce::ColourGradient topBloom (juce::Colours::white.withAlpha (mistAlpha), w * 0.5f, 52.0f,
                                   juce::Colours::transparentWhite, w * 0.5f + (130.0f * mistScale), 52.0f, true);
    g.setGradientFill (topBloom);
    g.fillEllipse (w * 0.5f - (140.0f * mistScale), 10.0f, 280.0f * mistScale, 90.0f * mistScale);

    juce::ColourGradient centerMist (juce::Colour (255, 240, 250).withAlpha (mistAlpha * 0.9f), w * 0.5f, h * 0.52f,
                                     juce::Colours::transparentWhite, w * 0.5f, h * 0.95f, true);
    g.setGradientFill (centerMist);
    g.fillEllipse (w * 0.15f, h * 0.25f, w * 0.70f, h * 0.55f);

    // "VolumeCloud" Title
    juce::Font titleFont (juce::FontOptions (27.0f).withStyle ("Bold"));
    titleFont.setExtraKerningFactor (0.12f);
    g.setFont (titleFont);

    // Outer dreamy title halo that reacts to the slider
    g.setColour (juce::Colours::white.withAlpha (0.15f + 0.30f * intensity));
    g.drawFittedText ("V O L U M E C L O U D", 0, 35, getWidth(), 36, juce::Justification::centred, 1);
    g.drawFittedText ("V O L U M E C L O U D", 0, 37, getWidth(), 36, juce::Justification::centred, 1);

    // Crisp foreground title
    g.setColour (juce::Colours::white.withAlpha (0.96f));
    g.drawFittedText ("V O L U M E C L O U D", 0, 36, getWidth(), 36, juce::Justification::centred, 1);

    // 4. Frosted Glass Pill behind the dB readout
    const float pillW = 86.0f;
    const float pillH = 24.0f;
    const float pillX = (w - pillW) * 0.5f;
    const float pillY = 171.0f;

    g.setColour (juce::Colours::white.withAlpha (0.18f));
    g.fillRoundedRectangle (pillX, pillY, pillW, pillH, pillH * 0.5f);
    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.drawRoundedRectangle (pillX, pillY, pillW, pillH, pillH * 0.5f, 1.0f);

}

void VolumeCloudAudioProcessorEditor::resized()
{
    volumeSlider.setBounds (55, 110, getWidth() - 110, 85);
}
