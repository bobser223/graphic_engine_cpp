#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>

namespace logging {

enum class LogLevel {
  Trace = 0,
  Debug,
  Info,
  Warn,
  Error
};

class Logger {
public:
  static Logger& instance();

  void setLogFile(const std::filesystem::path& path, bool append = true);
  void setMinLevel(LogLevel level);
  void enableConsole(bool enabled);
  void enableFile(bool enabled);

  template<typename... Args>
  void trace(const char* file, int line, Args&&... args) {
    log(LogLevel::Trace, file, line, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void debug(const char* file, int line, Args&&... args) {
    log(LogLevel::Debug, file, line, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void info(const char* file, int line, Args&&... args) {
    log(LogLevel::Info, file, line, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void warn(const char* file, int line, Args&&... args) {
    log(LogLevel::Warn, file, line, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void error(const char* file, int line, Args&&... args) {
    log(LogLevel::Error, file, line, std::forward<Args>(args)...);
  }

private:
  Logger();

  template<typename... Args>
  void log(LogLevel level, const char* file, int line, Args&&... args) {
    if (level < min_level_) {
      return;
    }

    std::ostringstream message;
    (message << ... << std::forward<Args>(args));
    write(level, file, line, message.str());
  }

  void write(LogLevel level, const char* file, int line, const std::string& message);

  std::mutex mutex_;
  std::ofstream file_;
  std::filesystem::path file_path_;
  LogLevel min_level_ = LogLevel::Trace;
  bool console_enabled_ = true;
  bool file_enabled_ = true;
};

const char* toString(LogLevel level);

} // namespace logging

#define LOG_TRACE(...) ::logging::Logger::instance().trace(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) ::logging::Logger::instance().debug(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) ::logging::Logger::instance().info(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) ::logging::Logger::instance().warn(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) ::logging::Logger::instance().error(__FILE__, __LINE__, __VA_ARGS__)

#endif // ENGINE_LOGGER_H
