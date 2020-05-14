#include "numpad.h"

void matrix_init_kb(void) {
  setPinOutput(B4);
  setPinOutput(B5);
  writePinHigh(B4);
  writePinLow(B5);
  matrix_init_user();
}
