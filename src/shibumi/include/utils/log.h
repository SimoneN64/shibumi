#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum message_type {
  INFO, WARNING, FATAL
};

extern char* last_message;
extern enum message_type last_message_type;

#define logfatal(fmt, ...) do { printf(fmt, ##__VA_ARGS__); exit(1); } while(0)
// wraps allocation to a format and copy to it
#define log_(type, fmt, ...) \
  do { \
    last_message_type = type; \
    size_t len = snprintf(NULL, 0, fmt, ##__VA_ARGS__) + 1; \
    if(last_message != NULL && strcmp(last_message, "")) { \
      last_message = (char*)realloc(last_message, len); \
    } else { \
      last_message = (char*)calloc(len, 1); \
    } \
    snprintf(last_message, len, fmt, ##__VA_ARGS__); \
  } while(0)
