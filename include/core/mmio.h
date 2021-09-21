#pragma once
#include <mi.h>
#include <pi.h>
#include <ri.h>

typedef struct {
  mi_t mi;
  pi_t pi;
  ri_t ri;
} mmio_t;

void init_mmio(mmio_t* mmio);