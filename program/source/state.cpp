#include "state.hpp"

#include <exception>
#include <filesystem>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "exception.hpp"

namespace csr::base
{
  bool resource::operator<(const resource &other) const { return path < other.path; }

  state::state(int argc, char *argv[])
  {
    if (argc < 3)
      throw utility::exception("Usage:\n"
                               " - CSResource compile (<resource1> <resource2> ... <resourceN>) "
                               "<output_shader_directory> <output_include_directory> <output_source_directory>\n"
                               " - CSResource list <texture_directory>\n"
                               " - CSResource set <texture> <frame_width>");

    if (std::string(argv[1]) == "compile")
    {
      task = COMPILE;
      if (argc < 6)
        throw utility::exception("Compile: CSResource compile (<resource1> <resource2> ... <resourceN>) "
                                 "<output_shader_directory> <output_include_directory> <output_source_directory>");

      std::vector<std::filesystem::path> output_directories;
      std::set<resource> unique_resources;
      for (int index = 2; index < argc; ++index)
      {
        if (index == argc - 3)
        {
          output_shader_directory = argv[index];
          output_directories.emplace_back(output_shader_directory);
        }
        else if (index == argc - 2)
        {
          output_include_directory = argv[index];
          output_directories.emplace_back(output_include_directory);
        }
        else if (index == argc - 1)
        {
          output_source_directory = argv[index];
          output_directories.emplace_back(output_source_directory);
        }
        else
        {
          std::filesystem::path resource = argv[index];
          if (!std::filesystem::exists(resource) || !std::filesystem::is_regular_file(resource))
            throw utility::exception("Resource does not exist or is not a valid file {}", resource.string());
          if (resource.extension() == ".vert" || resource.extension() == ".frag")
            unique_resources.emplace(resource::SHADER, std::move(resource));
          else if (resource.extension() == ".png")
            unique_resources.emplace(resource::TEXTURE, std::move(resource));
          else
            throw utility::exception("Unsupported resource type {}", resource.extension().string());
        }
      }
      for (const resource &resource : unique_resources) { resources.emplace_back(resource.type, resource.path); }
    }
    else if (std::string(argv[1]) == "list")
    {
      task = LIST;
      if (argc != 3) throw utility::exception("List: CSResource list <texture_directory>");
      texture_directory = argv[2];
    }
    else if (std::string(argv[1]) == "set")
    {
      task = SET;
      if (argc != 6)
        throw utility::exception("Set: CSResource set <texture> <frame_width> <frame_height> <frame_count>");
      texture_path = argv[2];
      try
      {
        frame_width = std::stoul(argv[3]);
        frame_height = std::stoul(argv[4]);
        frame_count = std::stoul(argv[5]);
      }
      catch (const std::exception &error)
      {
        throw utility::exception("Invalid frame dimensions or count: {}x{} {}f. Error: {}", std::string(argv[3]),
                                 argv[4], argv[5], error.what());
      }
    }
    else
      throw utility::exception("Invalid task {}", std::string(argv[1]));
  }
}
