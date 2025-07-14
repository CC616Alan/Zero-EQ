/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//class ZeroEQAudioProcessorEditor;

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::black);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(2);
        
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // fill
        g.setColour(juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);
        // outline
        g.setColour(juce::Colours::grey);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);
        
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
 
        // Draw value text
        juce::String text;
        auto paramID = slider.getName();
        auto value = slider.getValue();
        
        if (paramID == "Peak Freq" || paramID == "LowCut Freq" || paramID == "HighCut Freq")
        {
            text = juce::String::formatted("%.0f Hz", value);
        }
        else if (paramID == "Peak Gain")
        {
            text = juce::String::formatted("%.1f dB", value);
            //DBG("Peak Gain Text: " + text);  // 调试输出
        }
        else if (paramID == "Peak Quality")
        {
            text = juce::String::formatted("%.2f", value);
        }
        else if (paramID == "LowCut Slope" || paramID == "HighCut Slope")
        {
            int slopeValue = static_cast<int>(value);
            text = juce::String(12 + slopeValue * 12) + " dB/Oct";
        }
        else
        {
            text = juce::String(value);
        }
        
       /*
        auto value = slider.getValue();
        auto minValue = slider.getMinimum();
        auto maxValue = slider.getMaximum();
        auto text = juce::String::formatted("%.2f\nMin: %.2f\nMax: %.2f", value, minValue, maxValue);
       */
        
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText(text, bounds.reduced(10), juce::Justification::centredBottom, true);
        
        // 调试：显示文本区域边界（临时启用）
        //g.setColour(juce::Colours::red);
        //g.drawRect(bounds, 1.0f);
    }
};

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
//        getLookAndFeel().setColour (juce::Slider::thumbColourId, juce::Colours::white);
        setLookAndFeel(&lookAndFeel);
    }
    

    ~CustomRotarySlider()
    {
        setLookAndFeel(nullptr);
    }

private:
    CustomLookAndFeel lookAndFeel;
 
};
 
struct ResponseCurveComponent: juce::Component,
juce::AudioProcessorParameter::Listener,
juce::Timer
{
    ResponseCurveComponent(ZeroEQAudioProcessor&);
    ~ResponseCurveComponent();
    void parameterValueChanged (int parameterIndex, float newValue) override;
    
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { }
    
    void timerCallback() override;
    
    void paint(juce::Graphics& g) override;
    
    private:
    ZeroEQAudioProcessor& audioProcessor;
    juce::Atomic<bool> parametersChanged { false };
    
    MonoChain monoChain;
};

//==============================================================================
/**
*/
class ZeroEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ZeroEQAudioProcessorEditor (ZeroEQAudioProcessor&);
    ~ZeroEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ZeroEQAudioProcessor& audioProcessor;
    
    CustomRotarySlider peakFreqSlider, peakGainSlider, peakQualitySlider, lowCutFreqSlider, highCutFreqSlider, lowCutSlopeSlider, highCutSlopeSlider;
    
    ResponseCurveComponent responseCurveComponent;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment peakFreqSliderAttachment, peakGainSliderAttachment, peakQualitySliderAttachment, lowCutFreqSliderAttachment, highCutFreqSliderAttachment, lowCutSlopeSliderAttachment, highCutSlopeSliderAttachment;
    
    std::vector<juce::Component*> getComps();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZeroEQAudioProcessorEditor)
};
