#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CottonCandyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CottonCandyLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    }

    int getSliderThumbRadius (juce::Slider&) override
    {
        return 30;
    }

    juce::Font getSliderPopupFont (juce::Slider&) override
    {
        return juce::Font (juce::FontOptions (13.0f)).boldened();
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        juce::ignoreUnused (minSliderPos, maxSliderPos, style);

        const float trackHeight = 6.0f;
        const float centerY = (float) y + (float) height * 0.5f;
        const float trackY = centerY - trackHeight * 0.5f;

        // Normalized value (0.0 to 1.0) to drive reactive glow
        const float normValue = static_cast<float> ((slider.getValue() - slider.getMinimum())
                                                  / (slider.getMaximum() - slider.getMinimum()));

        // 1. Sleek recessed glass slider track
        g.setColour (juce::Colour (30, 40, 75).withAlpha (0.14f));
        g.fillRoundedRectangle ((float) x, trackY, (float) width, trackHeight, trackHeight * 0.5f);

        g.setColour (juce::Colours::white.withAlpha (0.35f));
        g.drawRoundedRectangle ((float) x, trackY, (float) width, trackHeight, trackHeight * 0.5f, 1.0f);

        // 2. Glowing active fill bar (Pastel Blue to White-Pink)
        const float fillWidth = juce::jlimit (0.0f, (float) width, sliderPos - (float) x);
        if (fillWidth > 1.0f)
        {
            // Outer bloom on the active track
            g.setColour (juce::Colours::white.withAlpha (0.25f + 0.25f * normValue));
            g.fillRoundedRectangle ((float) x - 2.0f, trackY - 2.0f, fillWidth + 4.0f, trackHeight + 4.0f, (trackHeight + 4.0f) * 0.5f);

            juce::ColourGradient fillGrad (juce::Colour (185, 225, 255), (float) x, centerY,
                                           juce::Colours::white, sliderPos, centerY, false);
            g.setGradientFill (fillGrad);
            g.fillRoundedRectangle ((float) x, trackY, fillWidth, trackHeight, trackHeight * 0.5f);
        }

        // 3. Single continuous Bezier Cloud silhouette (zero internal ring artifacts)
        const float cx = sliderPos;
        const float cy = centerY + 4.0f;

        juce::Path cloud;
        cloud.startNewSubPath (cx - 18.0f, cy + 6.0f);
        cloud.cubicTo (cx - 26.0f, cy + 6.0f,  cx - 27.0f, cy - 5.0f,  cx - 17.0f, cy - 7.0f);
        cloud.cubicTo (cx - 16.0f, cy - 17.0f, cx - 4.0f,  cy - 20.0f, cx + 2.0f,  cy - 13.0f);
        cloud.cubicTo (cx + 8.0f,  cy - 18.0f, cx + 19.0f, cy - 13.0f, cx + 18.0f, cy - 5.0f);
        cloud.cubicTo (cx + 26.0f, cy - 3.0f,  cx + 25.0f, cy + 6.0f,  cx + 16.0f, cy + 6.0f);
        cloud.closeSubPath();

        // Atmospheric radial aura behind the cloud knob (intensifies with volume)
        const float auraRadius = 28.0f + (normValue * 18.0f);
        juce::ColourGradient cloudAura (juce::Colours::white.withAlpha (0.35f + 0.30f * normValue), cx, cy - 4.0f,
                                        juce::Colours::transparentWhite, cx + auraRadius, cy - 4.0f, true);
        g.setGradientFill (cloudAura);
        g.fillEllipse (cx - auraRadius, (cy - 4.0f) - auraRadius, auraRadius * 2.0f, auraRadius * 2.0f);

        // Soft diffused drop shadow
        g.setColour (juce::Colour (75, 85, 140).withAlpha (0.22f));
        g.fillPath (cloud, juce::AffineTransform::translation (0.0f, 3.0f));

        // Cloud body with subtle vertical sky-to-white shading
        juce::ColourGradient cloudFill (juce::Colours::white, cx, cy - 18.0f,
                                        juce::Colour (240, 246, 255), cx, cy + 8.0f, false);
        g.setGradientFill (cloudFill);
        g.fillPath (cloud);

        // Clean outer rim highlight (only strokes the unified perimeter)
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        g.strokePath (cloud, juce::PathStrokeType (1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
};

class VolumeCloudAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VolumeCloudAudioProcessorEditor (VolumeCloudAudioProcessor&);
    ~VolumeCloudAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    VolumeCloudAudioProcessor& audioProcessor;
    CottonCandyLookAndFeel cottonCandyLnF;
    juce::Slider volumeSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumeCloudAudioProcessorEditor)
};
