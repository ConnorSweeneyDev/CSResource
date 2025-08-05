#pragma once

#include "exception.hpp"

#include <format>
#include <string>

#include "SDL3/SDL_error.h"
const char *stbi_failure_reason(void);

namespace csr::utility
{
  template <typename... message_arguments>
  exception::exception(const std::string &message_, message_arguments &&...arguments_)
    : message(std::format(
        "{}.", std::vformat(message_, std::make_format_args(std::forward<const message_arguments>(arguments_)...))))
  {
  }

  template <typename... message_arguments>
  sdl_exception::sdl_exception(const std::string &message_, message_arguments &&...arguments_)
    : exception(message_, std::forward<const message_arguments>(arguments_)...)
  {
    message.pop_back();
    std::string sdl_error(SDL_GetError());
    if (sdl_error.empty())
      message = std::format("{}: Unknown SDL error.", message);
    else
      message = std::format("{}: {}.", message, sdl_error);
  }

  template <typename... message_arguments>
  stbi_exception::stbi_exception(const std::string &message_, message_arguments &&...arguments_)
    : exception(message_, std::forward<const message_arguments>(arguments_)...)
  {
    message.pop_back();
    std::string stbi_error(stbi_failure_reason());
    if (stbi_error.empty())
      message = std::format("{}: Unknown STBI error.", message);
    else
      message = std::format("{}: {}.", message, stbi_error);
  }
}
