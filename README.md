# Frost Reverb

Shimmer reverb plugin.

## Instructions

### Prerequisites
- JUCE installed

### Build

1. Update path to JUCE on this line in `CMakeLists.txt`
    ```
    add_subdirectory("C:/JUCE" "JUCE")
    ```
2. Run `cmake -B build`
3. Run `cmake --build build --config Release`
4. VST3 file (folder) and standalone executable will be in `build\FrostReverb_artefacts\Release`