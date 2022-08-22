#include "rev2.h"

void matrix_init_kb(void) {
  PCICR |= (1<<PCIE1);
  PCMSK0 |= (1<<PCINT6);
  setPinOutput(CAPS_LED_PIN);
  writePinHigh(CAPS_LED_PIN);
  matrix_init_user();
  
}

