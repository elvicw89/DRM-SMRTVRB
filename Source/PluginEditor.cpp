#include "PluginEditor.h"

namespace
{
const juce::Colour chassis { 0xff1b1d1f };
const juce::Colour panel { 0xff25282b };
const juce::Colour recessed { 0xff131517 };
const juce::Colour border { 0xff464b4f };
const juce::Colour ivory { 0xffe8e1ca };
const juce::Colour muted { 0xffaaa595 };
const juce::Colour amber { 0xffe0a040 };
const juce::Colour inactive { 0xff6d7173 };
const juce::Colour meterInput { 0xff71836b };

juce::Font plain(float size, bool bold = false)
{
    return juce::Font(juce::FontOptions(size, bold ? juce::Font::bold : juce::Font::plain));
}

// Deterministic horizontal brushed-metal grain.
void drawBrushed(juce::Graphics& g, juce::Rectangle<float> area, juce::Colour top, juce::Colour bottom, float grain)
{
    g.setGradientFill(juce::ColourGradient(top, 0.0f, area.getY(), bottom, 0.0f, area.getBottom(), false));
    g.fillRect(area);
    juce::Random random(1234);
    for (float y = area.getY(); y < area.getBottom(); y += 1.0f)
    {
        const float level = random.nextFloat();
        g.setColour((level > 0.5f ? juce::Colours::white : juce::Colours::black).withAlpha(grain * std::abs(level - 0.5f) * 2.0f));
        g.drawHorizontalLine(static_cast<int>(y), area.getX(), area.getRight());
    }
}

void drawBevel(juce::Graphics& g, juce::Rectangle<float> r, float radius, bool raised)
{
    g.setColour((raised ? juce::Colours::white : juce::Colours::black).withAlpha(0.22f));
    g.drawRoundedRectangle(r.reduced(0.5f), radius, 1.0f);
    g.setColour((raised ? juce::Colours::black : juce::Colours::white).withAlpha(0.35f));
    g.drawRoundedRectangle(r.reduced(1.5f), radius, 1.0f);
}

void drawCaption(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text)
{
    g.setColour(ivory.withAlpha(0.9f));
    g.setFont(plain(11.0f, true));
    g.drawText(text, area.getX() + 12, area.getY() + 5, area.getWidth() - 24, 14, juce::Justification::centredLeft);
    g.setColour(border.withAlpha(0.8f));
    g.drawHorizontalLine(area.getY() + 21, static_cast<float>(area.getX() + 12), static_cast<float>(area.getRight() - 12));
}

void drawScrew(juce::Graphics& g, float x, float y)
{
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff6a6e70), x - 4.0f, y - 4.0f,
                                           juce::Colour(0xff17191a), x + 4.0f, y + 4.0f, false));
    g.fillEllipse(x - 5.0f, y - 5.0f, 10.0f, 10.0f);
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.drawEllipse(x - 5.0f, y - 5.0f, 10.0f, 10.0f, 1.0f);
    g.setColour(juce::Colour(0xff0c0d0e));
    g.drawLine(x - 3.0f, y + 1.8f, x + 3.0f, y - 1.8f, 1.6f);
}

