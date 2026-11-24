#pragma once

#include <JuceHeader.h>
#include "PluginState.h"
#include "TargetProfiles.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

class DRMsmrtvrbAudioProcessor final : public juce::AudioProcessor
{
public:
    DRMsmrtvrbAudioProcessor();
    void prepareToPlay(double sampleRate, int maximumBlockSize) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "DRM-SMRTVRB"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }
    int getNumPrograms() override;
    int getCurrentProgram() override { return juce::jmax(0, currentPreset.load()); }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& destination) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void loadFactoryPreset(int index);
    void tapTempoNow();
    float inputPeak(int channel) const noexcept { return inputPeakValue[static_cast<std::size_t>(juce::jlimit(0, 1, channel))].load(std::memory_order_relaxed); }
    float outputPeak(int channel) const noexcept { return outputPeakValue[static_cast<std::size_t>(juce::jlimit(0, 1, channel))].load(std::memory_order_relaxed); }
    static constexpr std::size_t activityHistorySize = 64;
    void copyActivityHistory(std::array<float, activityHistorySize>& destination) const noexcept;
    bool isCurrentPresetModified() const noexcept;
    int presetIndexForDisplay() const noexcept { return currentPreset.load(std::memory_order_relaxed); }
    double displayedBpm() const noexcept { return bpmValue.load(std::memory_order_relaxed); }
    bool hostTempoAvailable() const noexcept { return hostTempoAvailableValue.load(std::memory_order_relaxed); }

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    using RawParameter = std::atomic<float>*;
    RawParameter parameter(const char* id) const noexcept;
    float read(const char* id) const noexcept;
    drmspace::PluginState makeState() const noexcept;
    void applyState(const drmspace::PluginState& state);

    drmspace::ProfiledReverb reverb;
    drmspace::SpaceMacroSmoother macroSmoother;
    drmspace::TempoSync tempoSync;
    drmspace::TapTempo tapTempo;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tailSmoother;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGainSmoother;
    std::array<std::vector<float>, 2> wetBuffer;
    std::array<std::atomic<float>, 2> inputPeakValue {};
    std::array<std::atomic<float>, 2> outputPeakValue {};
    std::array<std::atomic<float>, activityHistorySize> activityHistory {};
    std::atomic<std::uint64_t> activityWriteCount { 0 };
    std::atomic<double> bpmValue { 120.0 };
    std::atomic<bool> hostTempoAvailableValue { false };
    std::atomic<int> currentPreset { 0 };
    int maximumBlockSize = 1;
    float tailSmootherTarget = 1.2f;
    drmspace::SourceTarget activeTarget = drmspace::SourceTarget::shells;
    drmspace::Engine activeEngine = drmspace::Engine::room;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DRMsmrtvrbAudioProcessor)
};
