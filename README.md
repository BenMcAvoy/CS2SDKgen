# CS2SDKgen

CS2SDKgen is a small Windows C++ experiment with process inspection and memory-reading helpers. The current program looks for `cs2.exe`, reads the first 16-bit value at its module base and prints it.

## Build

Open `CS2SDK.sln` in a recent version of Visual Studio and build the x64 configuration.

## Status

Despite the repository name, SDK generation is not implemented yet. This is an early reverse-engineering and process-inspection experiment, not a complete SDK generator.
