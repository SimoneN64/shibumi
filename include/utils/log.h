#pragma once
#define logfatal(fmt, ...) do { printf(fmt, __VA_ARGS__); exit(1); } while(0)