#include "rev2.h"



void matrix_init_kb(void) {
  setPinOutput(B2);
  writePinHigh(B2);
  matrix_init_user();
}
