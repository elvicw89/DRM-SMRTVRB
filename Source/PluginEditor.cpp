#include "PluginEditor.h"

namespace
{
const juce::Colour chassis { 0xff1b1d1f };
const juce::Colour panel { 0xff25282b };
const juce::Colour recessed { 0xff131517 };
const juce::Colour border { 0xff464b4f };
const juce::Colour ivory { 0xffe8e1ca };
const juce::Colour muted { 0xff9f9a8c };
const juce::Colour amber { 0xffe0a040 };
const juce::Colour inactive { 0xff6d7173 };
const juce::Colour meterInput { 0xff71836b };

juce::Font plain(float size, bool bold = false)
{
    return juce::Font(juce::FontOptions(size, bold ? juce::Font::bold : juce::Font::plain));
}

void drawCaption(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text)
{
    g.setColour(muted);
    g.setFont(plain(10.5f, true));
    g.drawText(text, area.getX() + 10, area.getY() + 5, area.getWidth() - 20, 14, juce::Justification::centredLeft);
    g.setColour(border.withAlpha(0.7f));
    g.drawHorizontalLine(area.getY() + 21, static_cast<float>(area.getX() + 10), static_cast<float>(area.getRight() - 10));
}

void drawPanel(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& caption)
{
    const auto r = area.toFloat();
    g.setColour(recessed);
    g.fillRoundedRectangle(r, 5.0f);
    g.setColour(border);
    g.drawRoundedRectangle(r.reduced(0.5f), 5.0f, 1.0f);
    drawCaption(g, area, caption);
}

void drawScrew(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colour(0xff0d0e0f));
    g.fillEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f);
    g.setColour(border);
    g.drawEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f, 1.0f);
    g.drawLine(x - 2.5f, y + 1.5f, x + 2.5f, y - 1.5f, 1.0f);
}
}

DRMLookAndFeel::DRMLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, recessed);
    setColour(juce::ComboBox::textColourId, ivory);
    setColour(juce::ComboBox::outlineColourId, border);
    setColour(juce::ComboBox::arrowColourId, amber);
    setColour(juce::PopupMenu::backgroundColourId, recessed);
    setColour(juce::PopupMenu::textColourId, ivory);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, amber.withAlpha(0.85f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0xff17181a));
    setColour(juce::Slider::rotarySliderFillColourId, amber);
    setColour(juce::Slider::rotarySliderOutlineColourId, border);
    setColour(juce::Slider::textBoxTextColourId, ivory);
    setColour(juce::Slider::textBoxBackgroundColourId, recessed);
    setColour(juce::Slider::textBoxOutlineColourId, border);
    setColour(juce::TextButton::buttonColourId, panel);
    setColour(juce::TextButton::textColourOffId, amber);
    setColour(juce::TextButton::textColourOnId, amber);
    setColour(juce::Label::textColourId, muted);
    setColour(juce::TooltipWindow::backgroundColourId, recessed);
    setColour(juce::TooltipWindow::textColourId, ivory);
}

void DRMLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                      float startAngle, float endAngle, juce::Slider& slider)
{
    const bool enabled = slider.isEnabled();
    const float alpha = enabled ? 1.0f : 0.45f;
    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                               static_cast<float>(width), static_cast<float>(height)).reduced(2.0f);
    const float diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
    const auto centre = bounds.getCentre();
    const float tickOuter = diameter * 0.5f;
    const float ringRadius = tickOuter - juce::jmax(4.0f, diameter * 0.075f);
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    // Scale ticks every 10% of travel.
    g.setColour(muted.withAlpha(0.55f * alpha));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * static_cast<float>(i) / 10.0f;
        const float inner = ringRadius + 2.5f;
        g.drawLine(centre.x + inner * std::sin(a), centre.y - inner * std::cos(a),
                   centre.x + tickOuter * std::sin(a), centre.y - tickOuter * std::cos(a),
                   i % 5 == 0 ? 1.6f : 1.0f);
    }

    const float stroke = juce::jmax(2.5f, diameter * 0.04f);
    juce::Path track;
    track.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0xff34383b).withAlpha(alpha));
    g.strokePath(track, juce::PathStrokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    juce::Path value;
    value.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0f, startAngle, angle, true);
    g.setColour((enabled ? amber : inactive).withAlpha(alpha));
    g.strokePath(value, juce::PathStrokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float capRadius = ringRadius - stroke - 3.0f;
    juce::ColourGradient cap(juce::Colour(0xff565b5f), centre.x - capRadius * 0.5f, centre.y - capRadius * 0.7f,
                             juce::Colour(0xff1f2123), centre.x + capRadius * 0.6f, centre.y + capRadius * 0.8f, false);
    g.setGradientFill(cap);
    g.fillEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    g.setColour(juce::Colour(0xff0b0c0d));
    g.drawEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.5f);
    g.setColour(juce::Colour(0xff6b7175).withAlpha(0.5f * alpha));
    g.drawEllipse(centre.x - capRadius + 2.0f, centre.y - capRadius + 2.0f,
                  capRadius * 2.0f - 4.0f, capRadius * 2.0f - 4.0f, 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle(-1.4f, -capRadius * 0.88f, 2.8f, capRadius * 0.52f, 1.2f);
    g.setColour((enabled ? ivory : muted).withAlpha(alpha));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
}

