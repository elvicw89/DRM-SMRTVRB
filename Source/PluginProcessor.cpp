#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <cmath>
#include <memory>

namespace
{
using namespace juce;
using namespace drmspace;

std::unique_ptr<AudioParameterFloat> floatParameter(const char* id, const char* name,
                                                      float minimum, float maximum, float initial,
                                                      const char* unit = "", int decimals = 2,
                                                      float displayScale = 1.0f)
{
    const auto fromValue = [decimals, displayScale](float value, int)
    {
        return String(value * displayScale, decimals);
    };
    const auto toValue = [displayScale](const String& text)
    {
        return text.getFloatValue() / displayScale;
    };
    return std::make_unique<AudioParameterFloat>(ParameterID { id, 1 }, name,
        NormalisableRange<float>(minimum, maximum), initial,
        AudioParameterFloatAttributes().withLabel(unit)
            .withStringFromValueFunction(fromValue)
            .withValueFromStringFunction(toValue));
}

std::unique_ptr<AudioParameterBool> boolParameter(const char* id, const char* name, bool initial = false)
{
    return std::make_unique<AudioParameterBool>(ParameterID { id, 1 }, name, initial);
}

std::unique_ptr<AudioParameterChoice> choiceParameter(const char* id, const char* name,
                                                       StringArray choices, int initial = 0)
{
    return std::make_unique<AudioParameterChoice>(ParameterID { id, 1 }, name, choices, initial);
}

SourceTarget targetFrom(float value) noexcept
{
    return static_cast<SourceTarget>(jlimit(0, 3, static_cast<int>(std::lround(value))));
}

Engine engineFrom(float value) noexcept
{
    return static_cast<Engine>(jlimit(0, 2, static_cast<int>(std::lround(value))));
}

SyncMode syncFrom(float value) noexcept
{
    return static_cast<SyncMode>(jlimit(0, 2, static_cast<int>(std::lround(value))));
}

Division divisionFrom(float value) noexcept
{
    return static_cast<Division>(jlimit(0, 5, static_cast<int>(std::lround(value))));
}
}

juce::AudioProcessorValueTreeState::ParameterLayout DRMsmrtvrbAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    p.push_back(choiceParameter("target", "Target", { "Shells", "Snare", "Room", "Overheads" }));
    p.push_back(choiceParameter("engine", "Character", { "Room", "Spring", "Gated" }));
    p.emplace_back(floatParameter("space", "SPACE", 0.0f, 100.0f, 0.0f, "%", 0));
    p.push_back(choiceParameter("sync", "Sync", { "Free", "Host", "Tap" }, 1));
    p.emplace_back(floatParameter("bpm", "Tap BPM", 20.0f, 300.0f, 120.0f, "BPM", 1));
    p.push_back(choiceParameter("division", "Division", { "1/16", "1/8", "1/4", "1/2", "1 bar", "2 bars" }, 2));
    p.emplace_back(floatParameter("freeDecay", "Free Decay", 0.08f, 8.0f, 1.2f, "s", 2));
    p.push_back(boolParameter("advPreDelayOn", "Advanced Pre-delay Enabled"));
    p.emplace_back(floatParameter("advPreDelay", "Advanced Pre-delay", 0.0f, 200.0f, 12.0f, "ms", 1));
    p.push_back(boolParameter("advToneOn", "Advanced Tone Enabled"));
    p.emplace_back(floatParameter("advTone", "Advanced Tone", -1.0f, 1.0f, 0.0f, "", 2));
    p.push_back(boolParameter("advWidthOn", "Advanced Width Enabled"));
    p.emplace_back(floatParameter("advWidth", "Advanced Width", 0.0f, 2.0f, 1.0f, "", 2));
    p.push_back(boolParameter("advMixOn", "Advanced Mix Enabled"));
    p.emplace_back(floatParameter("advMix", "Advanced Mix", 0.0f, 1.0f, 1.0f, "%", 0, 100.0f));
    p.push_back(boolParameter("advTailOn", "Advanced Tail Enabled"));
    p.emplace_back(floatParameter("advTail", "Advanced Tail", 0.08f, 8.0f, 1.2f, "s", 2));
    p.emplace_back(floatParameter("outGain", "Output Level", -24.0f, 18.0f, 0.0f, "dB", 1));
    return { p.begin(), p.end() };
}

DRMsmrtvrbAudioProcessor::DRMsmrtvrbAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
                                      .withOutput("Output", AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "DRM_SMRTVRB_STATE", createParameterLayout())
{
    loadFactoryPreset(0);
}

