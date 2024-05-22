#include "logger.hpp"

namespace kape {
Logger::Logger(std::string filepath)
    : file_out{filepath, std::ios::out | std::ios::trunc}
{
  if (!file_out.is_open()) {
    throw std::runtime_error{"failed to open the file for the logger at: "
                             + filepath};
  }
}


} // namespace kape