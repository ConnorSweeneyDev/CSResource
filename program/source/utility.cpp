#include "utility.hpp"

#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

namespace csr::utility
{
  std::string unsigned_char_to_hex(unsigned char character)
  {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(character);
    return ss.str();
  }
}
