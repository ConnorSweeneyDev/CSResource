#pragma once

#include <exception>
#include <string>

namespace csr::utility
{
  class exception : public std::exception
  {
  public:
    template <typename... message_arguments> exception(const std::string &message_, message_arguments &&...arguments_);

    const char *what() const noexcept override;

  protected:
    std::string message = "";
  };

  class sdl_exception : public exception
  {
  public:
    template <typename... message_arguments>
    sdl_exception(const std::string &message_, message_arguments &&...arguments_);
  };

  class stbi_exception : public exception
  {
  public:
    template <typename... message_arguments>
    stbi_exception(const std::string &message_, message_arguments &&...arguments_);
  };
}

#include "exception.inl"
