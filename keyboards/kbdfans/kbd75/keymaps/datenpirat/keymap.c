#include QMK_KEYBOARD_H

bool autoshift_enabled = false;

typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7
};

enum {
  DEFAULT_LAYER,
  PLAIN_LAYER,
  SHIFT_LAYER,
  CTRL_LAYER,
  FUNKY_LAYER,
  DISABLED_LAYER,
  FN_LAYER_1,
  FN_LAYER_2
};

//Tap Dance Declarations
enum {
  TD_PSCR,
  TD_AKZENT,
  SUPER_TAB,
  SUPER_CAPS,
  SUPER_CTRL,
};
enum custom_keycodes {
  AUTOSHIFT_TOGGLE = SAFE_RANGE,
  MARKUP_CODE,
  REMOVE_LINE,
  KC_UNSHIFT_DEL,
  KC_UNSHIFT_BSPC,
  FUCK_SHIFT
};

int get_dance_state (qk_tap_dance_state_t *state);
//for the x tap dance. Put it here so it can be used in any keymap
void custom_autoshift_set(bool enabled);
void disable_caps(void);


void matrix_init_user(void){

}


bool is_capslock_on(void){
    led_t led_state = host_keyboard_led_state();
    return led_state.caps_lock;
}


const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 0, 0, 0, 0}
);
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, HSV_PINK}
);
const rgblight_segment_t PROGMEM my_layer3_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 10, 1, HSV_BLUE}
);
const rgblight_segment_t PROGMEM my_layer4_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 15, 1, HSV_BLUE}
);
const rgblight_segment_t PROGMEM my_caps_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 0, 1, HSV_RED},
    { 15, 1, HSV_RED}
);

// etc..

// Now define the array of layers. Later layers take precedence
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_layer1_layer,    // Overrides caps lock layer
    my_layer2_layer,     // Overrides other layers
    my_layer3_layer,     // Overrides other layers
    my_layer4_layer,     // Overrides other layers
    my_caps_layer
);

void custom_autoshift_set(bool enabled){
    autoshift_enabled = enabled;
    writePin(B2, !enabled);
    if (enabled)
        autoshift_enable();
    else
        autoshift_disable();
}

void custom_autoshift_toggle(void){
        custom_autoshift_set(!autoshift_enabled);
}

void disable_caps(){
        if(is_capslock_on()){   // Disable CAPS LOCK when it's on
            tap_code(KC_CAPS);
        }
}




bool led_update_user(led_t led_state) {

  rgblight_set_layer_state(4, led_state.caps_lock); // CAPS LAYER

  if (led_state.caps_lock || autoshift_enabled) {
    writePinLow(B2);

  } else {
    writePinHigh(B2);
  }
  return true;
}


int get_dance_state (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
      if ((state->interrupted || !state->pressed))  return SINGLE_TAP;
      //key has not been interrupted, but they key is still held. Means you want to send a 'HOLD'.
      else return SINGLE_HOLD;

  }
  else if (state->count == 2) {
    /*
     * DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
     * action when hitting 'pp'. Suggested use case for this return value is when you want to send two
     * keystrokes of the key, and not the 'double tap' action/macro.
    */
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  //Assumes no one is trying to type the same letter three times (at least not quickly).
  //If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
  //an exception here to return a 'TRIPLE_SINGLE_TAP', and define that enum just like 'DOUBLE_SINGLE_TAP'
  if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8; //magic number. At some point this method will expand to work for more presses
}


//instanalize an instance of 'tap' for the 'x' tap dance.
static tap tap_state = {
  .is_press_action = true,
  .state = 0
};


void dance_PSCR_finished (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_PSCR);
  }
  else if (state->count == 2) {
    register_code (KC_PAUS);
  }
  else if (state->count == 3) {
    register_code (KC_SLCK);
  }
  else if (state->count == 5) {
    reset_keyboard();               // RESET
  }
  else {
    register_code (KC_PSCR);
  }
}


void dance_PSCR_reset (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_PSCR);
  }
  else if (state->count == 2) {
    unregister_code (KC_PAUS);
  }
  else if (state->count == 3) {
    unregister_code (KC_SLCK);
  }
  else if (state->count == 4) {
    //unregister_code (KC_SLCK);
  }
  else if (state->count == 5) {

  }
  else {
    unregister_code (KC_PSCR);
  }
}


