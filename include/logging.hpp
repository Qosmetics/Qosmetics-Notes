#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <string_view>

#include "paper/shared/logger.hpp"

#define INFO(str, ...) Paper::Logger::fmtLog<Paper::LogLevel::INF>(str, "Qosmetics Notes" __VA_OPT__(, __VA_ARGS__))
#define ERROR(str, ...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(str, "Qosmetics Notes" __VA_OPT__(, __VA_ARGS__))
#define WARNING(str, ...) Paper::Logger::fmtLog<Paper::LogLevel::WRN>(str, "Qosmetics Notes" __VA_OPT__(, __VA_ARGS__))
#define CRITICAL(str, ...) Paper::Logger::fmtLog<Paper::LogLevel::CRIT>(str, "Qosmetics Notes" __VA_OPT__(, __VA_ARGS__))
#define DEBUG(str, ...) Paper::Logger::fmtLog<Paper::LogLevel::DBG>(str, "Qosmetics Notes" __VA_OPT__(, __VA_ARGS__))