juce::Label* DRMLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* label = juce::LookAndFeel_V4::createSliderTextBox(slider);
    label->setFont(plain(12.0f, true));
    label->setJustificationType(juce::Justification::centred);
    return label;
}

void DRMLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool, bool)
{
    const bool enabled = button.isEnabled();
    const bool on = button.getToggleState();
    const float alpha = enabled ? 1.0f : 0.45f;
    const auto bounds = button.getLocalBounds().toFloat();

    g.setColour((on && enabled ? ivory : muted).withAlpha(alpha));
    g.setFont(plain(10.5f, true));
    g.drawText(button.getButtonText().toUpperCase(), bounds.withHeight(14.0f).toNearestInt(), juce::Justification::centred);

    const float w = juce::jmin(52.0f, bounds.getWidth() - 4.0f);
    const auto track = juce::Rectangle<float>(w, 20.0f).withCentre({ bounds.getCentreX(), bounds.getY() + 14.0f + 4.0f + 10.0f });
    g.setColour(recessed.withAlpha(alpha));
    g.fillRoundedRectangle(track, 4.0f);
    g.setColour((on ? amber : border).withAlpha(alpha));
    g.drawRoundedRectangle(track.reduced(0.5f), 4.0f, 1.0f);

    const float thumbW = track.getWidth() * 0.46f;
    const auto thumb = juce::Rectangle<float>(thumbW, track.getHeight() - 6.0f)
        .withCentre({ on ? track.getRight() - thumbW * 0.5f - 3.0f : track.getX() + thumbW * 0.5f + 3.0f, track.getCentreY() });
    g.setColour((on ? amber : juce::Colour(0xff4b5053)).withAlpha(alpha));
    g.fillRoundedRectangle(thumb, 3.0f);

    g.setFont(plain(9.5f, true));
    g.setColour((on ? amber : muted).withAlpha(alpha));
    const auto textArea = on ? track.withWidth(track.getWidth() - thumbW - 3.0f)
                             : track.withTrimmedLeft(thumbW + 3.0f);
    g.drawText(on ? "ON" : "OFF", textArea.toNearestInt(), juce::Justification::centred);

    if (button.hasKeyboardFocus(true))
    {
        g.setColour(amber);
        g.drawRoundedRectangle(track.expanded(2.0f), 5.0f, 1.5f);
    }
}

void DRMLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                          bool highlighted, bool down)
{
    const auto r = button.getLocalBounds().toFloat().reduced(0.5f);
    juce::ColourGradient face(juce::Colour(down ? 0xff202325 : 0xff3a3e41), 0.0f, r.getY(),
                              juce::Colour(down ? 0xff2d3134 : 0xff25282b), 0.0f, r.getBottom(), false);
    g.setGradientFill(face);
    g.fillRoundedRectangle(r, 4.0f);
    g.setColour(highlighted && button.isEnabled() ? amber : border);
    g.drawRoundedRectangle(r, 4.0f, 1.0f);
}

void DRMLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int,
                                  juce::ComboBox& box)
{
    const auto r = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)).reduced(0.5f);
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(r, 3.0f);
    g.setColour(box.hasKeyboardFocus(true) ? amber : box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(r, 3.0f, box.hasKeyboardFocus(true) ? 1.5f : 1.0f);

    const float cx = static_cast<float>(width) - 13.0f;
    const float cy = static_cast<float>(height) * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(cx - 4.0f, cy - 2.0f, cx + 4.0f, cy - 2.0f, cx, cy + 3.0f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha(box.isEnabled() ? 1.0f : 0.4f));
    g.fillPath(arrow);
}

juce::Font DRMLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return plain(13.0f, true);
}