void super_AKZENT_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_EQL);
        break;
    case DOUBLE_HOLD:
        SEND_STRING("+ + + ");
        break;
    case DOUBLE_TAP:
    case DOUBLE_SINGLE_TAP:
        tap_code(KC_EQL);register_code(KC_EQL);
        break;
    case TRIPLE_TAP:
        tap_code(KC_EQL);tap_code(KC_EQL);register_code(KC_EQL);
        break;
    default:
        register_code(KC_EQL);
        break;
  }
}

void super_AKZENT_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_EQL);
        break;
    case DOUBLE_HOLD:
        //unregister_code(KC_LCTRL);
        break;
    case DOUBLE_TAP:
    case DOUBLE_SINGLE_TAP:
        unregister_code(KC_EQL);
        break;
    case TRIPLE_TAP:
        unregister_code(KC_EQL);
        break;
     default:
        unregister_code(KC_EQL);
        break;
  }
  tap_state.state = 0;
}



void super_CAPS_start (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1){
      layer_on(FN_LAYER_2);
  }
}

void super_CAPS_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);

  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_HOME);
        break;
    case SINGLE_HOLD:
        layer_on(FN_LAYER_2);
        break;
    case DOUBLE_TAP:
        disable_caps();
        custom_autoshift_toggle();
        break;
    case DOUBLE_HOLD:
        register_code(KC_LCTL);
        break;
    case DOUBLE_SINGLE_TAP:
        //register_code(KC_X); unregister_code(KC_X); register_code(KC_X);
        break;
    case TRIPLE_TAP:
        custom_autoshift_set(false); // disable autoshift
        register_code(KC_CAPS);
        break;
    //Last case is for fast typing. Assuming your key is `f`:
    //For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
    //In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
  }
}

void super_CAPS_reset (qk_tap_dance_state_t *state, void *user_data) {

  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_HOME);
        break;
    case SINGLE_HOLD:
        break;
    case DOUBLE_TAP:
        break;
    case DOUBLE_HOLD:
        unregister_code(KC_LCTL);
        break;
    case DOUBLE_SINGLE_TAP:
        //unregister_code(KC_X);
        break;
    case TRIPLE_TAP:
        unregister_code(KC_CAPS);
        break;
    default:
        break;
  }

  layer_off(FN_LAYER_2);

  tap_state.state = 0;
}

void super_CTRL_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
    case SINGLE_HOLD:
        register_code(KC_RCTL);
        break;
    case DOUBLE_TAP:
        register_code(KC_MPLY);
        break;
    case DOUBLE_HOLD:
        register_code(KC_LCTL); register_code(KC_LALT);
        break;
    default:
        register_code(KC_RCTL);
        break;
  }
}

void super_CTRL_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
    case SINGLE_HOLD:
        unregister_code(KC_RCTL);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_MPLY);
        break;
    case DOUBLE_HOLD:
        unregister_code(KC_LCTL); unregister_code(KC_LALT);
        break;
    default:
        unregister_code(KC_RCTL);
        break;
  }
  tap_state.state = 0;
}


//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  [TD_PSCR]         = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_PSCR_finished, dance_PSCR_reset),
  [SUPER_CAPS]       = ACTION_TAP_DANCE_FN_ADVANCED(super_CAPS_start ,super_CAPS_finished, super_CAPS_reset),
  [SUPER_CTRL]       = ACTION_TAP_DANCE_FN_ADVANCED(NULL,super_CTRL_finished, super_CTRL_reset),
  [TD_AKZENT]       = ACTION_TAP_DANCE_FN_ADVANCED(NULL,super_AKZENT_finished, super_AKZENT_reset)
};



