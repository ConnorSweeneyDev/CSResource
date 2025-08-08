#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace csr::utility
{
  std::string unsigned_char_to_hex(unsigned char character);
  uint32_t calculate_crc(const std::vector<unsigned char> &data);
}
