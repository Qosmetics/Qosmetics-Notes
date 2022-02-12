#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include <fmt/core.h>
#include <string_view>

namespace Qosmetics::Notes
{
    class Logging
    {
    public:
        static Logger& getLogger();
        static LoggerContextObject& getContextLogger(const char* fun, const char* file, int line);

        template <typename... TArgs>
        constexpr static void fmtLog(LoggerContextObject& logger, ::Logging::Level lvl, fmt::format_string<TArgs...> str, TArgs&&... args) noexcept
        {
            logger.log(lvl, fmt::format<TArgs...>(str, std::forward<TArgs>(args)...));
        }
    };
}
#define INFO(...) ::Qosmetics::Notes::Logging::fmtLog(::Qosmetics::Notes::Logging::getContextLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__), ::Logging::Level::INFO, __VA_ARGS__)
#define ERROR(...) ::Qosmetics::Notes::Logging::fmtLog(::Qosmetics::Notes::Logging::getContextLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__), ::Logging::Level::ERROR, __VA_ARGS__)
#define WARNING(...) ::Qosmetics::Notes::Logging::fmtLog(::Qosmetics::Notes::Logging::getContextLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__), ::Logging::Level::WARNING, __VA_ARGS__)
#define CRITICAL(...) ::Qosmetics::Notes::Logging::fmtLog(::Qosmetics::Notes::Logging::getContextLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__), ::Logging::Level::CRITICAL, __VA_ARGS__)
#define DEBUG(...) ::Qosmetics::Notes::Logging::fmtLog(::Qosmetics::Notes::Logging::getContextLogger(__PRETTY_FUNCTION__, __FILE__, __LINE__), ::Logging::Level::DEBUG, __VA_ARGS__)
//#define DEBUG(...)