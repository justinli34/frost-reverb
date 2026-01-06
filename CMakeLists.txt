cmake_minimum_required(VERSION 3.22)

project(FROST_REVERB VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/JUCE)

juce_add_plugin(FrostReverb
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME "JustinLi"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE JuLi
    PLUGIN_CODE Fre1
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Frost Reverb")

juce_generate_juce_header(FrostReverb)

target_sources(FrostReverb
    PRIVATE
        src/PluginEditor.h
        src/PluginEditor.cpp
        src/PluginProcessor.h
        src/PluginProcessor.cpp)

target_include_directories(FrostReverb
    PUBLIC
        include)

target_compile_definitions(FrostReverb
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(FrostReverb
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
