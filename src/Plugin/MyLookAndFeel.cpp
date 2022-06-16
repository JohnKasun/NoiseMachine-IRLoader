#include "MyLookAndFeel.h"

MyLookAndFeel::MyLookAndFeel()
{
}

MyLookAndFeel::~MyLookAndFeel()
{
}

void MyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
	float radius = juce::jmin(width / 2.0f, height / 2.0f) - 25.0f;
	float centerX = x + width * 0.5f;
	float centerY = y + height * 0.5f;
	float rx = centerX - radius;
	float ry = centerY - radius;
	float rw = radius * 2.0f;
	float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	g.setColour(juce::Colours::grey);
	g.fillEllipse(rx, ry, rw, rw);
	g.setColour(juce::Colours::black);
	g.drawEllipse(rx, ry, rw, rw, 5.0f);

	juce::Path path;
	float pointerLength = radius * 0.33f;
	float pointerThickness = 5.0f;
	path.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
	path.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
	g.setColour(juce::Colours::black);
	g.fillPath(path);

	g.setColour(juce::Colours::whitesmoke);
	g.drawText(slider.getName(), rx, ry, rw, rw, juce::Justification::centred);
}
