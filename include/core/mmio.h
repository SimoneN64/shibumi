#pragma once
#include <mi.h>
#include <pi.h>
#include <ri.h>
#include <vi.h>
#include <si.h>

typedef struct {
  mi_t mi;
  pi_t pi;
  ri_t ri;
  vi_t vi;
  si_t si;
} mmio_t;

void init_mmio(mmio_t* mmio);