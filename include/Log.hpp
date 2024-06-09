#pragma once

#define DEBUG 0
#define INFO 1
#define WARN 2
#define ERR 3
#define NONE 255

#ifndef LOG_LEVEL
#define LOG_LEVEL NONE
#endif

#define LOG_DEBUG(n) void()
#define LOG_INFO(n) void()
#define LOG_WARN(n) void()
#define LOG_ERROR(n) void()