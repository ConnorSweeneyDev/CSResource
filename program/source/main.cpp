#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_shadercross/SDL_shadercross.h"

int main(int argc, char *argv[])
{
  if (argc > 1 || !argv)
  {
    std::cerr << "Usage: " << argv[0] << std::endl;
    return EXIT_FAILURE;
  }

  if (!SDL_ShaderCross_Init())
  {
    SDL_LogError(SDL_LOG_CATEGORY_GPU, "%s", "Failed to initialize shadercross!");
    return EXIT_FAILURE;
  }

  size_t file_size = 0;
  char *file_data = reinterpret_cast<char *>(SDL_LoadFile("resource/shader/main.vert", &file_size));
  if (!file_data)
  {
    SDL_LogError(SDL_LOG_CATEGORY_GPU, "Failed to load shader file: %s", SDL_GetError());
    SDL_free(file_data);
    SDL_ShaderCross_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_ShaderCross_HLSL_Info hlsl_info = {};
  hlsl_info.source = reinterpret_cast<char *>(file_data);
  hlsl_info.entrypoint = "main";
  hlsl_info.include_dir = nullptr;
  hlsl_info.defines = nullptr;
  hlsl_info.shader_stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
  hlsl_info.enable_debug = false;
  hlsl_info.name = nullptr;
  hlsl_info.props = 0;

  size_t compiled_size = 0;
  Uint8 *compiled_shader = reinterpret_cast<Uint8 *>(SDL_ShaderCross_CompileDXILFromHLSL(&hlsl_info, &compiled_size));
  if (!compiled_shader)
  {
    SDL_LogError(SDL_LOG_CATEGORY_GPU, "Failed to compile shader: %s", SDL_GetError());
    SDL_free(file_data);
    SDL_ShaderCross_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }
  std::filesystem::create_directories("build/Output");
  SDL_IOStream *output = SDL_IOFromFile("build/Output/main.vert.dxil", "w");
  if (!output)
  {
    SDL_LogError(SDL_LOG_CATEGORY_GPU, "Failed to open output file: %s", SDL_GetError());
    SDL_free(file_data);
    SDL_free(compiled_shader);
    SDL_ShaderCross_Quit();
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_WriteIO(output, compiled_shader, compiled_size);
  SDL_CloseIO(output);

  std::cout << "Finished!" << std::endl;
  return EXIT_SUCCESS;
}
