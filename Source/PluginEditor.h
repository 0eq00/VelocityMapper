/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
/*
class VelocityMapperAudioProcessorEditor : public juce::AudioProcessorEditor
*/
class VelocityMapperAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::Timer

{
public:
    VelocityMapperAudioProcessorEditor (VelocityMapperAudioProcessor&);
    ~VelocityMapperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;
    void openButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VelocityMapperAudioProcessor& audioProcessor;

    juce::TextButton openButton{ "Open" };
    std::unique_ptr<juce::FileChooser> chooser;
    juce::Label msgLabel;
    juce::Label nameLabel;
    juce::Slider factorSlider;
    juce::Label factorLabel;
    juce::Slider centerSlider;
    juce::Label lokeyLabel;
    juce::Slider lokeySlider;
    juce::Label hikeyLabel;
    juce::Slider hikeySlider;
    juce::Label centerLabel;
    juce::Label titleLabel;
    juce::TextEditor textContent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VelocityMapperAudioProcessorEditor)
};
