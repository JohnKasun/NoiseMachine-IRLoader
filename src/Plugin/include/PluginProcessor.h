#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <functional>

#include "ErrorDef.h"
#include "Convolver.h"

//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor
{
public:

    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void loadIr(juce::File irFile);
    void clearIr();
    bool isIrLoaded() const;
    void setAudioProcessErrorCallback(std::function<void(juce::String)> callback);

private:

    std::function<void(juce::String)> onAudioProcessorError;
    juce::AudioFormatManager mFormatManager;
    std::unique_ptr<juce::AudioSampleBuffer> mIrBuffer;
    std::vector<std::unique_ptr<Convolver>> mConvolver;
    std::unique_ptr<float> mTempOutputBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};