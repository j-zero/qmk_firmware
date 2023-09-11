#include "rev2.h"

void matrix_init_kb(void) {
  /* PS/2 Timer/INT Change
  PCICR |= (1<<PCIE0);
  PCMSK0 |= (1<<PCINT6);
  */
  setPinOutput(CAPS_LED_PIN);
  writePinHigh(CAPS_LED_PIN);
  matrix_init_user();
  
}

