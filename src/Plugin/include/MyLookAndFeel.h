#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class MyLookAndFeel : public juce::LookAndFeel_V4 {
public:
	MyLookAndFeel();
	~MyLookAndFeel();
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
private:
};