// Dark inset panel set into the metal plate, with metal rim, caption and corner screws.
void drawPanel(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& caption)
{
    const auto r = area.toFloat();
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.fillRoundedRectangle(r.expanded(2.0f), 6.0f);
    drawBrushed(g, r.reduced(1.0f), juce::Colour(0xff202224), juce::Colour(0xff141516), 0.06f);
    g.setColour(juce::Colour(0xff7a7f82).withAlpha(0.8f));
    g.drawRoundedRectangle(r.expanded(1.0f), 6.0f, 1.2f);
    drawBevel(g, r, 5.0f, false);
    drawCaption(g, area, caption);
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
    const bool active = enabled && (slider.isMouseOverOrDragging() || slider.hasKeyboardFocus(true));

    // Soft amber glow marks hover, drag and keyboard focus instead of a focus box.
    if (active)
    {
        for (int i = 4; i >= 1; --i)
        {
            const float r = tickOuter + static_cast<float>(i) * 1.6f;
            g.setColour(amber.withAlpha(0.07f * static_cast<float>(5 - i)));
            g.fillEllipse(centre.x - r, centre.y - r, r * 2.0f, r * 2.0f);
        }
    }

    // Scale ticks every 10% of travel.
    g.setColour(ivory.withAlpha(0.6f * alpha));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * static_cast<float>(i) / 10.0f;
        const float inner = ringRadius + 2.5f;
        g.drawLine(centre.x + inner * std::sin(a), centre.y - inner * std::cos(a),
                   centre.x + tickOuter * std::sin(a), centre.y - tickOuter * std::cos(a),
                   i % 5 == 0 ? 1.6f : 1.0f);
    }

    // Black bezel with the amber value arc.
    const float stroke = juce::jmax(2.5f, diameter * 0.045f);
    g.setColour(juce::Colour(0xff0a0b0c));
    g.fillEllipse(centre.x - ringRadius - stroke, centre.y - ringRadius - stroke,
                  (ringRadius + stroke) * 2.0f, (ringRadius + stroke) * 2.0f);
    juce::Path track;
    track.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0xff2f3336).withAlpha(alpha));
    g.strokePath(track, juce::PathStrokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    juce::Path value;
    value.addCentredArc(centre.x, centre.y, ringRadius, ringRadius, 0.0f, startAngle, angle, true);
    g.setColour((enabled ? amber : inactive).withAlpha(alpha));
    g.strokePath(value, juce::PathStrokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Brushed-metal cap with concentric machining rings.
    const float capRadius = ringRadius - stroke - 2.0f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffc9ccce), centre.x - capRadius * 0.6f, centre.y - capRadius * 0.8f,
                                           juce::Colour(0xff3d4245), centre.x + capRadius * 0.7f, centre.y + capRadius * 0.9f, false));
    g.setOpacity(alpha);
    g.fillEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    for (float r = 2.0f; r < capRadius; r += 1.8f)
    {
        g.setColour(juce::Colours::black.withAlpha(0.10f + 0.05f * std::sin(r * 2.3f)));
        g.drawEllipse(centre.x - r, centre.y - r, r * 2.0f, r * 2.0f, 0.7f);
    }
    g.setColour(juce::Colour(0xff0b0c0d));
    g.drawEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.5f);
    g.setColour(juce::Colours::white.withAlpha(0.28f * alpha));
    g.drawEllipse(centre.x - capRadius + 1.5f, centre.y - capRadius + 1.5f,
                  capRadius * 2.0f - 3.0f, capRadius * 2.0f - 3.0f, 1.0f);

    juce::Path pointer;
    pointer.addRoundedRectangle(-1.5f, -capRadius * 0.9f, 3.0f, capRadius * 0.5f, 1.2f);
    const auto transform = juce::AffineTransform::rotation(angle).translated(centre.x, centre.y);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillPath(pointer, transform.translated(0.8f, 0.8f));
    g.setColour((enabled ? ivory : muted).withAlpha(alpha));
    g.fillPath(pointer, transform);
}

// Horizontal metal-cap fader in a recessed slot, with 0 dB tick marks.
void DRMLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                      float, float, juce::Slider::SliderStyle, juce::Slider& slider)
{
    const float alpha = slider.isEnabled() ? 1.0f : 0.45f;
    const float cy = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
    const float left = static_cast<float>(x) + 8.0f, right = static_cast<float>(x + width) - 8.0f;
    const auto slot = juce::Rectangle<float>(left, cy - 3.5f, right - left, 7.0f);

    g.setColour(juce::Colours::black.withAlpha(0.9f));
    g.fillRoundedRectangle(slot.expanded(1.5f), 4.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff050606), 0.0f, slot.getY(),
                                           juce::Colour(0xff1b1d1e), 0.0f, slot.getBottom(), false));
    g.fillRoundedRectangle(slot, 3.0f);

    const double range = slider.getMaximum() - slider.getMinimum();
    for (double mark : { -24.0, -12.0, 0.0, 6.0, 12.0, 18.0 })
    {
        const float tx = left + static_cast<float>((mark - slider.getMinimum()) / range) * (right - left);
        g.setColour((mark == 0.0 ? ivory : muted).withAlpha(0.7f * alpha));
        g.drawVerticalLine(juce::roundToInt(tx), cy + 7.0f, cy + (mark == 0.0 ? 13.0f : 11.0f));
    }

    const float fill = juce::jlimit(left, right, sliderPos);
    g.setColour(amber.withAlpha(0.85f * alpha));
    g.fillRoundedRectangle(juce::Rectangle<float>(left, cy - 2.0f, fill - left, 4.0f), 2.0f);

    const auto cap = juce::Rectangle<float>(14.0f, 24.0f).withCentre({ fill, cy });
    if (slider.hasKeyboardFocus(true) || slider.isMouseOverOrDragging())
    {
        g.setColour(amber.withAlpha(0.3f * alpha));
        g.fillRoundedRectangle(cap.expanded(4.0f), 5.0f);
    }
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(cap.expanded(1.0f), 3.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xffb9bdbf).withAlpha(alpha), cap.getX(), 0.0f,
                                           juce::Colour(0xff595d60).withAlpha(alpha), cap.getRight(), 0.0f, false));
    g.fillRoundedRectangle(cap, 2.5f);
    g.setColour(juce::Colours::black.withAlpha(0.75f));
    g.drawVerticalLine(juce::roundToInt(cap.getCentreX()), cap.getY() + 3.0f, cap.getBottom() - 3.0f);
    g.setColour(juce::Colours::white.withAlpha(0.35f));
    g.drawVerticalLine(juce::roundToInt(cap.getCentreX()) + 1, cap.getY() + 3.0f, cap.getBottom() - 3.0f);
}

