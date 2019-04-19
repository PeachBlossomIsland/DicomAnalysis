#include "pch.h"
#include "global.h"
std::shared_ptr<spdlog::logger> g_log= spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "E:/async_log.txt");