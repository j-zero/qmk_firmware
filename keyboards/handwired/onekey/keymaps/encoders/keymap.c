#include QMK_KEYBOARD_H

//Tap Dance Declarations
enum {
  TD_BL = 0
};

void dance_cln_finished (qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 1:
      // single tap
      register_code (KC_MUTE);
      break;
    case 2:
      // single tap
      register_code (KC_MPLY);
      break;
    case 3:
      // single tap
      register_code (KC_MNXT);
      break;
    case 4:
      // single tap
      register_code (KC_MPRV);
      break;
    default:
      // more - nothing
      break;
  }
}

void dance_cln_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 1:
      // single tap
      unregister_code  (KC_MUTE);
      break;
    case 2:
      // single tap
      unregister_code  (KC_MPLY);
      break;
    case 3:
      // single tap
      unregister_code  (KC_MNXT);
      break;
    case 4:
      // single tap
      unregister_code (KC_MPRV);
      break;
    default:
      // more - nothing
      break;
  }
}

qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_BL]  = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_cln_finished, dance_cln_reset)
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  LAYOUT( TD(TD_BL) )
};

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLD);
        } else {
            tap_code(KC_VOLU);
        }
    }
}
