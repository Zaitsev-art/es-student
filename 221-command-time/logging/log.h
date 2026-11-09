#pragma once

#include <stdio.h>

#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_INF 2
#define LOG_LEVEL_DBG 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

void log_version(void);
void log_prefix(const char *level, const char *function, int line);

#define LOG_AT_LEVEL(level, label, ...)                  \
    do                                                   \
    {                                                    \
        if (LOG_LEVEL >= (level))                         \
        {                                                \
            log_prefix((label), __func__, __LINE__);       \
            printf(__VA_ARGS__);                          \
        }                                                \
    } while (0)

#define LOG_ERR(...) LOG_AT_LEVEL(LOG_LEVEL_ERR, "err", __VA_ARGS__)
#define LOG_INF(...) LOG_AT_LEVEL(LOG_LEVEL_INF, "inf", __VA_ARGS__)
#define LOG_DBG(...) LOG_AT_LEVEL(LOG_LEVEL_DBG, "dbg", __VA_ARGS__)