DRMsmrtvrbAudioProcessor::RawParameter DRMsmrtvrbAudioProcessor::parameter(const char* id) const noexcept
{
    return parameters.getRawParameterValue(id);
}

float DRMsmrtvrbAudioProcessor::read(const char* id) const noexcept
{
    const auto* value = parameter(id);
    return value != nullptr ? value->load(std::memory_order_relaxed) : 0.0f;
}

void DRMsmrtvrbAudioProcessor::prepareToPlay(double sampleRate, int blockSize)
{
    maximumBlockSize = std::max(1, blockSize);
    for (auto& channel : wetBuffer) channel.assign(static_cast<std::size_t>(maximumBlockSize), 0.0f);
    activeTarget = targetFrom(read("target"));
    activeEngine = engineFrom(read("engine"));
    reverb.prepare(sampleRate, maximumBlockSize);
    reverb.setProfile(activeTarget, activeEngine);
    macroSmoother.prepare(sampleRate, 20.0f);
    macroSmoother.reset(SpaceMacro::evaluate(activeTarget, activeEngine, read("space")));
    tailSmoother.reset(sampleRate, 0.025);
    tailSmoother.setCurrentAndTargetValue(read("advTail"));
    tailSmootherTarget = read("advTail");
    outputGainSmoother.reset(sampleRate, 0.025);
    outputGainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(read("outGain"), -100.0f));
    tempoSync.reset();
    tapTempo.reset();
}

juce::AudioProcessorEditor* DRMsmrtvrbAudioProcessor::createEditor()
{
    return new DRMsmrtvrbAudioProcessorEditor(*this);
}

