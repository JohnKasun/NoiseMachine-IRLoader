#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    )
{
    mFormatManager.registerBasicFormats();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    clearIr();
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name;}
bool AudioPluginAudioProcessor::acceptsMidi() const{return false;}
bool AudioPluginAudioProcessor::producesMidi() const{return false;}
bool AudioPluginAudioProcessor::isMidiEffect() const { return false;}
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String AudioPluginAudioProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
bool AudioPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() { return new AudioPluginAudioProcessorEditor(*this); }
void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }
//==============================================================================

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return mConvolver.at(0).getTailLength() / mSampleRate;
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    mTempOutputBufferSize = samplesPerBlock * 2;
    mTempOutputBuffer.reset(new float[mTempOutputBufferSize]{});
}

void AudioPluginAudioProcessor::releaseResources()
{
    mSampleRate = 44100.0;
    mTempOutputBufferSize = 0.0f;
    mTempOutputBuffer.reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannels() < layouts.getMainInputChannels())
        return false;

    return true;
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto inputBuffer = getBusBuffer(buffer, true, 0);

    for (int c = 0; c < inputBuffer.getNumChannels(); c++) {
        mConvolver[c].process(buffer.getReadPointer(c), mTempOutputBuffer.get(), buffer.getNumSamples());
        buffer.addFrom(c, 0, mTempOutputBuffer.get(), buffer.getNumSamples());
    }
    for (int c = getTotalNumInputChannels(); c < getTotalNumOutputChannels(); c++) {
        buffer.copyFrom(c, 0, buffer.getReadPointer(0), buffer.getNumSamples());
    }

}
//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    
}

void AudioPluginAudioProcessor::loadIr(juce::File irFile)
{
    auto* reader = mFormatManager.createReaderFor(irFile);
    if (reader != nullptr) {
        suspendProcessing(true);
        auto lengthInSeconds = reader->lengthInSamples / reader->sampleRate;
        if (lengthInSeconds > 5) {
            onAudioProcessorError("IR must be under 5 seconds");
        }
        else {
            auto irBuffer = juce::AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
            reader->read(&irBuffer, 0, reader->lengthInSamples, 0, true, true);
            switch (irBuffer.getNumChannels()) {
            case 1:
                mConvolver.at(0).init(irBuffer.getWritePointer(0), irBuffer.getNumSamples());
                mConvolver.at(1).init(irBuffer.getWritePointer(0), irBuffer.getNumSamples());
                mIrLoaded = true;
                break;
            case 2:
                mConvolver.at(0).init(irBuffer.getWritePointer(0), irBuffer.getNumSamples());
                mConvolver.at(1).init(irBuffer.getWritePointer(1), irBuffer.getNumSamples());
                mIrLoaded = true;
                break;
            default:
                onAudioProcessorError("IR must be stereo or mono...");
                mIrLoaded = false;
            }
        }
        suspendProcessing(false);
        delete reader;
    }
    else {
        onAudioProcessorError("IR file can't be opened");
    }
}

void AudioPluginAudioProcessor::clearIr()
{
    suspendProcessing(true);
    for (int c = 0; c < mConvolver.size(); c++) {
        mConvolver.at(c).reset();
    }
    mIrLoaded = false;
    if (mTempOutputBuffer) {
        CVectorFloat::setZero(mTempOutputBuffer.get(), mTempOutputBufferSize);
    }
    suspendProcessing(false);
}

bool AudioPluginAudioProcessor::isIrLoaded() const
{
    return mIrLoaded;
}

void AudioPluginAudioProcessor::setAudioProcessErrorCallback(std::function<void(juce::String)> callback)
{
    onAudioProcessorError = callback;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}