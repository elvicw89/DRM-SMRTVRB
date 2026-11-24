#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DRMFocusableSlider : public juce::Slider
{
public:
};

// Mix has no enable switch in the UI. The existing advMixOn parameter is engaged when the
// user moves the knob and cleared by a double-click reset, so presets and state are unchanged.
class DRMMixSlider final : public DRMFocusableSlider
{
public:
    std::function<void()> onDisengage;

    bool isUserChange() const
    {
        if (resetting) return false;
        if (userEditing || isMouseButtonDown(true)) return true;
        auto* focused = juce::Component::getCurrentlyFocusedComponent();
        return focused != nullptr && isParentOf(focused);
    }

    void mouseDoubleClick(const juce::MouseEvent& event) override
    {
        resetting = true;
        if (onDisengage) onDisengage();
        DRMFocusableSlider::mouseDoubleClick(event);
        resetting = false;
    }

    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override
    {
        userEditing = true;
        DRMFocusableSlider::mouseWheelMove(event, wheel);
        userEditing = false;
    }

    bool keyPressed(const juce::KeyPress& key) override
    {
        userEditing = true;
        const bool handled = DRMFocusableSlider::keyPressed(key);
        userEditing = false;
        return handled;
    }

private:
    bool userEditing = false;
    bool resetting = false;
};

class DRMLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    DRMLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float startAngle, float endAngle, juce::Slider&) override;
    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider&) override;
    juce::Label* createSliderTextBox(juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool highlighted, bool down) override;
    void drawComboBox(juce::Graphics&, int width, int height, bool isDown, int bx, int by, int bw, int bh,
                      juce::ComboBox&) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
};

class DRMsmrtvrbAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                               private juce::Timer
{
public:
    explicit DRMsmrtvrbAudioProcessorEditor(DRMsmrtvrbAudioProcessor& pluginProcessor);
    ~DRMsmrtvrbAudioProcessorEditor() override;
    void paint(juce::Graphics& graphics) override;
    void resized() override;

private:
    void timerCallback() override;
    void styleSlider(juce::Slider& slider, bool rotary);
    void addLabel(juce::Label& label, const juce::String& text);
    void selectPreset();
    void renderBackdrop();

    DRMsmrtvrbAudioProcessor& pluginProcessor;
    DRMLookAndFeel lookAndFeel;
    juce::ComboBox preset, target, character, sync, division;
    juce::TextButton tapButton { "TAP" };
    DRMFocusableSlider space, bpm, freeDecay, preDelay, tone, width, tail, outLevel;
    DRMMixSlider mix;
    juce::ToggleButton preDelayOn { "Pre-delay" }, toneOn { "Tone" }, widthOn { "Width" }, tailOn { "Tail" };
    juce::Label title, brand, spaceLabel, bpmLabel, syncLabel, decayLabel, mixLabel, outLevelLabel;
    juce::Label targetLabel, characterLabel, presetLabel, presetStatus, divisionLabel;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 4> comboAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 7> sliderAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tailAttachment;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> buttonAttachments;
    juce::Rectangle<int> headerPlate, statusBox, sourcePanel, spacePanel, outputPanel, advancedPanel, levelPanel, meterBox, envelopeBox;
    juce::Image backdrop;
    bool presetModified = false;
    std::array<float, 4> meterLevels {};
    std::array<float, 4> meterPeakMarkers {};
    std::array<int, 4> meterPeakHoldTicks {};
    std::array<float, DRMsmrtvrbAudioProcessor::activityHistorySize> activitySnapshot {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DRMsmrtvrbAudioProcessorEditor)
};
