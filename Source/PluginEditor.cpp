/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VelocityMapperAudioProcessorEditor::VelocityMapperAudioProcessorEditor (VelocityMapperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize( 800, 660 );

    startTimerHz(60);

    openButton.setBounds(30, 20, 60, 20);
    addAndMakeVisible(openButton);
    openButton.onClick = [this] { openButtonClicked(); };

    nameLabel.setText("filename", juce::dontSendNotification);
    nameLabel.setBounds(110, 20, 500, 20);
    addAndMakeVisible(nameLabel);

    factorLabel.setText("Factor", juce::dontSendNotification);
    factorLabel.setBounds(30, 60, 60, 20);
    addAndMakeVisible(&factorLabel);

    factorSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    factorSlider.setRange(0, 2, 0.1);
    factorSlider.setBounds(110, 60, 250, 20);
    factorSlider.setValue(*(audioProcessor.factor));
    factorSlider.addListener(this);
    addAndMakeVisible(&factorSlider);

    centerLabel.setText("Center", juce::dontSendNotification);
    centerLabel.setBounds(30, 100, 60, 20);
    addAndMakeVisible(&centerLabel);

    centerSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    centerSlider.setRange(1, 127, 1);
    centerSlider.setBounds(110, 100, 250, 20);
    centerSlider.setValue(*(audioProcessor.center));
    centerSlider.addListener(this);
    addAndMakeVisible(&centerSlider);

    lokeyLabel.setText("Low Key", juce::dontSendNotification);
    lokeyLabel.setBounds(390, 60, 60, 20);
    addAndMakeVisible(&lokeyLabel);

    lokeySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lokeySlider.setRange(0, 127, 1);
    lokeySlider.setBounds(470, 60, 250, 20);
    lokeySlider.setValue(*(audioProcessor.lokey));
    lokeySlider.addListener(this);
    addAndMakeVisible(&lokeySlider);

    hikeyLabel.setText("High Key", juce::dontSendNotification);
    hikeyLabel.setBounds(390, 100, 60, 20);
    addAndMakeVisible(&hikeyLabel);

    hikeySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    hikeySlider.setRange(0, 127, 1);
    hikeySlider.setBounds(470, 100, 250, 20);
    hikeySlider.setValue(*(audioProcessor.hikey));
    hikeySlider.addListener(this);
    addAndMakeVisible(&hikeySlider);

    titleLabel.setText("title", juce::dontSendNotification);
    titleLabel.setBounds(30, 140, 500, 20);
    addAndMakeVisible(titleLabel);

    textContent.setBounds(470, 140, 250, 420);
    textContent.setMultiLine(true,false);
    textContent.setReadOnly(true);
    textContent.setCaretVisible(false);
    addAndMakeVisible(textContent);

    msgLabel.setText("message", juce::dontSendNotification);
    msgLabel.setBounds(30, 600, 250, 20);
    addAndMakeVisible(msgLabel);
}

VelocityMapperAudioProcessorEditor::~VelocityMapperAudioProcessorEditor()
{
}

//==============================================================================
void VelocityMapperAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
/*
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
*/

    g.setColour(juce::Colours::lightslategrey);
    for ( int i = 1; i < 128; i++)
    {
        g.fillEllipse(50 + (i*3), 560 - (audioProcessor.map[i]*3), 4, 4);
    }
    g.setColour(juce::Colours::red);
    g.fillEllipse(50 + (audioProcessor.lastIn*3), 560 - (audioProcessor.lastOut*3), 5, 5);
}

void VelocityMapperAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void VelocityMapperAudioProcessorEditor::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File{}, "*.txt");

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync( chooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file != juce::File{})
        {
            if (!file.existsAsFile()) // [1]
                return;

            audioProcessor.fileName = file.getFullPathName();
            audioProcessor.loadFile();
        }
    });
}

void VelocityMapperAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    *(audioProcessor.factor) = (float)(factorSlider.getValue());
    *(audioProcessor.center) = (int)(centerSlider.getValue());
    *(audioProcessor.lokey) = (int)(lokeySlider.getValue());
    *(audioProcessor.hikey) = (int)(hikeySlider.getValue());

    audioProcessor.compute();
}

void VelocityMapperAudioProcessorEditor::timerCallback()
{
    msgLabel.setText("in[" + (juce::String)(audioProcessor.lastIn) + "] out[" + (juce::String)(audioProcessor.lastOut) + "]", juce::dontSendNotification);
    nameLabel.setText(audioProcessor.fileName, juce::dontSendNotification);
    titleLabel.setText(audioProcessor.title, juce::dontSendNotification);

    juce::String contents = audioProcessor.title + "\n";
    for (int i = 1; i < 128; i++)
    {
        contents += juce::String( audioProcessor.map[i] ) + "\n";
    }
    textContent.setText(contents);

    this->repaint();
}
