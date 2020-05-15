#include QMK_KEYBOARD_H

bool autoshift_enabled = false;
bool swap_backspace_del = false;
uint16_t del_bspc_lastcode = KC_NO;


void matrix_init_user(void){
  autoshift_disable();
}

enum custom_keycodes {
  EXPERIMENT = SAFE_RANGE,
  AUTOSHIFT_TOGGLE,
  MARKUP_CODE,
  BSPCDEL,
  DELBSPC,
  SWAP_BSPCDEL,
  SHIFT_BSPCDEL
};

void custom_autoshift_toggle(void){
      if (autoshift_enabled) {
        autoshift_enabled = false;
        autoshift_disable();
        writePinHigh(B2);   // Disable CAPS LED
        //breathing_disable();
      }
      else {
        autoshift_enabled = true;
        autoshift_enable();
        writePinLow(B2); // Enable CAPS LED
        //breathing_enable();
      }
}

void dance_L_SHIFT_finished (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 3) {
    register_code (KC_CAPS);
  }
  else if (state->count == 2) {
    custom_autoshift_toggle();
  }
  else {
    register_code (KC_LSFT);
  }
}


void dance_L_SHIFT_reset (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 3) {
    unregister_code (KC_CAPS);
  }
  else if (state->count == 2) {
    //unregister_code (KC_LSFT);
  }
  else {
    unregister_code (KC_LSFT);
  }
}



void led_set_kb(uint8_t usb_led) {
  // put your keyboard LED indicator (ex: Caps Lock LED) toggling code here
  if (IS_LED_ON(usb_led, USB_LED_CAPS_LOCK) || autoshift_enabled) {
    writePinLow(B2);
  } else {
    writePinHigh(B2);
  }

  led_set_user(usb_led);
}

//Tap Dance Declarations
enum {
  TD_L_SHIFT = 0,
  //TD_ESC_RESET,
  //TD_SHIFT_CAPS
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //[TD_SHIFT_CAPS]  = ACTION_TAP_DANCE_DOUBLE(KC_RSFT, KC_CAPS),
  //[TD_ESC_RESET]  = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, RESET),
  [TD_L_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_L_SHIFT_finished, dance_L_SHIFT_reset)

};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // DEFAULT
	[0] = LAYOUT(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_HOME,   KC_INS,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   _______,  BSPCDEL,    KC_PGUP,
    KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,              DELBSPC ,
     MO(5) ,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,      KC_PGDN,
    TD(TD_L_SHIFT),  KC_NUBS,  KC_Z,   KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  RSFT_T(KC_HOME),       KC_UP,   KC_END ,
    KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,   KC_SPC,   KC_SPC,                       KC_RALT,  LT(4,KC_APP),  KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT
  ),

  // Functions I, activated by FN1
	[4] = LAYOUT(
      DF(0),    DF(1),    DF(2),  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_SLCK, KC_PAUS , KC_DEL,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_NLCK,  _______,  MARKUP_CODE,  _______, KC_BSPC, KC_ASUP,
    _______,  _______,  KC_WH_U,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_VOLU,  KC_MUTE,       SWAP_BSPCDEL,
    KC_CAPS,  KC_WH_L,  KC_WH_D,  KC_WH_R,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_F20,            KC_CALC,             KC_ASDN,
    AUTOSHIFT_TOGGLE,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_VOLD,  KC_ASRP,         KC_PGUP,  _______,
    _______,  _______,  _______,                  BL_STEP,  BL_STEP,  BL_STEP,                         _______,  _______,  KC_RGUI,           KC_HOME,   KC_PGDN,  KC_END
  ),

    // Functions II, activated by CAPS LOCK
	[5] = LAYOUT(
      DF(0),  RGB_M_P,  RGB_M_B,  RGB_M_R,  RGB_M_SW, RGB_M_SN,  RGB_M_K,  RGB_M_X,  RGB_M_G,  RGB_M_T,  _______,  _______,   _______,       RESET, _______, KC_DEL,
    _______,  RGB_TOG,  RGB_MOD,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD,  RGB_VAI,  RGB_VAD,   BL_DEC,  BL_INC, _______,  MARKUP_CODE,  _______, DELBSPC,  _______,
    _______,  _______,  KC_WH_U,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,               BSPCDEL,
    _______,  KC_WH_L,  KC_WH_D,  KC_WH_R,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                  _______,   _______ ,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  _______,  KC_RSFT,                    KC_PGDN, _______,
    _______,  _______,  _______,                  KC_MPLY,  KC_MPLY,  KC_MPLY,                      KC_MSTP,  KC_MPRV,   KC_MNXT,             KC_HOME, KC_PGDN,KC_END
  ), // BL_TOGG,  BL_STEP,

  // GAMING / PLAIN
	[1] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_BSPC, _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_DEL,
    KC_CAPS,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    KC_LSFT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_RSFT,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  MO(4),  _______,  _______,  _______,  _______
  ),


  // Empty/Testing
	[2] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,    MO(4),  _______,  _______,  _______,   _______
  ),

    // Empty/Testing
	[3] = LAYOUT(
    _______,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,    MO(4),  XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX
  ),

};



bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  static uint8_t saved_mods = 0; // Place this outside of the switch, but inside process_record_user()

  switch (keycode) {
    case EXPERIMENT:
      if (record->event.pressed) {
        // when keycode QMKBEST is pressed
        SEND_STRING("d@tenpir.at");
      } else {
        // when keycode QMKBEST is released
      }
      break;

    case SHIFT_BSPCDEL:
        if (record->event.pressed) {
            if (get_mods() & MOD_MASK_SHIFT) {
                saved_mods = get_mods() & MOD_MASK_SHIFT; // Mask off anything that isn't Shift
                del_mods(saved_mods); // Remove any Shifts present
                if (!swap_backspace_del){
                register_code(KC_DEL);
                } else {
                register_code(KC_BSPC);
                }
            } else {
                saved_mods = 0; // Clear saved mods so the add_mods() below doesn't add Shifts back when it shouldn't
                if (!swap_backspace_del){
                register_code(KC_BSPC);
                } else {
                register_code(KC_DEL);
                }
            }
        } else {
            add_mods(saved_mods);
            unregister_code(KC_DEL);
            unregister_code(KC_BSPC);
        }

    return false;


    case BSPCDEL: // backspace becomes delete
      if (record->event.pressed){
        if (swap_backspace_del){
          register_code(KC_DEL);
        } else {
          register_code(KC_BSPC);
        }
      } else {
        unregister_code(KC_DEL);
        unregister_code(KC_BSPC);
      }
      return false;

    case DELBSPC: // delete becomes backspace
      if (record->event.pressed){
        if (swap_backspace_del){
          register_code(KC_BSPC);
        } else {

          register_code(KC_DEL);
        }
      } else {
        unregister_code(KC_DEL);
        unregister_code(KC_BSPC);
      }
    return false;


    case SWAP_BSPCDEL:
      if (record->event.pressed) {
          swap_backspace_del = !swap_backspace_del;
      } else {
        // Do something else when release
      }
      return false; // Skip all further processing of this key

    case MARKUP_CODE:
      if (record->event.pressed) {
        // when keycode QMKBEST is pressed
        SEND_STRING("+ + + ");
      } else {
        // when keycode QMKBEST is released
      }
      break;

    case AUTOSHIFT_TOGGLE:
      if (record->event.pressed) {
        custom_autoshift_toggle();
      } else {
        // Do something else when release
      }
      return false; // Skip all further processing of this key

  }
  return true;
};
