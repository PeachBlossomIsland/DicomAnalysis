#pragma once
#ifndef Global_H
#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
extern std::shared_ptr<spdlog::logger> g_log;
#endif // !Global_H