juce::Label* DRMLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* label = juce::LookAndFeel_V4::createSliderTextBox(slider);
    label->setFont(plain(12.0f, true));
    label->setJustificationType(juce::Justification::centred);
    return label;
}

// Illuminated push button: the lit amber face and an ON/OFF caption show the state.
void DRMLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool, bool)
{
    const bool enabled = button.isEnabled();
    const bool on = button.getToggleState();
    const float alpha = enabled ? 1.0f : 0.45f;
    const auto bounds = button.getLocalBounds().toFloat();
    const float w = juce::jmin(bounds.getWidth() - 6.0f, 104.0f);
    const auto face = juce::Rectangle<float>(w, 26.0f).withCentre({ bounds.getCentreX(), bounds.getY() + 15.0f });

    if (button.hasKeyboardFocus(true))
    {
        g.setColour(amber.withAlpha(0.35f));
        g.fillRoundedRectangle(face.expanded(5.0f), 7.0f);
    }

    if (on && enabled)
    {
        for (int i = 4; i >= 1; --i)
        {
            g.setColour(amber.withAlpha(0.06f * static_cast<float>(5 - i)));
            g.fillRoundedRectangle(face.expanded(static_cast<float>(i) * 1.5f), 5.0f);
        }
    }
    g.setColour(juce::Colour(0xff0a0b0c));
    g.fillRoundedRectangle(face.expanded(2.0f), 5.0f);
    const auto inner = face.reduced(1.0f);
    g.setGradientFill(on ? juce::ColourGradient(juce::Colour(0xffffd884), 0.0f, inner.getY(),
                                                juce::Colour(0xffc98418), 0.0f, inner.getBottom(), false)
                         : juce::ColourGradient(juce::Colour(0xff4d5154), 0.0f, inner.getY(),
                                                juce::Colour(0xff25282a), 0.0f, inner.getBottom(), false));
    g.setOpacity(alpha);
    g.fillRoundedRectangle(inner, 4.0f);
    g.setColour(juce::Colours::white.withAlpha(on ? 0.45f : 0.15f));
    g.drawHorizontalLine(static_cast<int>(inner.getY()) + 1, inner.getX() + 4.0f, inner.getRight() - 4.0f);

    g.setColour((on ? juce::Colour(0xff2b1c05) : muted).withAlpha(alpha));
    g.setFont(plain(12.5f, true));
    g.drawText(button.getButtonText().toUpperCase(), inner.toNearestInt(), juce::Justification::centred);

    g.setColour((on ? amber : muted).withAlpha(alpha));
    g.setFont(plain(9.5f, true));
    g.drawText(on ? "ON" : "OFF", juce::Rectangle<float>(bounds.getX(), face.getBottom() + 3.0f, bounds.getWidth(), 11.0f).toNearestInt(),
               juce::Justification::centred);
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
    setResizeLimits(640, 460, 1040, 740);

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

    outLevel.setTitle("Output level");
    outLevel.setWantsKeyboardFocus(true);
    outLevel.setSliderStyle(juce::Slider::LinearHorizontal);
    outLevel.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 20);
    outLevel.setTextValueSuffix(" dB");
    outLevel.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(outLevel);

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
    addLabel(outLevelLabel, "OUTPUT LEVEL");
    outLevelLabel.setColour(juce::Label::textColourId, ivory);
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
    // Display-only: show SPACE as a whole percentage; the parameter value keeps full precision.
    space.textFromValueFunction = [](double value) { return juce::String(juce::roundToInt(value)); };
    space.updateText();
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
    outLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state, "outGain", outLevel);
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

    setSize(760, 540);
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

