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
    std::filesystem::path path = {};
    std::string include_text = {};
    std::string source_text = {};
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
    struct frame_data
    {
      struct frame_group
      {
        std::string name = {};
        unsigned int start_frame = {};
        unsigned int end_frame = {};
      };
      unsigned int width = {};
      unsigned int height = {};
      std::vector<frame_group> groups = {};
    };

    state(int argc, char *argv[]);

    task task = {};

    std::filesystem::path output_shader_directory = {};
    std::filesystem::path output_include_directory = {};
    std::filesystem::path output_source_directory = {};
    std::vector<resource> resources = {};

    std::filesystem::path texture_directory = {};

    std::filesystem::path texture_path = {};
    frame_data frame_data = {};
  };
}
