#pragma once
#include <mi.h>
#include <pi.h>

typedef struct {
  mi_t mi;
  pi_t pi;
} mmio_t;

void init_mmio(mmio_t* mmio);