void DRMsmrtvrbAudioProcessorEditor::renderBackdrop()
{
    backdrop = juce::Image(juce::Image::ARGB, juce::jmax(1, getWidth()), juce::jmax(1, getHeight()), true);
    juce::Graphics g(backdrop);

    // Outer brushed-steel faceplate with rim and corner screws.
    const auto full = getLocalBounds().toFloat();
    drawBrushed(g, full, juce::Colour(0xff5a5e61), juce::Colour(0xff2c2f31), 0.12f);
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawRect(full, 2.0f);
    drawBevel(g, full.reduced(2.0f), 2.0f, true);
    for (const auto& pt : { juce::Point<float>(14.0f, 14.0f), juce::Point<float>(full.getRight() - 14.0f, 14.0f),
                            juce::Point<float>(14.0f, full.getBottom() - 14.0f),
                            juce::Point<float>(full.getRight() - 14.0f, full.getBottom() - 14.0f) })
        drawScrew(g, pt.x, pt.y);

    // Dark header plate with title, preset and status dividers.
    const auto plate = headerPlate.toFloat();
    drawBrushed(g, plate, juce::Colour(0xff2a2c2e), juce::Colour(0xff151617), 0.07f);
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawRoundedRectangle(plate, 3.0f, 1.5f);
    drawBevel(g, plate, 3.0f, false);
    g.setColour(border);
    const float dividerTop = plate.getY() + 8.0f, dividerBottom = plate.getBottom() - 8.0f;
    const float presetLeft = static_cast<float>(preset.getX()) - 14.0f;
    const float statusLeft = static_cast<float>(statusBox.getX()) - 12.0f;
    g.drawVerticalLine(static_cast<int>(presetLeft), dividerTop, dividerBottom);
    g.drawVerticalLine(static_cast<int>(statusLeft), dividerTop, dividerBottom);

    drawPanel(g, sourcePanel, "SOURCE / TEMPO");
    drawPanel(g, spacePanel, "SPACE");
    drawPanel(g, outputPanel, "OUTPUT");
    drawPanel(g, advancedPanel, "ADVANCED PROFILE TRIMS");
    {
        const auto r = levelPanel.toFloat();
        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.fillRoundedRectangle(r.expanded(2.0f), 6.0f);
        drawBrushed(g, r.reduced(1.0f), juce::Colour(0xff202224), juce::Colour(0xff141516), 0.06f);
        g.setColour(juce::Colour(0xff7a7f82).withAlpha(0.8f));
        g.drawRoundedRectangle(r.expanded(1.0f), 6.0f, 1.2f);
        drawBevel(g, r, 5.0f, false);
    }
}

void DRMsmrtvrbAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (backdrop.isNull() || backdrop.getWidth() != getWidth() || backdrop.getHeight() != getHeight())
        renderBackdrop();
    g.drawImageAt(backdrop, 0, 0);

    // Preset status lamp.
    {
        const auto box = statusBox.toFloat();
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(box.expanded(1.5f), 4.0f);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff3d4043), 0.0f, box.getY(),
                                               juce::Colour(0xff1d1f21), 0.0f, box.getBottom(), false));
        g.fillRoundedRectangle(box, 3.0f);
        g.setColour(presetModified ? amber : border);
        g.drawRoundedRectangle(box.reduced(0.5f), 3.0f, 1.0f);
        const float lx = box.getRight() - 14.0f, ly = box.getCentreY();
        if (presetModified)
        {
            g.setColour(amber.withAlpha(0.3f));
            g.fillEllipse(lx - 8.0f, ly - 8.0f, 16.0f, 16.0f);
        }
        g.setColour(presetModified ? juce::Colour(0xffffd884) : juce::Colour(0xff4a4d4f));
        g.fillEllipse(lx - 4.5f, ly - 4.5f, 9.0f, 9.0f);
    }

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

    headerPlate = content.removeFromTop(58);
    auto header = headerPlate.reduced(14, 6);
    statusBox = header.removeFromRight(juce::jmin(126, header.getWidth() / 4)).withSizeKeepingCentre(juce::jmin(126, header.getWidth() / 4), 30);
    header.removeFromRight(12);
    auto presetArea = header.removeFromRight(juce::jmax(200, header.getWidth() * 45 / 100));
    header.removeFromRight(14);
    title.setBounds(header.removeFromTop(32));
    brand.setBounds(header.removeFromTop(18));
    presetLabel.setBounds(presetArea.removeFromTop(16));
    presetStatus.setBounds(statusBox.withTrimmedRight(24).withTrimmedLeft(4));
    presetStatus.setJustificationType(juce::Justification::centred);
    preset.setBounds(presetArea.removeFromTop(30).withTrimmedTop(2));

    content.removeFromTop(10);
    levelPanel = content.removeFromBottom(40);
    content.removeFromBottom(8);
    advancedPanel = content.removeFromBottom(juce::jlimit(132, 190, content.getHeight() * 38 / 100));
    content.removeFromBottom(8);
    sourcePanel = content.removeFromLeft(content.getWidth() * 40 / 100);
    content.removeFromLeft(8);
    outputPanel = content.removeFromRight(content.getWidth() * 38 / 100);
    content.removeFromRight(8);
    spacePanel = content;

    // Output level fader strip across the bottom.
    {
        auto strip = levelPanel.reduced(14, 6);
        outLevelLabel.setBounds(strip.removeFromLeft(100));
        outLevel.setBounds(strip);
    }

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
            toggles[i]->setBounds(column.removeFromTop(42));
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
    presetModified = modified;

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