juce::Font DRMLookAndFeel::getTextButtonFont(juce::TextButton&, int)
{
    return plain(12.0f, true);
}

DRMsmrtvrbAudioProcessorEditor::DRMsmrtvrbAudioProcessorEditor(DRMsmrtvrbAudioProcessor& p)
    : AudioProcessorEditor(&p), pluginProcessor(p)
{
    setLookAndFeel(&lookAndFeel);
    setResizable(true, true);
    setResizeLimits(640, 420, 1040, 700);

    for (auto* combo : { &preset, &target, &character, &sync, &division })
        addAndMakeVisible(*combo);
    preset.setTitle("Factory Preset");
    target.setTitle("Source");
    character.setTitle("Character");
    sync.setTitle("Tempo Sync");
    division.setTitle("Tempo Division");

    target.addItemList({ "Shells", "Snare", "Room", "Overheads" }, 1);
    character.addItemList({ "Room", "Spring", "Gated" }, 1);
    sync.addItemList({ "Free", "Host", "Tap" }, 1);
    division.addItemList({ "1/16", "1/8", "1/4", "1/2", "1 bar", "2 bars" }, 1);

    std::size_t presetCount = 0;
    const auto* presets = drmspace::factoryPresets(presetCount);
    preset.addItem("Factory Preset", 1);
    for (std::size_t i = 0; i < presetCount; ++i)
        preset.addItem(presets[i].name, static_cast<int>(i) + 2);
    preset.onChange = [this] { selectPreset(); };

    for (auto* knob : std::initializer_list<juce::Slider*> { &space, &bpm, &freeDecay, &preDelay, &tone, &width, &mix, &tail })
        styleSlider(*knob, true);
    space.setTitle("SPACE amount");
    bpm.setTitle("Tap BPM");
    freeDecay.setTitle("Free Decay");
    preDelay.setTitle("Pre-delay");
    tone.setTitle("Tone");
    width.setTitle("Width");
    mix.setTitle("Mix");
    tail.setTitle("Tail");
    space.setTextValueSuffix(" %");
    bpm.setTextValueSuffix(" BPM");
    freeDecay.setTextValueSuffix(" s");
    preDelay.setTextValueSuffix(" ms");
    tone.setTextValueSuffix(" ");
    width.setTextValueSuffix(" ");
    mix.setTextValueSuffix(" %");
    tail.setTextValueSuffix(" s");

    std::array<juce::ToggleButton*, 4> toggles { &preDelayOn, &toneOn, &widthOn, &tailOn };
    for (auto* toggle : toggles)
    {
        toggle->setTitle(toggle->getButtonText() + " enable");
        addAndMakeVisible(*toggle);
    }
    tapButton.onClick = [this] { pluginProcessor.tapTempoNow(); };
    tapButton.setTitle("Tap tempo");
    addAndMakeVisible(tapButton);

    addLabel(title, "DRM-SMRTVRB");
    title.setFont(plain(24.0f, true));
    title.setColour(juce::Label::textColourId, ivory);
    addLabel(brand, "yeahwaah audio  /  DRUM REVERB");
    addLabel(presetLabel, "PRESET");
    addLabel(presetStatus, "");
    addLabel(targetLabel, "SOURCE");
    addLabel(characterLabel, "CHARACTER");
    addLabel(spaceLabel, "SPACE");
    addLabel(syncLabel, "TEMPO SYNC");
    addLabel(bpmLabel, "BPM");
    addLabel(divisionLabel, "DIVISION");
    addLabel(decayLabel, "REVERB TIME");
    addLabel(mixLabel, "MIX");
    for (auto* label : { &spaceLabel, &bpmLabel, &decayLabel, &mixLabel })
        label->setJustificationType(juce::Justification::centred);
    spaceLabel.setColour(juce::Label::textColourId, amber);
    spaceLabel.setFont(plain(12.0f, true));

    auto& state = pluginProcessor.parameters;
    comboAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "target", target);
    comboAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "engine", character);
    comboAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "sync", sync);
    comboAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state, "division", division);
    sliderAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "space", space);
    sliderAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "bpm", bpm);
    sliderAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "freeDecay", freeDecay);
    sliderAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advPreDelay", preDelay);
    sliderAttachments[4] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advTone", tone);
    sliderAttachments[5] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advWidth", width);
    sliderAttachments[6] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advMix", mix);
    // Tail uses the same APVTS attachment type and is retained separately to keep the fixed arrays small.
    buttonAttachments[0] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advPreDelayOn", preDelayOn);
    buttonAttachments[1] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advToneOn", toneOn);
    buttonAttachments[2] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advWidthOn", widthOn);
    buttonAttachments[3] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state, "advTailOn", tailOn);
    tailAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "advTail", tail);

    // Moving the Mix knob engages the existing override; a double-click restores both defaults.
    auto setMixEnabled = [this](bool enabled)
    {
        if (auto* parameter = pluginProcessor.parameters.getParameter("advMixOn"))
        {
            if ((parameter->getValue() >= 0.5f) == enabled) return;
            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost(enabled ? 1.0f : 0.0f);
            parameter->endChangeGesture();
        }
    };
    mix.onValueChange = [this, setMixEnabled] { if (mix.isUserChange()) setMixEnabled(true); };
    mix.onDisengage = [setMixEnabled] { setMixEnabled(false); };

    setSize(760, 500);
    startTimerHz(24);
}