bool DRMsmrtvrbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void DRMsmrtvrbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto channels = std::min(2, getTotalNumInputChannels());
    const auto samples = buffer.getNumSamples();
    for (int channel = getTotalNumOutputChannels(); channel < buffer.getNumChannels(); ++channel)
        buffer.clear(channel, 0, samples);
    if (samples <= 0 || channels <= 0) return;

    const auto target = targetFrom(read("target"));
    const auto engine = engineFrom(read("engine"));
    if (target != activeTarget || engine != activeEngine)
    {
        activeTarget = target;
        activeEngine = engine;
        reverb.setProfile(target, engine);
        macroSmoother.setTarget(SpaceMacro::evaluate(target, engine, read("space")));
    }

    double hostBpm = 0.0;
    bool hasHostBpm = false;
    if (auto* playHead = getPlayHead())
        if (const auto position = playHead->getPosition())
            if (const auto bpm = position->getBpm()) { hostBpm = *bpm; hasHostBpm = true; }

    const auto mode = syncFrom(read("sync"));
    const auto division = divisionFrom(read("division"));
    const double manualBpm = read("bpm");
    const float freeDecay = read("freeDecay");
    const auto timing = tempoSync.calculate(mode, hostBpm, manualBpm, division, freeDecay, engine);
    bpmValue.store(timing.bpm > 0.0 ? timing.bpm : manualBpm, std::memory_order_relaxed);

    AdvancedControls advanced;
    advanced.preDelayEnabled = read("advPreDelayOn") > 0.5f;
    advanced.toneEnabled = read("advToneOn") > 0.5f;
    advanced.widthEnabled = read("advWidthOn") > 0.5f;
    advanced.mixEnabled = read("advMixOn") > 0.5f;
    advanced.tailEnabled = read("advTailOn") > 0.5f;
    advanced.preDelayMs = read("advPreDelay");
    advanced.tone = read("advTone");
    advanced.width = read("advWidth");
    advanced.mix = read("advMix");
    advanced.tailSeconds = mode == SyncMode::free ? read("advTail") : static_cast<float>(timing.durationMilliseconds * 0.001);
    const float nextTailTarget = mode == SyncMode::free ? advanced.tailSeconds
        : static_cast<float>(timing.durationMilliseconds * 0.001);
    if (std::abs(nextTailTarget - tailSmootherTarget) > 0.0001f)
    {
        tailSmootherTarget = nextTailTarget;
        tailSmoother.setTargetValue(tailSmootherTarget);
    }
    hostTempoAvailableValue.store(mode == SyncMode::host && hasHostBpm && !timing.usedFallback,
                                  std::memory_order_relaxed);

    const float space = read("space");
    macroSmoother.setRampMilliseconds(20.0f);
    macroSmoother.setTarget(SpaceMacro::evaluate(target, engine, space));
    std::array<float, 2> inputPeaks {};
    std::array<float, 2> outputPeaks {};
    constexpr int maximumQuantum = 32;
    std::array<AdvancedResult, maximumQuantum> controlBuffer {};
    std::array<float, maximumQuantum> gainBuffer {};
    outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(read("outGain"), -100.0f));
    auto historyWrite = activityWriteCount.load(std::memory_order_relaxed);
    for (int offset = 0; offset < samples;)
    {
        const int count = std::min({ maximumQuantum, maximumBlockSize, samples - offset });
        for (int i = 0; i < count; ++i)
        {
            auto smoothedAdvanced = advanced;
            smoothedAdvanced.tailSeconds = tailSmoother.getNextValue();
            auto result = applyAdvancedControls(macroSmoother.next(), smoothedAdvanced);
            if (mode != SyncMode::free)
            {
                result.reverb.decaySeconds = timing.decaySeconds;
                result.reverb.gateSeconds = timing.gateSeconds;
            }
            controlBuffer[static_cast<std::size_t>(i)] = result;
            gainBuffer[static_cast<std::size_t>(i)] = outputGainSmoother.getNextValue();
        }
        reverb.setParameters(controlBuffer[0].reverb);
        const float* inputPointers[2] {};
        float* wetPointers[2] { wetBuffer[0].data(), wetBuffer[1].data() };
        for (int channel = 0; channel < channels; ++channel)
            inputPointers[channel] = buffer.getReadPointer(channel, offset);
        reverb.process(inputPointers, wetPointers, channels, count);

        float quantumActivity = 0.0f;
        for (int channel = 0; channel < channels; ++channel)
        {
            auto* output = buffer.getWritePointer(channel, offset);
            for (int i = 0; i < count; ++i)
            {
                const float dry = inputPointers[channel][i];
                const float wet = wetBuffer[static_cast<std::size_t>(channel)][static_cast<std::size_t>(i)];
                inputPeaks[static_cast<std::size_t>(channel)] = std::max(inputPeaks[static_cast<std::size_t>(channel)], std::abs(dry));
                const auto& controls = controlBuffer[static_cast<std::size_t>(i)];
                output[i] = (dry * controls.dryGain + wet * controls.wetGain) * gainBuffer[static_cast<std::size_t>(i)];
                outputPeaks[static_cast<std::size_t>(channel)] = std::max(outputPeaks[static_cast<std::size_t>(channel)], std::abs(output[i]));
                quantumActivity = std::max(quantumActivity, std::abs(wet));
            }
        }
        activityHistory[static_cast<std::size_t>(historyWrite % activityHistorySize)].store(quantumActivity, std::memory_order_relaxed);
        ++historyWrite;
        offset += count;
    }
    for (std::size_t channel = 0; channel < 2; ++channel)
    {
        inputPeakValue[channel].store(inputPeaks[channel], std::memory_order_relaxed);
        outputPeakValue[channel].store(outputPeaks[channel], std::memory_order_relaxed);
    }
    activityWriteCount.store(historyWrite, std::memory_order_release);
}

void DRMsmrtvrbAudioProcessor::copyActivityHistory(std::array<float, activityHistorySize>& destination) const noexcept
{
    destination.fill(0.0f);
    const auto written = activityWriteCount.load(std::memory_order_acquire);
    const auto count = std::min<std::uint64_t>(written, activityHistorySize);
    const auto first = written - count;
    const auto outputOffset = activityHistorySize - static_cast<std::size_t>(count);
    for (std::uint64_t i = 0; i < count; ++i)
        destination[outputOffset + static_cast<std::size_t>(i)] =
            activityHistory[static_cast<std::size_t>((first + i) % activityHistorySize)].load(std::memory_order_relaxed);
}

