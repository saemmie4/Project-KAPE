#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

namespace kape {
class Logger
{
 private:
  std::ofstream file_out;

 public:
  inline static const std::string DEFAULT_FILEPATH{"./log/log.txt"};
  Logger(std::string filepath = DEFAULT_FILEPATH);

  template<class OUTPUT>
  friend Logger& operator<<(Logger& log, OUTPUT const& output)
  {
    log.file_out << output;
    return log;
  }
};

inline Logger log{};

} // namespace kape

#endif