#pragma once

#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:

    enum Dimen_t {
        buttonWidth = 300,
        buttonHeight = 75
    };

    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    MyLookAndFeel mMyLookAndFeel;
    AudioPluginAudioProcessor& processorRef;

    juce::TextButton mLoadButton;
    juce::TextButton mClearButton;

    std::unique_ptr<juce::FileChooser> mIrLoaderWindow;
    void timerCallback() override;
    void showErrorWindow(juce::String errorMessage);
    void openIrLoader();
    void clearIr();
    void updateIrState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};