#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processorRef(p), mValueTreeState(vts)
{

    addAndMakeVisible(mLoadButton);
    mLoadButton.onClick = [this]() {
        openIrLoader();
    };

    addAndMakeVisible(mClearButton);
    mClearButton.onClick = [this]() {
        clearIr();
    };
    mClearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    mClearButton.setButtonText("Clear");
    
    addAndMakeVisible(mGainSlider);
    mGainSliderAttachment.reset(new SliderAttachment(mValueTreeState, "gain", mGainSlider));
    mGainSlider.setName("Gain");
    mGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mGainSlider.setLookAndFeel(&mMyLookAndFeel);
    mGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mGainLabel);
    mGainLabel.attachToComponent(&mGainSlider, true);
    mGainLabel.setText("Gain", juce::dontSendNotification);
    
    startTimer(50);
    processorRef.setAudioProcessErrorCallback([this](juce::String message) { showErrorWindow(message); });
    setSize(buttonWidth, buttonHeight * 1.5 + buttonHeight);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    auto gainArea = area.removeFromBottom(buttonHeight);
    mGainLabel.setBounds(gainArea.removeFromLeft(buttonWidth / 8));
    mGainSlider.setBounds(gainArea);
    
    mLoadButton.setBounds(area.removeFromTop(buttonHeight));
    mClearButton.setBounds(area);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    updateIrState();
}

void AudioPluginAudioProcessorEditor::showErrorWindow(juce::String errorMessage)
{
    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", errorMessage);
}

void AudioPluginAudioProcessorEditor::openIrLoader()
{
    mIrLoaderWindow.reset(new juce::FileChooser("Select an IR to load", juce::File::getCurrentWorkingDirectory(), "*.wav"));
    mIrLoaderWindow->launchAsync(juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode, 
        [this](const juce::FileChooser& chooser) {
            juce::File file = chooser.getResult();
            if (file.exists()) {
                processorRef.loadIr(file);
            }
        });
}

void AudioPluginAudioProcessorEditor::clearIr()
{
    if (processorRef.isIrLoaded()) {
        processorRef.clearIr();
    }
}

void AudioPluginAudioProcessorEditor::updateIrState()
{
    if (processorRef.isIrLoaded()) {
        mLoadButton.setButtonText("IR Loaded!");
        mLoadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        mLoadButton.setEnabled(false);
        mClearButton.setEnabled(true);
    }
    else {
        mLoadButton.setButtonText("Load IR");
        mLoadButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
        mLoadButton.setEnabled(true);
        mClearButton.setEnabled(false);
    }
}
