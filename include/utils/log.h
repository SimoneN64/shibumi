#pragma once
#include <stdio.h>
#include <stdlib.h>
#define logfatal(fmt, ...) do { printf(fmt, ##__VA_ARGS__); exit(1); } while(0)