DRMsmrtvrbAudioProcessorEditor::~DRMsmrtvrbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void DRMsmrtvrbAudioProcessorEditor::styleSlider(juce::Slider& slider, bool)
{
    slider.setWantsKeyboardFocus(true);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
    slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, true);
    addAndMakeVisible(slider);
}

void DRMsmrtvrbAudioProcessorEditor::addLabel(juce::Label& label, const juce::String& value)
{
    label.setText(value, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, muted);
    label.setFont(plain(10.5f, true));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void DRMsmrtvrbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(chassis);
    const auto area = getLocalBounds().toFloat().reduced(8.0f);
    g.setColour(panel);
    g.fillRoundedRectangle(area, 8.0f);
    g.setColour(border);
    g.drawRoundedRectangle(area, 8.0f, 1.5f);
    drawScrew(g, area.getX() + 11.0f, area.getY() + 11.0f);
    drawScrew(g, area.getRight() - 11.0f, area.getY() + 11.0f);
    drawScrew(g, area.getX() + 11.0f, area.getBottom() - 11.0f);
    drawScrew(g, area.getRight() - 11.0f, area.getBottom() - 11.0f);

    g.setColour(border.withAlpha(0.8f));
    g.drawHorizontalLine(sourcePanel.getY() - 6, static_cast<float>(sourcePanel.getX()), static_cast<float>(outputPanel.getRight()));

    drawPanel(g, sourcePanel, "SOURCE / TEMPO");
    drawPanel(g, spacePanel, "SPACE");
    drawPanel(g, outputPanel, "OUTPUT");
    drawPanel(g, advancedPanel, "ADVANCED PROFILE TRIMS");

    // Meters: input L/R then output L/R.
    constexpr float barWidth = 11.0f, gap = 4.0f, pairGap = 9.0f;
    const float meterHeight = static_cast<float>(meterBox.getHeight() - 26);
    const float top = static_cast<float>(meterBox.getY() + 14);
    for (int i = 0; i < 4; ++i)
    {
        const float x = static_cast<float>(meterBox.getX()) + static_cast<float>(i) * (barWidth + gap)
                        + (i >= 2 ? pairGap - gap : 0.0f);
        const juce::Rectangle<float> meter { x, top, barWidth, meterHeight };
        g.setColour(chassis);
        g.fillRoundedRectangle(meter, 2.0f);
        const auto level = juce::jlimit(0.0f, 1.0f, meterLevels[static_cast<std::size_t>(i)]);
        g.setColour(i < 2 ? meterInput : amber);
        g.fillRoundedRectangle(juce::Rectangle<float>(x, meter.getBottom() - meterHeight * level, barWidth, meterHeight * level), 2.0f);
        const float markerY = meter.getBottom() - meterHeight * meterPeakMarkers[static_cast<std::size_t>(i)];
        g.setColour(ivory);
        g.drawLine(x - 1.0f, markerY, x + barWidth + 1.0f, markerY, 1.5f);
        g.setColour(muted);
        g.setFont(plain(9.0f, true));
        g.drawText((i % 2 == 0) ? "L" : "R", static_cast<int>(x) - 2, static_cast<int>(meter.getBottom()) + 1,
                   static_cast<int>(barWidth) + 4, 11, juce::Justification::centred);
    }
    g.setColour(muted);
    g.setFont(plain(9.0f, true));
    g.drawText("IN", meterBox.getX(), meterBox.getY(), static_cast<int>(2 * barWidth + gap), 12, juce::Justification::centred);
    g.drawText("OUT", meterBox.getX() + static_cast<int>(2 * (barWidth + gap) + pairGap - gap), meterBox.getY(),
               static_cast<int>(2 * barWidth + gap), 12, juce::Justification::centred);

    const auto activityArea = envelopeBox.withTrimmedBottom(13).toFloat();
    g.setColour(chassis);
    g.fillRoundedRectangle(activityArea, 2.0f);
    g.setColour(border);
    g.drawRoundedRectangle(activityArea.reduced(0.5f), 2.0f, 1.0f);
    g.setColour(amber.withAlpha(0.85f));
    const float step = activityArea.getWidth() / static_cast<float>(activitySnapshot.size());
    for (std::size_t i = 0; i < activitySnapshot.size(); ++i)
    {
        const float height = activityArea.getHeight() * juce::jlimit(0.0f, 1.0f, activitySnapshot[i]);
        g.fillRect(activityArea.getX() + static_cast<float>(i) * step,
                   activityArea.getBottom() - height, juce::jmax(1.0f, step - 0.5f), height);
    }
    g.setColour(muted);
    g.setFont(plain(9.5f, true));
    g.drawText("WET ENVELOPE", envelopeBox.withTop(envelopeBox.getBottom() - 12), juce::Justification::centred);
}

