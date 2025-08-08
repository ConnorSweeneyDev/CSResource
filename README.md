# CSResource
An executable for loading resources into a C++ compilation unit, intended for use in
[CSEngine](https://github.com/ConnorSweeneyDev/CSEngine).

# How to Use
- `CSResource compile (<resource1> <resource2> ... <resourceN>) <output_shader_directory> <output_include_directory>
  <output_source_directory>`: Compiles the given resources (shaders and textures) into a C++ compilation unit
  (resource.hpp and resource.cpp).
- `CSResource list <texture_directory>`: Lists the custom metadata values of all textures in the given directory
  recursively.
- `CSResource set <texture> <frame_width>`: Sets the frame width (int) of the given texture atlas stored as a custom png
  metadata value with header cSRe.

# How to Build
This project is optimized to be built on Windows using MSVC.

1. Ensure that you have [MSVC](https://visualstudio.microsoft.com/downloads/) installed.
2. Ensure that you have [CMake](https://cmake.org/download/) installed, you can run `winget install Kitware.CMake` if
   you don't.
3. Ensure that you have [LLVM](https://releases.llvm.org/) installed, you can run `winget install LLVM.LLVM` and put the
   install location in your environment variables if you don't (for language server and clang-format support).
4. Execute `script/build.sh` followed by `script/run.sh`.

# How to Update Dependencies
All dependencies are managed by CPM.cmake. After changing the version of a dependency, do a full clean build using
`script/clean.sh` before `script/build.sh`.

### [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake/releases)
Change the `CPM_VERSION` variable inside `cmake/Manage.cmake` to the desired version.

### [SDL](https://github.com/libsdl-org/SDL/releases), [SDL_Shadercross](https://github.com/libsdl-org/SDL_shadercross) and [STB](https://github.com/nothings/stb)
Change the `[DEPENDECY]_VERSION` variable inside `cmake/Dependency.cmake` to the desired version.
