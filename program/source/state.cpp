#include "state.hpp"

#include <cstddef>
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
      throw utility::exception(
        "Usage:\n"
        " - CSResource compile (<resource1> <resource2> ... <resourceN>) "
        "<output_shader_directory> <output_include_directory> <output_source_directory>\n"
        " - CSResource list <texture_directory>\n"
        " - CSResource set <texture> <frame_width> <frame_height> (<name1>:<start_frame1>:<end_frame1> "
        "<name2>:<start_frame2>:<end_frame2> ... <nameN>:<start_frameN>:<end_frameN>)");

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
      if (argc < 6)
        throw utility::exception(
          "Set: CSResource set <texture> <frame_width> <frame_height> (<name1>:<start_frame1>:<end_frame1> "
          "<name2>:<start_frame2>:<end_frame2> ... <nameN>:<start_frameN>:<end_frameN>)");
      texture_path = argv[2];
      try
      {
        frame_data.width = std::stoul(argv[3]);
        frame_data.height = std::stoul(argv[4]);
      }
      catch (const std::exception &error)
      {
        throw utility::exception("Invalid frame dimensions: {}x{}. Error: {}", std::string(argv[3]), argv[4],
                                 error.what());
      }

      for (size_t index = 5; index < static_cast<size_t>(argc); ++index)
      {
        std::string argument = argv[index];
        size_t first_colon = argument.find(':');
        size_t second_colon = argument.find(':', first_colon + 1);
        if (first_colon == std::string::npos || second_colon == std::string::npos || first_colon == second_colon)
          throw utility::exception("Invalid frame group specification {}", argument);
        std::string name = argument.substr(0, first_colon);
        unsigned int starting_frame = {};
        unsigned int last_frame = {};
        try
        {
          starting_frame = std::stoul(argument.substr(first_colon + 1, second_colon));
          last_frame = std::stoul(argument.substr(second_colon + 1));
        }
        catch (const std::exception &error)
        {
          throw utility::exception("Invalid frame group specification {}. Error: {}", argument, error.what());
        }
        for (const auto &group : frame_data.groups)
          if (group.name == name) throw utility::exception("Duplicate frame group name {}", name);
        frame_data.groups.push_back({std::move(name), starting_frame, last_frame});
      }
    }
    else
      throw utility::exception("Invalid task {}", std::string(argv[1]));
  }
}