bool DRMsmrtvrbAudioProcessor::isCurrentPresetModified() const noexcept
{
    const auto index = currentPreset.load(std::memory_order_relaxed);
    std::size_t count = 0;
    const auto* presets = factoryPresets(count);
    if (index < 0 || static_cast<std::size_t>(index) >= count) return true;
    const auto current = makeState();
    const auto& expected = presets[index].state;
    const auto close = [](float a, float b) { return std::abs(a - b) < 0.001f; };
    return current.target != expected.target || current.engine != expected.engine ||
        !close(current.spacePercent, expected.spacePercent) || current.syncMode != expected.syncMode ||
        current.division != expected.division || !close(current.freeDecaySeconds, expected.freeDecaySeconds) ||
        current.advanced.preDelayEnabled != expected.advanced.preDelayEnabled ||
        current.advanced.toneEnabled != expected.advanced.toneEnabled ||
        current.advanced.widthEnabled != expected.advanced.widthEnabled ||
        current.advanced.mixEnabled != expected.advanced.mixEnabled ||
        current.advanced.tailEnabled != expected.advanced.tailEnabled ||
        !close(current.advanced.preDelayMs, expected.advanced.preDelayMs) ||
        !close(current.advanced.tone, expected.advanced.tone) ||
        !close(current.advanced.width, expected.advanced.width) ||
        !close(current.advanced.mix, expected.advanced.mix) ||
        !close(current.advanced.tailSeconds, expected.advanced.tailSeconds);
}

void DRMsmrtvrbAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary(*xml, destination);
}

void DRMsmrtvrbAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(parameters.state.getType()))
        {
            currentPreset.store(-1, std::memory_order_relaxed);
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
        }
}

int DRMsmrtvrbAudioProcessor::getNumPrograms()
{
    std::size_t count = 0;
    factoryPresets(count);
    return static_cast<int>(count);
}

void DRMsmrtvrbAudioProcessor::setCurrentProgram(int index)
{
    loadFactoryPreset(index);
}

const juce::String DRMsmrtvrbAudioProcessor::getProgramName(int index)
{
    std::size_t count = 0;
    const auto* presets = factoryPresets(count);
    return index >= 0 && static_cast<std::size_t>(index) < count ? presets[index].name : juce::String();
}

void DRMsmrtvrbAudioProcessor::loadFactoryPreset(int index)
{
    std::size_t count = 0;
    const auto* presets = factoryPresets(count);
    if (index < 0 || static_cast<std::size_t>(index) >= count) return;
    applyState(presets[index].state);
    currentPreset.store(index, std::memory_order_relaxed);
}

drmspace::PluginState DRMsmrtvrbAudioProcessor::makeState() const noexcept
{
    using namespace drmspace;
    PluginState state;
    state.target = targetFrom(read("target"));
    state.engine = engineFrom(read("engine"));
    state.spacePercent = read("space");
    state.syncMode = syncFrom(read("sync"));
    state.division = divisionFrom(read("division"));
    state.freeDecaySeconds = read("freeDecay");
    state.advanced = { read("advPreDelayOn") > 0.5f, read("advToneOn") > 0.5f,
        read("advWidthOn") > 0.5f, read("advMixOn") > 0.5f, read("advTailOn") > 0.5f,
        read("advPreDelay"), read("advTone"), read("advWidth"), read("advMix"), read("advTail") };
    return state;
}

void DRMsmrtvrbAudioProcessor::applyState(const drmspace::PluginState& state)
{
    const auto setChoice = [this](const char* id, float value)
    {
        if (auto* p = parameters.getParameter(id)) p->setValueNotifyingHost(p->convertTo0to1(value));
    };
    const auto setFloat = setChoice;
    const auto setBool = [this](const char* id, bool value)
    {
        if (auto* p = parameters.getParameter(id)) p->setValueNotifyingHost(value ? 1.0f : 0.0f);
    };
    setChoice("target", static_cast<float>(state.target));
    setChoice("engine", static_cast<float>(state.engine));
    setFloat("space", state.spacePercent);
    setChoice("sync", static_cast<float>(state.syncMode));
    setChoice("division", static_cast<float>(state.division));
    setFloat("freeDecay", state.freeDecaySeconds);
    setBool("advPreDelayOn", state.advanced.preDelayEnabled);
    setFloat("advPreDelay", state.advanced.preDelayMs);
    setBool("advToneOn", state.advanced.toneEnabled);
    setFloat("advTone", state.advanced.tone);
    setBool("advWidthOn", state.advanced.widthEnabled);
    setFloat("advWidth", state.advanced.width);
    setBool("advMixOn", state.advanced.mixEnabled);
    setFloat("advMix", state.advanced.mix);
    setBool("advTailOn", state.advanced.tailEnabled);
    setFloat("advTail", state.advanced.tailSeconds);
}

void DRMsmrtvrbAudioProcessor::tapTempoNow()
{
    if (const auto bpm = tapTempo.tap(juce::Time::getMillisecondCounterHiRes()))
        if (auto* p = parameters.getParameter("bpm"))
            p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(*bpm)));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DRMsmrtvrbAudioProcessor();
}
