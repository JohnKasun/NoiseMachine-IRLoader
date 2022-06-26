#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
{
    mFormatManager.registerBasicFormats();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mTempOutputBuffer.reset(new float[samplesPerBlock * 2]{});
}

void AudioPluginAudioProcessor::releaseResources()
{
    mConvolver.clear();
    mIrBuffer.reset();
    mIrState = irEmpty;
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    if (!mConvolver.empty()) {
        for (int c = 0; c < getTotalNumInputChannels(); c++) {
            mConvolver[c]->process(buffer.getReadPointer(c), mTempOutputBuffer.get(), buffer.getNumSamples());
            buffer.addFrom(c, 0, mTempOutputBuffer.get(), buffer.getNumSamples());
        }
    }

}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    
}

juce::String AudioPluginAudioProcessor::loadIr(juce::File irFile)
{
    auto* reader = mFormatManager.createReaderFor(irFile);
    if (reader != nullptr) {
        if (reader->numChannels != getTotalNumInputChannels()) {
            delete reader;
            return juce::String("Channels Mismatch");
        }

        suspendProcessing(true);
        releaseResources();
        mIrBuffer.reset(new juce::AudioSampleBuffer(reader->numChannels, reader->lengthInSamples));
        reader->read(mIrBuffer.get(), 0, reader->lengthInSamples, 0, true, true);
        initConvolver(); 
        mIrState = irLoaded;
        suspendProcessing(false);
        delete reader;
        return juce::String("Success");
    }
    return juce::String("File can't be opened");
}

void AudioPluginAudioProcessor::clearIr()
{
    suspendProcessing(true);
    releaseResources();
    suspendProcessing(false);
}

AudioPluginAudioProcessor::IrState AudioPluginAudioProcessor::getIrState() const
{
    return mIrState;
}

void AudioPluginAudioProcessor::initConvolver()
{
    mConvolver.clear();
    for (int c = 0; c < mIrBuffer->getNumChannels(); c++) {
        mConvolver.emplace_back(new Convolver());
        mConvolver[c]->init(mIrBuffer->getWritePointer(c), mIrBuffer->getNumSamples());
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}