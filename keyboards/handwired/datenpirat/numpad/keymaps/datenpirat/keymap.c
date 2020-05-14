#include QMK_KEYBOARD_H

#define IS_LAYER_ON(layer)  (layer_state & (1UL << (layer)))

//Tap Dance Declarations
enum {
  TD_DOT_COMMAS = 0,
  TD_NUM = 1
};

void dance_NUM_finished (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_NLCK);
  }
  else if (state->count == 2) {
    layer_invert(1);
  }
  else if (state->count == 3) {
    layer_invert(3);
  }
  else if (state->count == 4) {

  }
  else if (state->count == 5) {
      reset_keyboard();
  }
  else {
    register_code (KC_NLCK);
  }
}


void dance_NUM_reset (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_NLCK);
  }
  else if (state->count == 2) {

  }
  else if (state->count == 3) {

  }
  else if (state->count == 4) {

  }
  else if (state->count == 5) {
      //reset_keyboard();
  }
  else {
    unregister_code (KC_NLCK);
  }
}



//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //Tap once for Esc, twice for Caps Lock
  [TD_DOT_COMMAS]  = ACTION_TAP_DANCE_DOUBLE(KC_DOT, KC_COMMA),
  [TD_NUM] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_NUM_finished, dance_NUM_reset)
// Other declarations would go here, separated by commas, if you have them
};




bool led_update_user(led_t led_state) {

    writePinLow(B4);
    writePinLow(B5);

    writePin(B4, led_state.num_lock);
    writePin(B5, !led_state.num_lock);

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
    case 0:
        rgblight_setrgb (0x00,  0x00, 0xFF);
        break;
    case 1:
        rgblight_setrgb (0xFF,  0x00, 0x00);
        break;
    case 2:
        rgblight_setrgb (0x00,  0xFF, 0x00);
        break;
    case 3:
        rgblight_setrgb (0x00,  0xFF, 0xFF);
        break;
    case 4:
        rgblight_setrgb (0xFF,  0xFF, 0x00);
        break;
    default: //  for any other layers, or the default layer
        rgblight_setrgb (0xFF,  0xFF, 0xFF);
        break;
    }
  return state;
}

//In Layer declaration, add tap dance item in place of a key code

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_numpad_5x4(
    TD(TD_NUM),  KC_PSLS, KC_PAST, KC_PMNS,
    KC_P7,   KC_P8,   KC_P9,
    KC_P4,   KC_P5,   KC_P6,  KC_PPLS,
    KC_P1,   KC_P2,   KC_P3,
    LT(2, KC_P0), KC_COMMA,  KC_PENT
  ),

  [1] = LAYOUT_numpad_5x4(
    KC_TRNS,   KC_PSLS, KC_PAST, KC_PMNS,
    KC_P7,   KC_P8,   KC_P9,
    KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
    KC_P1,   KC_P2,   KC_P3,
    LT(2, KC_P0), TD(TD_DOT_COMMAS), KC_PENT
  ),

  [2] = LAYOUT_numpad_5x4(
    KC_TRNS, RESET, KC_MUTE, KC_VOLD,
    KC_HOME,   RGB_TOG,   KC_PGUP,
    KC_BSPC,   KC_ESC,   KC_MPLY, KC_VOLU,
    KC_END,   RGB_MOD,   KC_PGDN,
    KC_NO,    KC_PDOT,  KC_CALC
  ),

  [3] = LAYOUT_numpad_5x4(
    KC_TRNS, KC_NO, KC_MUTE, KC_NO,
    KC_HOME,   KC_MS_UP,   KC_MS_WH_UP,
    KC_MS_LEFT,   KC_MS_DOWN,   KC_MS_RIGHT, KC_VOLU,
    KC_END,   KC_MS_DOWN,   KC_MS_WH_DOWN,
    KC_MS_BTN1,    KC_MS_BTN3,  KC_MS_BTN2
  )
};