void DRMsmrtvrbAudioProcessorEditor::resized()
{
    auto content = getLocalBounds().reduced(24, 20);

    auto header = content.removeFromTop(54);
    auto presetArea = header.removeFromRight(juce::jmax(220, content.getWidth() / 2));
    title.setBounds(header.removeFromTop(32));
    brand.setBounds(header.removeFromTop(18));
    auto presetRow = presetArea.removeFromTop(16);
    presetLabel.setBounds(presetRow.removeFromLeft(presetRow.getWidth() / 2));
    presetStatus.setBounds(presetRow);
    presetStatus.setJustificationType(juce::Justification::centredRight);
    preset.setBounds(presetArea.removeFromTop(30).withTrimmedTop(2));

    content.removeFromTop(12);
    advancedPanel = content.removeFromBottom(juce::jlimit(132, 190, content.getHeight() * 38 / 100));
    content.removeFromBottom(8);
    sourcePanel = content.removeFromLeft(content.getWidth() * 40 / 100);
    content.removeFromLeft(8);
    outputPanel = content.removeFromRight(content.getWidth() * 38 / 100);
    content.removeFromRight(8);
    spacePanel = content;

    // Source / tempo panel.
    {
        auto inner = sourcePanel.reduced(10).withTrimmedTop(18);
        auto row = inner.removeFromTop(46);
        const int half = row.getWidth() / 2;
        auto left = row.removeFromLeft(half).withTrimmedRight(4);
        auto right = row.withTrimmedLeft(4);
        targetLabel.setBounds(left.removeFromTop(16));
        target.setBounds(left.withHeight(26));
        characterLabel.setBounds(right.removeFromTop(16));
        character.setBounds(right.withHeight(26));

        inner.removeFromTop(4);
        auto columns = inner;
        auto controls = columns.removeFromLeft(columns.getWidth() * 54 / 100).withTrimmedRight(6);
        syncLabel.setBounds(controls.removeFromTop(16));
        auto syncRow = controls.removeFromTop(26);
        tapButton.setBounds(syncRow.removeFromRight(syncRow.getWidth() * 40 / 100));
        sync.setBounds(syncRow.withTrimmedRight(4));
        controls.removeFromTop(4);
        divisionLabel.setBounds(controls.removeFromTop(16));
        division.setBounds(controls.removeFromTop(26));

        bpmLabel.setBounds(columns.removeFromTop(16));
        bpm.setTextBoxStyle(juce::Slider::TextBoxBelow, false, juce::jmin(72, columns.getWidth()), 18);
        bpm.setBounds(columns);
    }

    // SPACE panel: large SPACE knob with Reverb Time beside it.
    {
        auto inner = spacePanel.reduced(10).withTrimmedTop(18);
        auto decayColumn = inner.removeFromRight(inner.getWidth() * 34 / 100);
        spaceLabel.setBounds(inner.removeFromTop(16));
        space.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
        space.setBounds(inner);
        decayLabel.setBounds(decayColumn.removeFromTop(16));
        const int decayHeight = juce::jmin(decayColumn.getHeight(), decayColumn.getWidth() + 22);
        freeDecay.setTextBoxStyle(juce::Slider::TextBoxBelow, false, juce::jmin(72, decayColumn.getWidth()), 18);
        freeDecay.setBounds(decayColumn.withSizeKeepingCentre(decayColumn.getWidth(), decayHeight));
    }

    // Output panel: meters, Mix, wet envelope.
    {
        auto inner = outputPanel.reduced(10).withTrimmedTop(18);
        envelopeBox = inner.removeFromBottom(46);
        inner.removeFromBottom(6);
        meterBox = inner.removeFromLeft(66);
        auto mixArea = inner.withTrimmedLeft(4);
        mixLabel.setBounds(mixArea.removeFromTop(16));
        mix.setTextBoxStyle(juce::Slider::TextBoxBelow, false, juce::jmin(64, mixArea.getWidth()), 18);
        mix.setBounds(mixArea);
    }

    // Advanced panel: four ON/OFF switches, each above its knob.
    {
        auto inner = advancedPanel.reduced(10).withTrimmedTop(20);
        const int cell = inner.getWidth() / 4;
        std::array<juce::ToggleButton*, 4> toggles { &preDelayOn, &toneOn, &widthOn, &tailOn };
        std::array<juce::Slider*, 4> knobs { &preDelay, &tone, &width, &tail };
        for (std::size_t i = 0; i < 4; ++i)
        {
            auto column = inner.removeFromLeft(cell).reduced(4, 0);
            toggles[i]->setBounds(column.removeFromTop(38));
            knobs[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, juce::jmin(72, column.getWidth()), 18);
            knobs[i]->setBounds(column);
        }
    }
}

