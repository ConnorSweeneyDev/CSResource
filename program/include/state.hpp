#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace csr
{
  struct resource
  {
    enum type
    {
      INVALID,
      SHADER,
      TEXTURE
    };
    bool operator<(const resource &other) const;
    type type = INVALID;
    std::filesystem::path path = "";
    std::string include_text = "";
    std::string source_text = "";
  };

  struct state
  {
    state(int argc, char *argv[]);

    std::filesystem::path output_shader_directory = "";
    std::filesystem::path output_include_directory = "";
    std::filesystem::path output_source_directory = "";
    std::vector<resource> resources = {};
  };
}
