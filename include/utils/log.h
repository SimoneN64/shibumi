#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define logfatal(fmt, ...) do { printf(fmt, ##__VA_ARGS__); exit(1); } while(0)
#ifdef DEBUG
#define logdebug(fmt, ...) do { printf(fmt, ##__VA_ARGS__); } while(0)
#else
#define logdebug(fmt, ...)
#endif
// wraps allocation to a format and copy to it
#define alloc_strlencpy(dst, fmt, ...) \
  do { \
    size_t len = snprintf(NULL, 0, fmt, ##__VA_ARGS__) + 1; \
    if(dst != NULL && strcmp(dst, "")) { \
      dst = realloc(dst, len); \
    } else { \
      dst = calloc(len, 1); \
    } \
    snprintf(dst, len, fmt, ##__VA_ARGS__); \
  } while(0)

extern char* last_message;
enum message_type {
  INFO, WARNING, FATAL
};

extern enum message_type last_message_type;