void DRMsmrtvrbAudioProcessorEditor::timerCallback()
{
    const auto syncMode = static_cast<int>(std::lround(pluginProcessor.parameters.getRawParameterValue("sync")->load()));
    bpm.setEnabled(syncMode == 2);
    tapButton.setEnabled(syncMode == 2);
    division.setEnabled(syncMode != 0);
    freeDecay.setEnabled(syncMode == 0);
    if (syncMode != 2)
        bpm.setValue(syncMode == 1 ? pluginProcessor.displayedBpm() : 120.0,
                     juce::dontSendNotification);
    bpmLabel.setText(syncMode == 2 ? "TAP BPM"
        : syncMode == 1 ? (pluginProcessor.hostTempoAvailable() ? "HOST BPM" : "FALLBACK BPM")
                        : "BPM",
        juce::dontSendNotification);

    preDelay.setEnabled(preDelayOn.getToggleState());
    tone.setEnabled(toneOn.getToggleState());
    width.setEnabled(widthOn.getToggleState());
    tailOn.setEnabled(syncMode == 0);
    tail.setEnabled(syncMode == 0 && tailOn.getToggleState());
    tailOn.setTooltip(syncMode == 0 ? "Override the profile tail length" : "Tail timing follows the selected tempo division");
    tail.setTooltip(syncMode == 0 ? "Tail length in seconds" : "Tail timing follows the selected tempo division");
    const auto currentProgram = pluginProcessor.presetIndexForDisplay();
    if (currentProgram >= 0)
        preset.setSelectedId(currentProgram + 2, juce::dontSendNotification);
    else
        preset.setSelectedId(1, juce::dontSendNotification);
    const bool modified = pluginProcessor.isCurrentPresetModified();
    presetStatus.setText(modified ? "MODIFIED" : "FACTORY", juce::dontSendNotification);
    presetStatus.setColour(juce::Label::textColourId, modified ? amber : muted);

    for (int channel = 0; channel < 2; ++channel)
    {
        for (int direction = 0; direction < 2; ++direction)
        {
            const auto meterIndex = static_cast<std::size_t>(direction * 2 + channel);
            const auto current = direction == 0 ? pluginProcessor.inputPeak(channel) : pluginProcessor.outputPeak(channel);
            auto& level = meterLevels[meterIndex];
            auto& marker = meterPeakMarkers[meterIndex];
            auto& hold = meterPeakHoldTicks[meterIndex];
            level = juce::jmax(current, level * 0.90f);
            if (current >= marker)
            {
                marker = current;
                hold = 18;
            }
            else if (hold > 0)
                --hold;
            else
                marker *= 0.96f;
        }
    }
    pluginProcessor.copyActivityHistory(activitySnapshot);
    repaint();
}

void DRMsmrtvrbAudioProcessorEditor::selectPreset()
{
    const int selection = preset.getSelectedId();
    if (selection >= 2) pluginProcessor.loadFactoryPreset(selection - 2);
}
