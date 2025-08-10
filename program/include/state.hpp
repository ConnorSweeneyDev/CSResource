#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace csr::base
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
    enum task
    {
      INVALID,
      COMPILE,
      LIST,
      SET
    };

    state(int argc, char *argv[]);

    task task = INVALID;

    std::filesystem::path output_shader_directory = "";
    std::filesystem::path output_include_directory = "";
    std::filesystem::path output_source_directory = "";
    std::vector<resource> resources = {};

    std::filesystem::path texture_directory = "";

    std::filesystem::path texture_path = "";
    unsigned int frame_width = 0;
    unsigned int frame_height = 0;
    unsigned int frame_count = 0;
  };
}
