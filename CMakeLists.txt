cmake_minimum_required(VERSION 3.22)
project(DRM_SMRTVRB VERSION 0.2.0 LANGUAGES C CXX)

option(DRMSMRTVRB_BUILD_TESTS "Build DSP validation tests" ON)
option(DRMSMRTVRB_BUILD_PLUGIN "Build the JUCE VST3 and standalone plugin" ON)

if(DRMSMRTVRB_BUILD_PLUGIN)
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.6
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)

    juce_add_plugin(DRMSMRTVRB
        COMPANY_NAME "yeahwaah audio"
        BUNDLE_ID "audio.yeahwaah.drmsmrtvrb"
        PLUGIN_MANUFACTURER_CODE Ywaa
        PLUGIN_CODE SmRv
        PRODUCT_NAME "DRM-SMRTVRB"
        DESCRIPTION "Source-aware drum reverb"
        FORMATS VST3 Standalone
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE)
    juce_generate_juce_header(DRMSMRTVRB)
    target_sources(DRMSMRTVRB PRIVATE
        Source/PluginProcessor.cpp Source/PluginProcessor.h
        Source/PluginEditor.cpp Source/PluginEditor.h)
    target_compile_features(DRMSMRTVRB PRIVATE cxx_std_17)
    target_compile_definitions(DRMSMRTVRB PUBLIC
        JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
    target_link_libraries(DRMSMRTVRB PRIVATE
        drmsmrtvrb_dsp juce::juce_audio_utils juce::juce_dsp
        PUBLIC juce::juce_recommended_config_flags
               juce::juce_recommended_lto_flags
               juce::juce_recommended_warning_flags)

    juce_add_console_app(drmsmrtvrb_plugin_benchmark PRODUCT_NAME "drmsmrtvrb_plugin_benchmark")
    juce_generate_juce_header(drmsmrtvrb_plugin_benchmark)
    target_sources(drmsmrtvrb_plugin_benchmark PRIVATE
        Tools/PluginBenchmark.cpp
        Source/PluginProcessor.cpp Source/PluginProcessor.h
        Source/PluginEditor.cpp Source/PluginEditor.h)
    target_compile_features(drmsmrtvrb_plugin_benchmark PRIVATE cxx_std_17)
    target_compile_definitions(drmsmrtvrb_plugin_benchmark PRIVATE
        JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(drmsmrtvrb_plugin_benchmark PRIVATE
        drmsmrtvrb_dsp juce::juce_audio_utils juce::juce_dsp
        PUBLIC juce::juce_recommended_config_flags)

    juce_add_console_app(drmsmrtvrb_soak_test PRODUCT_NAME "drmsmrtvrb_soak_test")
    juce_generate_juce_header(drmsmrtvrb_soak_test)
    target_sources(drmsmrtvrb_soak_test PRIVATE
        Tools/SoakTest.cpp
        Source/PluginProcessor.cpp Source/PluginProcessor.h
        Source/PluginEditor.cpp Source/PluginEditor.h)
    target_compile_features(drmsmrtvrb_soak_test PRIVATE cxx_std_17)
    target_compile_definitions(drmsmrtvrb_soak_test PRIVATE
        JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(drmsmrtvrb_soak_test PRIVATE
        drmsmrtvrb_dsp juce::juce_audio_utils juce::juce_dsp
        PUBLIC juce::juce_recommended_config_flags)
    if(DRMSMRTVRB_BUILD_TESTS)
        enable_testing()
        add_test(NAME PluginSoak COMMAND drmsmrtvrb_soak_test 10)
    endif()
endif()

add_library(drmsmrtvrb_dsp STATIC
    Source/ReverbEngines.cpp
    Source/ReverbEngines.h
    Source/SpaceMacro.cpp
    Source/SpaceMacro.h
    Source/PluginState.cpp
    Source/PluginState.h
    Source/TargetProfiles.cpp
    Source/TargetProfiles.h
    Source/TempoSync.cpp
    Source/TempoSync.h)
target_include_directories(drmsmrtvrb_dsp PUBLIC Source)
target_compile_features(drmsmrtvrb_dsp PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(drmsmrtvrb_dsp PRIVATE /W4 /permissive-)
else()
    target_compile_options(drmsmrtvrb_dsp PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(drmsmrtvrb_wav STATIC Tools/WavFile.cpp Tools/WavFile.h)
target_include_directories(drmsmrtvrb_wav PUBLIC Tools)
target_compile_features(drmsmrtvrb_wav PUBLIC cxx_std_17)

add_library(drmsmrtvrb_metrics STATIC Tools/AudioMetrics.cpp Tools/AudioMetrics.h)
target_link_libraries(drmsmrtvrb_metrics PUBLIC drmsmrtvrb_wav)
target_compile_features(drmsmrtvrb_metrics PUBLIC cxx_std_17)

add_executable(drmsmrtvrb_analyze Tools/AnalyzeWav.cpp)
target_link_libraries(drmsmrtvrb_analyze PRIVATE drmsmrtvrb_metrics)
target_compile_features(drmsmrtvrb_analyze PRIVATE cxx_std_17)

add_executable(drmsmrtvrb_render Tools/RenderWav.cpp)
target_link_libraries(drmsmrtvrb_render PRIVATE drmsmrtvrb_dsp drmsmrtvrb_wav)
target_compile_features(drmsmrtvrb_render PRIVATE cxx_std_17)

add_executable(drmsmrtvrb_benchmark Tools/Benchmark.cpp)
target_link_libraries(drmsmrtvrb_benchmark PRIVATE drmsmrtvrb_dsp)
target_compile_features(drmsmrtvrb_benchmark PRIVATE cxx_std_17)

if(DRMSMRTVRB_BUILD_TESTS)
    enable_testing()
    add_executable(drmsmrtvrb_dsp_tests Tests/ReverbEnginesTests.cpp)
    target_link_libraries(drmsmrtvrb_dsp_tests PRIVATE drmsmrtvrb_dsp)
    add_test(NAME ReverbEngines COMMAND drmsmrtvrb_dsp_tests)
    add_executable(drmsmrtvrb_profile_tests Tests/TargetProfilesTests.cpp)
    target_link_libraries(drmsmrtvrb_profile_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_profile_tests PRIVATE cxx_std_17)
    add_test(NAME TargetProfiles COMMAND drmsmrtvrb_profile_tests)
    add_executable(drmsmrtvrb_space_tests Tests/SpaceMacroTests.cpp)
    target_link_libraries(drmsmrtvrb_space_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_space_tests PRIVATE cxx_std_17)
    add_test(NAME SpaceMacro COMMAND drmsmrtvrb_space_tests)
    add_executable(drmsmrtvrb_tempo_tests Tests/TempoSyncTests.cpp)
    target_link_libraries(drmsmrtvrb_tempo_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_tempo_tests PRIVATE cxx_std_17)
    add_test(NAME TempoSync COMMAND drmsmrtvrb_tempo_tests)
    add_executable(drmsmrtvrb_state_tests Tests/PluginStateTests.cpp)
    target_link_libraries(drmsmrtvrb_state_tests PRIVATE drmsmrtvrb_dsp)
    target_compile_features(drmsmrtvrb_state_tests PRIVATE cxx_std_17)
    add_test(NAME PluginState COMMAND drmsmrtvrb_state_tests)
    add_executable(drmsmrtvrb_wav_tests Tests/WavFileTests.cpp)
    target_link_libraries(drmsmrtvrb_wav_tests PRIVATE drmsmrtvrb_wav)
    target_compile_features(drmsmrtvrb_wav_tests PRIVATE cxx_std_17)
    add_test(NAME WavFile COMMAND drmsmrtvrb_wav_tests)
    add_executable(drmsmrtvrb_metrics_tests Tests/AudioMetricsTests.cpp)
    target_link_libraries(drmsmrtvrb_metrics_tests PRIVATE drmsmrtvrb_metrics)
    target_compile_features(drmsmrtvrb_metrics_tests PRIVATE cxx_std_17)
    add_test(NAME AudioMetrics COMMAND drmsmrtvrb_metrics_tests)
endif()
