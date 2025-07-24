include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Manage.cmake")
set(SDL_VERSION "3.2.16")
set(SDL_SHADERCROSS_VERSION "392d12a")
set(STB_VERSION "f58f558")

CPMAddPackage(
  URI
  "gh:libsdl-org/SDL#release-${SDL_VERSION}"
  OPTIONS "SDL_INSTALL OFF" "SDL_UNINSTALL OFF" "SDL_INSTALL_TESTS OFF" "SDL_TESTS OFF"
)
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-src/include")
list(APPEND LIBRARIES "SDL3-static")
install(TARGETS "SDL3-static" "SDL3_Headers" EXPORT "LLVMExports")

CPMAddPackage(
  URI
  "gh:libsdl-org/SDL_shadercross#${SDL_SHADERCROSS_VERSION}"
  OPTIONS
    "SDLSHADERCROSS_VENDORED ON"
    "SDLSHADERCROSS_SHARED OFF"
    "SDLSHADERCROSS_SPIRVCROSS_SHARED OFF"
    "SDLSHADERCROSS_STATIC ON"
    "SDLSHADERCROSS_CLI OFF"
)
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/_deps/SDL_shadercross-src/include")
list(APPEND LIBRARIES "SDL3_shadercross-static")

set(
  DXCOMPILER_DLL_SOURCE
  "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl_shadercross-build/external/DirectXShaderCompiler/${CMAKE_BUILD_TYPE}/bin/dxcompiler.dll"
)
set(DXCOMPILER_DLL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/dxcompiler.dll")
add_custom_target(
  "CopyDXCompiler"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${DXCOMPILER_DLL_SOURCE}" "${DXCOMPILER_DLL_DESTINATION}"
  DEPENDS "SDL3_shadercross-static"
)
list(APPEND DEPENDENCIES "CopyDXCompiler")

set(STB_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/stb-src")
set(STB_IMAGE "${STB_DIRECTORY}/stb_image.h")
if(NOT EXISTS "${STB_IMAGE}")
  file(DOWNLOAD "https://github.com/nothings/stb/blob/${STB_VERSION}/stb_image.h?raw=true" "${STB_IMAGE}")
endif()
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${STB_DIRECTORY}")
