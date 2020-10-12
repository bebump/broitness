#pragma once

#include <juce_core/juce_core.h>

#include <memory>

namespace bbmp {
struct LogMessage {
  LogMessage();
  LogMessage(char const*);
  LogMessage(std::unique_ptr<juce::String>&&);

  char const* message_;
  std::unique_ptr<juce::String> heap_message_;
};

// All public functions are thread safe, non blocking, non allocating
struct NonBlockingLogger {
 private:
  NonBlockingLogger();

 public:
  static NonBlockingLogger& GetInstance() noexcept;

  bool TryLog(LogMessage&& message) noexcept;

  bool TryDequeue(LogMessage& message) noexcept;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace bbmp
