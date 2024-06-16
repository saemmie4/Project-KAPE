#include "logger.hpp"
#include <iostream>

namespace kape {
Logger::Logger(std::string filepath)
    : file_out_{filepath, std::ios::out | std::ios::trunc}
    , is_available_{true}
{
  if (!file_out_.is_open()) {
    std::cout << "[ERROR]: Failed to open the file for the logger at: \""
                     + filepath + "\"";
    is_available_ = false;
  }
}

} // namespace kape