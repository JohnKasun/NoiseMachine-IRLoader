#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    juce::ignoreUnused(processorRef);

    addAndMakeVisible(mLoadButton);
    mLoadButton.setButtonText("Load IR");
    mLoadButton.onClick = [this]() {
        openIrLoader();
    };

    setSize(buttonWidth, buttonHeight);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    mLoadButton.setBounds(area);
}

void AudioPluginAudioProcessorEditor::openIrLoader()
{
    mIrLoaderWindow.reset(new juce::FileChooser("Select an IR to load", juce::File::getCurrentWorkingDirectory(), "*.wav"));
    mIrLoaderWindow->launchAsync(juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode, 
        [this](const juce::FileChooser& chooser) {
            juce::File file = chooser.getResult();
            if (file.exists()) {
                processorRef.requestLoadIr(file);
            }
        });
}
