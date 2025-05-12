#pragma once

#include <cstdlib>
#include <format>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>

enum class LogLevel {
    Info,
    Warn,
    Error
};

template <typename... Args>
inline auto log(LogLevel level, std::string_view fmt, Args &&...args) -> void {
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    switch (level) {
    case LogLevel::Info:
        std::cout << "[INFO]  " << msg << "\n";
        break;
    case LogLevel::Warn:
        std::cout << "[WARN]  " << msg << "\n";
        break;
    case LogLevel::Error:
        std::cerr << "[ERROR] " << msg << "\n";
        break;
    }
}

template <
    typename T,
    typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
inline auto log(LogLevel level, T &&value) -> void {
    log(level, "{}", std::forward<T>(value));
}

#define LOG_INFO(fmt, ...) log(LogLevel::Info, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) log(LogLevel::Warn, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) log(LogLevel::Error, fmt, ##__VA_ARGS__)

/**
 * panic_impl: prints an error and aborts.
 * - msg defaults to empty if not provided.
 * - loc defaults to the *call-site* source_location.
 */
[[noreturn]] inline auto
panic_impl(
    const std::string &msg = {},
    const std::source_location loc = std::source_location::current()) -> void {
    // Build the panic message: either with or without a user-provided msg
    std::string full;
    if (msg.empty()) {
        full = std::format("PANIC at {}:{}", loc.file_name(), loc.line());
    } else {
        full = std::format("PANIC: '{}' at {}:{}", msg,
            loc.file_name(), loc.line());
    }

    // Log it and abort
    LOG_ERR("{}", full);
    std::exit(EXIT_FAILURE);
}

// Now macros *at the use site* to forward into panic_impl():
#undef PANIC
#define PANIC(...) panic_impl(__VA_ARGS__)