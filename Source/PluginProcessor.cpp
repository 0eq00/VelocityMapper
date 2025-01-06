/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VelocityMapperAudioProcessor::VelocityMapperAudioProcessor()
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
    addParameter(factor = new juce::AudioParameterFloat("factor", "Factor", 0, 2, 0.1));
    *factor = 1;
    addParameter(center = new juce::AudioParameterInt("center", "Center", 1, 127, 1));
    *center = 64;
    addParameter(lokey = new juce::AudioParameterInt("lokey", "LoKey", 0, 127, 1));
    *lokey = 0;
    addParameter(hikey = new juce::AudioParameterInt("hikey", "HiKey", 0, 127, 1));
    *hikey = 127;

    fileName = "";
    title = "Default";
    for (int i = 0; i < 128; i++)
    {
        map[i] = i;
        save[i] = i;
    }

    lastIn = 1;
    lastOut = 1;
}

VelocityMapperAudioProcessor::~VelocityMapperAudioProcessor()
{
}

//==============================================================================
const juce::String VelocityMapperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VelocityMapperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VelocityMapperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VelocityMapperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VelocityMapperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VelocityMapperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VelocityMapperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VelocityMapperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VelocityMapperAudioProcessor::getProgramName (int index)
{
    return {};
}

void VelocityMapperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VelocityMapperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VelocityMapperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VelocityMapperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VelocityMapperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    juce::MidiBuffer processedMidi;

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        const auto time = metadata.samplePosition;

        if (message.isNoteOn())
        {
            int key = message.getNoteNumber();
            if ( key >= *(this->lokey) && key <= *(this->hikey) )
            {
                this->lastIn = message.getVelocity();
                this->lastOut = this->map[this->lastIn];
                message = juce::MidiMessage::noteOn(message.getChannel(), message.getNoteNumber(), (juce::uint8)this->lastOut);
            }
        }

        processedMidi.addEvent( message, time );
    }

    midiMessages.swapWith(processedMidi);
/*
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
*/
}

//==============================================================================
bool VelocityMapperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VelocityMapperAudioProcessor::createEditor()
{
    return new VelocityMapperAudioProcessorEditor (*this);
}

//==============================================================================
void VelocityMapperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

//  juce::MemoryOutputStream(destData, true).writeString(this->fileName);
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("0eq00:VelocityMapper"));
    xml->setAttribute("factor", (double)(*this->factor));
    xml->setAttribute("center", (int)(*this->center));
    xml->setAttribute("lokey", (int)(*this->lokey));
    xml->setAttribute("hikey", (int)(*this->hikey));
    xml->setAttribute("filename", this->fileName );
    copyXmlToBinary(*xml, destData);
}

void VelocityMapperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

//  this->fileName = juce::MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readString();
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("0eq00:VelocityMapper"))
        {
            *this->factor = xmlState->getDoubleAttribute("factor");
            *this->center = xmlState->getIntAttribute("center");
            *this->lokey = xmlState->getIntAttribute("lokey");
            *this->hikey = xmlState->getIntAttribute("hikey");
            this->fileName = xmlState->getStringAttribute("filename");
        }
    }

    this->loadFile();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VelocityMapperAudioProcessor();
}

void VelocityMapperAudioProcessor::loadFile()
{
    auto fileToRead = juce::File::File(this->fileName);

    if (!fileToRead.existsAsFile())
        return;  // file doesn't exist

    juce::FileInputStream inputStream(fileToRead);

    if (!inputStream.openedOk())
        return;  // failed to open

    int i = 0;
    while (!inputStream.isExhausted())
    {
        auto line = inputStream.readNextLine();

        if (i == 0)
            this->title = line;
        else if (i > 127)
            break;
        else
            this->map[i] = line.getIntValue();

        i++;
    }

    for (int i = 0; i < 128; i++)
        this->save[i] = this->map[i];

    this->compute();
}

void VelocityMapperAudioProcessor::compute()
{
    int c = *(this->center);
    float f = this->save[c] *  (*(this->factor)) - this->save[c];

    for (int i = 1; i < 128; i++)
    {
        float r = 0;
        if (i <= c)
            r = juce::MathConstants<double>::pi / 2 / (c - 1) * (i - 1);
        else
            r = juce::MathConstants<double>::pi / 2 + juce::MathConstants<double>::pi / 2 / (127 - c) * (i - c);

        this->map[i] = std::floor(this->save[i] + sin(r) * f);
        if (this->map[i] < 1)
            this->map[i] = 1;
        if (this->map[i] > 127)
            this->map[i] = 127;
    }
}