void keyboard_post_init_user(void) {
  custom_autoshift_set(false);
  rgblight_layers = my_rgb_layers;
  rgblight_set_layer_state(0, true);
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Both layers will light up if both kb layers are active
    rgblight_set_layer_state(0, layer_state_cmp(state, DEFAULT_LAYER)); // Normal
    rgblight_set_layer_state(1, layer_state_cmp(state, PLAIN_LAYER)); // Plain
    rgblight_set_layer_state(2, layer_state_cmp(state, FN_LAYER_1)); // Fn1
    rgblight_set_layer_state(3, layer_state_cmp(state, FN_LAYER_2)); // Fn2

    return state;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // DEFAULT
	[DEFAULT_LAYER] = LAYOUT(
    KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   TD(TD_PSCR),  KC_HOME,   KC_INS,
    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  TD(TD_AKZENT),   _______,  KC_BSPC,    KC_PGUP,
    KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,              KC_DEL ,
    TD(SUPER_CAPS),  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,      KC_PGDN,
    LM(SHIFT_LAYER , MOD_LSFT),  KC_NUBS,  KC_Z,   KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  RSFT_T(KC_HOME),       KC_UP,   KC_END ,
    KC_LCTL,  KC_LGUI,  KC_LALT,                      KC_SPC,   KC_SPC,   KC_SPC,                       KC_RALT,  LT(FN_LAYER_1 ,KC_APP),  TD(SUPER_CTRL),  KC_LEFT,  KC_DOWN,  KC_RGHT
  ),

  // GAMING / PLAIN
	[PLAIN_LAYER] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  TD_PSCR,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_BSPC, _______,
    _______ ,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            KC_DEL,
    KC_CAPS,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    KC_LSFT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_RSFT,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,  _______
  ),


  // Shift Layer
	[SHIFT_LAYER] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_UNSHIFT_DEL, _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,   _______
   ),

    // Funky Layer
	[FUNKY_LAYER] = LAYOUT(
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,      _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
    _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,   _______
  ),

    // Empty/Testing
	[DISABLED_LAYER] = LAYOUT(
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,  XXXXXXX,
    XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  MO(FN_LAYER_1),  XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX
  ),
  // Functions I, activated by FN1
	[FN_LAYER_1] = LAYOUT(
    DF(DEFAULT_LAYER), TG(PLAIN_LAYER), TG(FUNKY_LAYER), _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_SLCK, KC_PAUS , KC_DEL,
    KC_WAKE,  RGB_M_P, RGB_M_B,  RGB_M_R,  RGB_M_SW, RGB_M_SN,  RGB_M_K,  RGB_M_T,  RGB_M_G,  _______,  KC_NLCK,  _______,  MARKUP_CODE,  _______, REMOVE_LINE, KC_ASUP,
    _______,  RGB_TOG,  RGB_MOD,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD,  RGB_VAI,  RGB_VAD,  _______,  _______,  _______,  KC_VOLU,  KC_MUTE,       _______,
    KC_CAPS,  BL_DEC,  BL_INC,  _______,  _______,  _______,  _______,  _______,  _______,  TG(DISABLED_LAYER),  _______,  KC_F20,            KC_CALC,             KC_ASDN,
    _______,  _______,  KC_BRIU,  KC_BRID,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_VOLD,  KC_ASRP,         KC_PGUP,  _______,
    RESET  ,  _______,  _______,                  BL_STEP,  BL_STEP,  BL_STEP,                         _______,  _______,  KC_RGUI,           KC_HOME,   KC_PGDN,  KC_END
  ),

    // Functions II, activated by CAPS LOCK
	[FN_LAYER_2] = LAYOUT(
    DF(DEFAULT_LAYER),  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,       RESET, _______, KC_DEL,
    KC_SLEP ,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______, _______,  MARKUP_CODE,  _______, REMOVE_LINE,  _______,
    _______,  _______,  KC_WH_U,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, KC_VOLU,  _______,               _______,
    _______,  KC_WH_L,  KC_WH_D,  KC_WH_R,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                  _______,   _______ ,
    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  KC_VOLD,  _______,                    KC_PGDN, _______,
    _______,  _______,  _______,                  KC_MPLY,  KC_MPLY,  KC_MPLY,                      _______,  _______,   _______,             KC_MPRV, KC_PGDN,KC_MNXT
  ),

};



bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  switch (keycode) {

    case MARKUP_CODE:
      if (record->event.pressed) {
        SEND_STRING("+ + + ");
      } else {

      }
      break;

    case REMOVE_LINE:
      if (record->event.pressed) {
          tap_code(KC_END); register_code(KC_LSFT); tap_code(KC_HOME); unregister_code(KC_LSFT); tap_code(KC_DEL);
      } else {

      }
      break;

    case KC_UNSHIFT_DEL:
      if (record->event.pressed) {
          unregister_code(KC_LSFT); tap_code(KC_DEL); register_code(KC_LSFT);
      } else {

      }
      break;

    case KC_UNSHIFT_BSPC:
      if (record->event.pressed) {
          unregister_code(KC_LSFT); tap_code(KC_BSPC); register_code(KC_LSFT);
      } else {

      }
      break;
  }

  return true;
};

void post_process_record_user(uint16_t keycode, keyrecord_t *record){

}
