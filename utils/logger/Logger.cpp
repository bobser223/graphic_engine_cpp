#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstring>
#include <execinfo.h>
#include <fcntl.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <optional>
#include <unistd.h>

#include "../defines.h"

namespace logging {

namespace {

std::filesystem::path crash_log_path;

constexpr int log_number_width = 5;

void writeCrashLine(int fd, const char* text) {
  if (fd >= 0 && text != nullptr) {
    const auto length = std::strlen(text);
    while (write(fd, text, length) == -1 && errno == EINTR) {
    }
  }
}

void handleCrashSignal(int signal) {
  void* frames[64];
  const int frame_count = backtrace(frames, 64);

  const char* header = "\n[LOGGER] Process crashed. Backtrace:\n";
  writeCrashLine(STDERR_FILENO, header);
  backtrace_symbols_fd(frames, frame_count, STDERR_FILENO);

  int fd = -1;
  if (!crash_log_path.empty()) {
    fd = open(crash_log_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
  }

  if (fd >= 0) {
    writeCrashLine(fd, header);
    backtrace_symbols_fd(frames, frame_count, fd);
    close(fd);
  }

  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

void installCrashHandlers() {
  static bool installed = false;
  if (installed) {
    return;
  }

  std::signal(SIGSEGV, handleCrashSignal);
  std::signal(SIGABRT, handleCrashSignal);
  installed = true;
}

std::string timestamp() {
  const auto now = std::chrono::system_clock::now();
  const auto time = std::chrono::system_clock::to_time_t(now);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    now.time_since_epoch()
  ) % 1000;

  std::tm local_time{};
#if defined(_WIN32)
  localtime_s(&local_time, &time);
#else
  localtime_r(&time, &local_time);
#endif

  std::ostringstream stream;
  stream << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S")
         << '.' << std::setfill('0') << std::setw(3) << ms.count();
  return stream.str();
}

std::string shortFileName(const char* file) {
  return std::filesystem::path(file).filename().string();
}

std::optional<int> extractLogNumber(const std::filesystem::path& path) {
  const std::string filename = path.filename().string();
  constexpr std::string_view prefix = "engine";
  constexpr std::string_view extension = ".log";

  if (!filename.starts_with(prefix) || !filename.ends_with(extension)) {
    return std::nullopt;
  }

  const size_t number_start = prefix.size();
  const size_t number_length = filename.size() - prefix.size() - extension.size();
  if (number_length == 0) {
    return std::nullopt;
  }

  const std::string number = filename.substr(number_start, number_length);
  if (!std::all_of(number.begin(), number.end(), [](const char symbol) {
        return std::isdigit(static_cast<unsigned char>(symbol));
      })) {
    return std::nullopt;
  }

  return std::stoi(number);
}

std::filesystem::path makeLogFilePath(const std::filesystem::path& directory, int number) {
  std::ostringstream filename;
  filename << "engine"
           << std::setfill('0') << std::setw(log_number_width) << number
           << ".log";

  return directory / filename.str();
}

std::filesystem::path nextLogFilePath(const std::filesystem::path& directory) {
  std::filesystem::create_directories(directory);

  int max_number = 0;
  for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    const std::optional<int> number = extractLogNumber(entry.path());
    if (number.has_value()) {
      max_number = std::max(max_number, *number);
    }
  }

  return makeLogFilePath(directory, max_number + 1);
}

} // namespace

Logger& Logger::instance() {
  static Logger logger;
  return logger;
}

Logger::Logger() {
  setLogFile(nextLogFilePath(PROJECT_PATH / "logs"), false);
  installCrashHandlers();
}

void Logger::setLogFile(const std::filesystem::path& path, bool append) {
  std::lock_guard lock(mutex_);

  file_path_ = path;
  crash_log_path = file_path_;
  std::filesystem::create_directories(file_path_.parent_path());

  if (file_.is_open()) {
    file_.close();
  }

  const auto mode = append ? std::ios::app : std::ios::trunc;
  file_.open(file_path_, mode);

  if (!file_.is_open()) {
    std::cerr << "[LOGGER] Failed to open log file: " << file_path_ << std::endl;
  }
}

void Logger::setMinLevel(LogLevel level) {
  std::lock_guard lock(mutex_);
  min_level_ = level;
}

void Logger::enableConsole(bool enabled) {
  std::lock_guard lock(mutex_);
  console_enabled_ = enabled;
}

void Logger::enableFile(bool enabled) {
  std::lock_guard lock(mutex_);
  file_enabled_ = enabled;
}

void Logger::write(LogLevel level, const char* file, int line, const std::string& message) {
  std::lock_guard lock(mutex_);

  std::ostringstream record;
  record << '[' << timestamp() << "] "
         << '[' << toString(level) << "] "
         << '[' << shortFileName(file) << ':' << line << "] "
         << message;

  if (console_enabled_) {
    std::ostream& output = level >= LogLevel::Warn ? std::cerr : std::cout;
    output << record.str() << std::endl;
  }

  if (file_enabled_ && file_.is_open()) {
    file_ << record.str() << std::endl;
  }
}

const char* toString(LogLevel level) {
  switch (level) {
    case LogLevel::Trace:
      return "TRACE";
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
  }

  return "UNKNOWN";
}

} // namespace logging
