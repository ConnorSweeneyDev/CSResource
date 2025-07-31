#include "state.hpp"

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
    if (argc < 5)
      throw utility::exception("Usage: CSResource (<resource1> <resource2> ... <resourceN>) <output_shader_directory> "
                               "<output_include_directory> <output_source_directory>]");

    std::vector<std::filesystem::path> output_directories;
    std::set<resource> unique_resources;
    for (int index = 1; index < argc; ++index)
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
          throw utility::exception("Resource does not exist or is not a valid file: " + resource.string());
        if (resource.extension() == ".vert" || resource.extension() == ".frag")
          unique_resources.emplace(resource::SHADER, std::move(resource));
        else if (resource.extension() == ".png")
          throw utility::exception("Texture resources are not implemented yet: " + resource.string());
        else
          throw utility::exception("Unsupported resource type: " + resource.extension().string());
      }
    }
    for (const resource &resource : unique_resources) { resources.emplace_back(resource.type, resource.path); }
  }
}
