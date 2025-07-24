#include <cstddef>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_shadercross/SDL_shadercross.h"

int try_main(int argc, char *argv[])
{
  const std::string usage =
    "Usage: CSResource [compile <shaders> <output_directory>] | [load <resources> <output>(.hpp|.cpp)]";

  std::vector<std::string> args;
  for (int index = 0; index < argc; ++index) args.emplace_back(argv[index]);
  if (args.size() == 1) throw std::runtime_error(usage);

  std::string command = args.at(1);
  if (command != "compile" && command != "load") throw std::runtime_error(usage);

  if (command == "compile")
  {
    if (args.size() < 4) throw std::runtime_error(usage);

    std::filesystem::path output_directory = args.back();
    try
    {
      std::filesystem::create_directories(output_directory);
    }
    catch (const std::filesystem::filesystem_error &error)
    {
      SDL_ShaderCross_Quit();
      SDL_Quit();
      throw std::runtime_error("Failed to create output directory " + output_directory.string() + ": " +
                               std::string(error.what()));
    }

    std::vector<std::filesystem::path> shaders = {};
    shaders.reserve(args.size() - 2);
    for (size_t index = 2; index < args.size() - 1; ++index)
    {
      std::filesystem::path shader = args.at(index);
      if (!std::filesystem::exists(shader) || !std::filesystem::is_regular_file(shader) ||
          (shader.extension() != ".vert" && shader.extension() != ".frag"))
        throw std::runtime_error("Shader does not exist or is not a valid vert or frag file: " + shader.string());
      shaders.emplace_back(shader);
    }

    if (!SDL_ShaderCross_Init())
      throw std::runtime_error("Failed to initialize shadercross: " + std::string(SDL_GetError()));

    for (std::filesystem::path shader : shaders)
    {
      size_t file_size = 0;
      char *file_data = reinterpret_cast<char *>(SDL_LoadFile(shader.string().c_str(), &file_size));
      if (!file_data)
      {
        SDL_ShaderCross_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to load shader " + shader.string() + ": " + std::string(SDL_GetError()));
      }

      SDL_ShaderCross_HLSL_Info hlsl_info = {reinterpret_cast<char *>(file_data),
                                             "main",
                                             nullptr,
                                             nullptr,
                                             shader.extension() == ".vert" ? SDL_SHADERCROSS_SHADERSTAGE_VERTEX
                                                                           : SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT,
                                             false,
                                             nullptr,
                                             0};
      size_t compiled_dxil_size = 0;
      Uint8 *compiled_dxil_shader =
        reinterpret_cast<Uint8 *>(SDL_ShaderCross_CompileDXILFromHLSL(&hlsl_info, &compiled_dxil_size));
      if (!compiled_dxil_shader)
      {
        SDL_ShaderCross_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to compile dxil shader " + shader.string() + ": " +
                                 std::string(SDL_GetError()));
      }

      SDL_IOStream *dxil_output =
        SDL_IOFromFile((output_directory / shader.filename()).string().append(".dxil").c_str(), "w");
      if (!dxil_output)
      {
        SDL_free(compiled_dxil_shader);
        SDL_ShaderCross_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to open dxil output file for shader " + shader.string() + ": " +
                                 std::string(SDL_GetError()));
      }
      SDL_WriteIO(dxil_output, compiled_dxil_shader, compiled_dxil_size);

      SDL_CloseIO(dxil_output);
      SDL_free(compiled_dxil_shader);

      size_t compiled_spv_size = 0;
      Uint8 *compiled_spv_shader =
        reinterpret_cast<Uint8 *>(SDL_ShaderCross_CompileSPIRVFromHLSL(&hlsl_info, &compiled_spv_size));
      if (!compiled_spv_shader)
      {
        SDL_ShaderCross_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to compile spirv shader " + shader.string() + ": " +
                                 std::string(SDL_GetError()));
      }

      SDL_IOStream *spv_output =
        SDL_IOFromFile((output_directory / shader.filename()).string().append(".spv").c_str(), "w");
      if (!spv_output)
      {
        SDL_free(compiled_spv_shader);
        SDL_ShaderCross_Quit();
        SDL_Quit();
        throw std::runtime_error("Failed to open spirv output file for shader " + shader.string() + ": " +
                                 std::string(SDL_GetError()));
      }
      SDL_WriteIO(spv_output, compiled_spv_shader, compiled_spv_size);
      SDL_CloseIO(spv_output);
      SDL_free(compiled_spv_shader);

      SDL_free(file_data);
    }
  }

  std::cout << "Finished!" << std::endl;
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  try
  {
    return try_main(argc, argv);
  }
  catch (const std::exception &error)
  {
    std::cerr << "Exception: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
}
