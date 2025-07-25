#include <array>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3_shadercross/SDL_shadercross.h"

#include "main.hpp"

int try_main(int argc, char *argv[])
{
  for (int index = 0; index < argc; ++index) args.emplace_back(argv[index]);
  if (args.size() < 5) throw std::runtime_error(usage);

  std::filesystem::path output_shader_directory = args.at(args.size() - 3);
  std::filesystem::path output_include_directory = args.at(args.size() - 2);
  std::filesystem::path output_source_directory = args.back();
  std::array<std::filesystem::path, 3> output_directories = {output_shader_directory, output_include_directory,
                                                             output_source_directory};
  try
  {
    for (const std::filesystem::path &output_directory : output_directories)
      std::filesystem::create_directories(output_directory);
  }
  catch (const std::filesystem::filesystem_error &error)
  {
    std::string directories = "[";
    for (const std::filesystem::path &output_directory : output_directories)
      directories += output_directory.string() + (output_directories.back() == output_directory ? "" : ", ");
    directories += "]";
    throw std::runtime_error("Failed to create output directories " + directories + ": " + std::string(error.what()));
  }

  resources.reserve(args.size() - 4);
  for (size_t index = 1; index < args.size() - 3; ++index)
  {
    std::filesystem::path resource = args.at(index);
    if (!std::filesystem::exists(resource) || !std::filesystem::is_regular_file(resource) ||
        (resource.extension() != ".vert" && resource.extension() != ".frag" && resource.extension() != ".png"))
      throw std::runtime_error("Shader does not exist or is not a valid vert, frag or png file: " + resource.string());
    resources.emplace_back(resource);
  }

  if (!SDL_ShaderCross_Init())
    throw std::runtime_error("Failed to initialize shadercross: " + std::string(SDL_GetError()));

  for (const std::filesystem::path &resource : resources)
  {
    if (resource.extension() == ".png") continue; // Not implemented yet.

    size_t file_size = 0;
    char *file_data = reinterpret_cast<char *>(SDL_LoadFile(resource.string().c_str(), &file_size));
    if (!file_data)
    {
      SDL_ShaderCross_Quit();
      SDL_Quit();
      throw std::runtime_error("Failed to load shader " + resource.string() + ": " + std::string(SDL_GetError()));
    }

    SDL_ShaderCross_HLSL_Info hlsl_info = {reinterpret_cast<char *>(file_data),
                                           "main",
                                           nullptr,
                                           nullptr,
                                           resource.extension() == ".vert" ? SDL_SHADERCROSS_SHADERSTAGE_VERTEX
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
      throw std::runtime_error("Failed to compile dxil shader " + resource.string() + ": " +
                               std::string(SDL_GetError()));
    }

    SDL_IOStream *dxil_output =
      SDL_IOFromFile((output_shader_directory / resource.filename()).string().append(".dxil").c_str(), "w");
    if (!dxil_output)
    {
      SDL_free(compiled_dxil_shader);
      SDL_ShaderCross_Quit();
      SDL_Quit();
      throw std::runtime_error("Failed to open dxil output file for shader " + resource.string() + ": " +
                               std::string(SDL_GetError()));
    }
    SDL_WriteIO(dxil_output, compiled_dxil_shader, compiled_dxil_size);
    SDL_CloseIO(dxil_output);

    size_t compiled_spirv_size = 0;
    Uint8 *compiled_spirv_shader =
      reinterpret_cast<Uint8 *>(SDL_ShaderCross_CompileSPIRVFromHLSL(&hlsl_info, &compiled_spirv_size));
    if (!compiled_spirv_shader)
    {
      SDL_free(compiled_dxil_shader);
      SDL_ShaderCross_Quit();
      SDL_Quit();
      throw std::runtime_error("Failed to compile spirv shader " + resource.string() + ": " +
                               std::string(SDL_GetError()));
    }

    SDL_IOStream *spirv_output =
      SDL_IOFromFile((output_shader_directory / resource.filename()).string().append(".spv").c_str(), "w");
    if (!spirv_output)
    {
      SDL_free(compiled_dxil_shader);
      SDL_free(compiled_spirv_shader);
      SDL_ShaderCross_Quit();
      SDL_Quit();
      throw std::runtime_error("Failed to open spirv output file for shader " + resource.string() + ": " +
                               std::string(SDL_GetError()));
    }
    SDL_WriteIO(spirv_output, compiled_spirv_shader, compiled_spirv_size);
    SDL_CloseIO(spirv_output);

    include_content += "  extern const compiled_shader " + resource.stem().string() +
                       ((resource.extension() == ".vert") ? "_vertex" : "_fragment") + ";\n";
    source_content += "  const compiled_shader " + resource.stem().string() +
                      ((resource.extension() == ".vert") ? "_vertex" : "_fragment") + " = {\n    {";
    for (size_t index = 0; index < compiled_dxil_size; ++index)
    {
      source_content += unsigned_char_to_hex(compiled_dxil_shader[index]);
      if (index < compiled_dxil_size - 1)
      {
        source_content += ",";
        if ((index + 1) % 16 == 0)
          source_content += "\n     ";
        else
          source_content += " ";
      }
    }
    source_content += "},\n    {";
    for (size_t index = 0; index < compiled_spirv_size; ++index)
    {
      source_content += unsigned_char_to_hex(compiled_spirv_shader[index]);
      if (index < compiled_spirv_size - 1)
      {
        source_content += ",";
        if ((index + 1) % 16 == 0)
          source_content += "\n     ";
        else
          source_content += " ";
      }
    }
    source_content += "}};\n";

    SDL_free(compiled_dxil_shader);
    SDL_free(compiled_spirv_shader);
    SDL_free(file_data);
  }
  include_content += "}";
  source_content += "}";

  SDL_IOStream *include_output = SDL_IOFromFile((output_include_directory / "resource.hpp").string().c_str(), "w");
  if (!include_output)
  {
    SDL_ShaderCross_Quit();
    SDL_Quit();
    throw std::runtime_error("Failed to open include output file: " + std::string(SDL_GetError()));
  }
  SDL_WriteIO(include_output, include_content.c_str(), include_content.size());
  SDL_CloseIO(include_output);

  SDL_IOStream *source_output = SDL_IOFromFile((output_source_directory / "resource.cpp").string().c_str(), "w");
  if (!source_output)
  {
    SDL_ShaderCross_Quit();
    SDL_Quit();
    throw std::runtime_error("Failed to open source output file: " + std::string(SDL_GetError()));
  }
  SDL_WriteIO(source_output, source_content.c_str(), source_content.size());
  SDL_CloseIO(source_output);

  SDL_ShaderCross_Quit();
  SDL_Quit();
  return EXIT_SUCCESS;
}

std::string unsigned_char_to_hex(unsigned char character)
{
  std::stringstream ss;
  ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(character);
  return ss.str();
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
