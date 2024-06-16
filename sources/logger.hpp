#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>

namespace kape {
class Logger
{
 private:
  std::ofstream file_out_;
  bool is_available_;

 public:
  inline static const std::string DEFAULT_FILEPATH{"./log/log.txt"};
  Logger(std::string filepath = DEFAULT_FILEPATH);

  template<class OUTPUT>  
  friend Logger& operator<<(Logger& log, OUTPUT const& output)
  {
    if (log.is_available_) {
      log.file_out_ << output;
    }
    return log;
  }
};

inline Logger log{};

} // namespace kape

#endif