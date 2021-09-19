#include <mmio.h>

void init_mmio(mmio_t *mmio) {
  init_mi(&mmio->mi);
  init_pi(&mmio->pi);
}