# ❄ Frost Reverb

Shimmer reverb audio effect plugin. Adds two adjustable pitch-shifted layers, passes them through reverb, and combines with the original dry signal.

## Download Instructions

You can download the plugin for Windows and macOS through the [GitHub releases](https://github.com/justinli34/frost-reverb/releases).

Note: For macOS you will need to run this command in your terminal after unzipping to remove Apple's quarantine attribute since the programs are unsigned:
```sh
xattr -cr /path/to/FrostReverb-v0.0.1-macOS-Universal  # Replace version with your version
```

Alternatively, you can follow the build instructions below to build the app from scratch.

## Build Instructions

Clone the repo:
```sh
git clone --recurse-submodules https://github.com/justinli34/frost-reverb.git
```

Configure the project, generate build files, and compile:
```sh
make build
```

VST3, AU (if on Mac), and standalone executable will be created in `build/FrostReverb_artefacts/Release/`.

## License

This project is licensed under the AGPL-3.0 license.

Music or audio created using this plugin is not affected by this license. You retain full rights to your creative output and can distribute or sell it without any restrictions.
