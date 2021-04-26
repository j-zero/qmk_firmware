#include QMK_KEYBOARD_H



typedef union {
  uint32_t raw;
  struct {
    bool     sexy_shift_enabled     :1;
    bool     sweet_caps_enabled     :1;
    bool     rshift_home_enabled    :1;
  };
} user_config_t;

user_config_t user_config;

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
  TRIPLE_HOLD = 7,
  SINGLE_HOLD_INTERRUPTED = 8,
  TAP_END = 9
};

enum {
    DEFAULT_LAYER,
    PLAIN_LAYER,
    SHIFT_LAYER,
    CTRL_LAYER,
    FUNKY_LAYER,
    DISABLED_LAYER,
    FN_LAYER_1,
    FN_LAYER_2,
    RSHIFT_LAYER
};

//Tap Dance Declarations
enum {
    TD_AKZENT,
    TD_CTRL,
    TD_CAPS,
    TD_PSCR,
};

enum custom_keycodes {
    AUTOSHIFT_TOGGLE = SAFE_RANGE,
    MARKUP_CODE,
    REMOVE_LINE,
    KC_UNSHIFT_DEL,
    KC_UNSHIFT_HOME,
    DP_LSFT,
    DP_RSFT,
    TG_SESFT,
    TG_SWCPS,
    TG_RSFTHM
};

static bool sweet_caps_enabled = true;
static bool sweet_caps_was_enabled = false;
static bool rshift_home_enabled = true;

static bool sexy_shift_enabled = true;
static bool sexy_shift_on = false;
static bool sexy_shift_tapped = false;
static bool sexy_shift_oneshot = false;
static uint16_t sexy_shift_code = KC_NO;
static uint16_t sexy_shift_layer = RSHIFT_LAYER;
static uint16_t sexy_shift_command_keycode = KC_NO;
static uint16_t sexy_shift_last_keycode = 0;
static uint16_t sexy_shift_tap_timer = 0;

void print_keycode(uint16_t keycode);
uint16_t get_td_keycode(uint16_t n);

void sexy_shift_start(uint16_t command_keycode, uint16_t code, uint16_t layer);
void sexy_shift_reset(void);
void sexy_shift_stop(void);
void sexy_shift_restart(void);
bool sexy_shift_is_tapped(void);
bool sexy_shift_is_tapped_time(uint16_t term);
void sexy_shift_process(uint16_t keycode);
void sexy_shift_enable(bool enable);
void sexy_shift_toggle(void);

void sweet_caps_toggle(void);
void rshift_home_toggle(void);
void ack_signal(bool);

int get_dance_state (qk_tap_dance_state_t *state);

//for the x tap dance. Put it here so it can be used in any keymap
void custom_autoshift_set(bool enabled);
void set_caps(bool enabled);

void update_eeprom(void);

void set_caps_led(bool enabled){
  writePin(B2, !enabled);
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
const rgblight_segment_t PROGMEM my_fn1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 10, 1, HSV_WHITE}
);
const rgblight_segment_t PROGMEM my_fn2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 15, 1, HSV_WHITE}
);

// CAPS
const rgblight_segment_t PROGMEM my_caps_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 0, 1, HSV_RED},
    { 15, 1, HSV_RED}
);

const rgblight_segment_t PROGMEM my_acknowledge_layer_on[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, HSV_GREEN}
);

const rgblight_segment_t PROGMEM my_acknowledge_layer_off[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, HSV_RED}
);

const rgblight_segment_t PROGMEM my_funky_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 16, 0, 0, 0},
    { 6, 2, HSV_WHITE}
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_layer1_layer,    // Overrides caps lock layer
    my_layer2_layer,     // Overrides other layers
    my_fn1_layer,     // Overrides other layers
    my_fn2_layer,     // Overrides other layers
    my_caps_layer,
    my_acknowledge_layer_on,
    my_acknowledge_layer_off,
    my_funky_layer
);

layer_state_t layer_state_set_user(layer_state_t state) {
    // Both layers will light up if both kb layers are active
    rgblight_set_layer_state(0, layer_state_cmp(state, DEFAULT_LAYER)); // Normal
    rgblight_set_layer_state(1, layer_state_cmp(state, PLAIN_LAYER)); // Plain
    rgblight_set_layer_state(2, layer_state_cmp(state, FN_LAYER_1)); // Fn1
    rgblight_set_layer_state(3, layer_state_cmp(state, FN_LAYER_2)); // Fn2
    rgblight_set_layer_state(7, layer_state_cmp(state, FUNKY_LAYER)); // Funky!
    return state;
}

void ack_signal(bool enabled){
    rgblight_blink_layer(enabled ? RGB_ACK_ON_LAYER : RGB_ACK_OFF_LAYER, RGB_ACK_BLINK_TIME);
}

void update_eeprom(){
    user_config.sexy_shift_enabled = sexy_shift_enabled;
    user_config.sweet_caps_enabled = sweet_caps_enabled;
    user_config.rshift_home_enabled = rshift_home_enabled;
    eeconfig_update_user(user_config.raw); // Writes the new status to EEPROM
}

bool led_update_user(led_t led_state) {
    //rgblight_set_layer_state(4, led_state.caps_lock); // CAPS LAYER
    set_caps_led(led_state.caps_lock || sexy_shift_on);
    return true;
}

// rshift home
void rshift_home_enable(bool enabled){
    rshift_home_enabled = enabled;
    ack_signal(enabled);
    update_eeprom();
}

void rshift_home_toggle(void){
    rshift_home_enable(!rshift_home_enabled);
}

// Sweet Caps
void sweet_caps_enable(bool enabled){
    sweet_caps_enabled = enabled;
    ack_signal(enabled);
    update_eeprom();
}
void sweet_caps_toggle(void){
    sweet_caps_enable(!sweet_caps_enabled);
}

bool sweet_caps_break(uint16_t keycode){
    switch(keycode){  // Keycodes die CAPS nicht deaktivieren.
        case KC_A:
        case KC_B:
        case KC_C:
        case KC_D:
        case KC_E:
        case KC_F:
        case KC_G:
        case KC_H:
        case KC_I:
        case KC_J:
        case KC_K:
        case KC_L:
        case KC_M:
        case KC_N:
        case KC_O:
        case KC_P:
        case KC_Q:
        case KC_R:
        case KC_S:
        case KC_T:
        case KC_U:
        case KC_V:
        case KC_W:
        case KC_X:
        case KC_Y:
        case KC_Z:
        case KC_UNSHIFT_DEL:
        case KC_DEL:
        case KC_BSPC:
        case KC_LEFT:
        case KC_RIGHT:
        case KC_SLSH:
        case KC_CAPS:
            return false;
    }
    return true;
}




void sexy_shift_enable(bool enabled){
    sexy_shift_enabled = enabled;
    ack_signal(enabled);
    update_eeprom();
}

void sexy_shift_toggle(void){
    sexy_shift_enable(!sexy_shift_enabled);
}

void sexy_shift_start_oneshot(uint16_t command_keycode, uint16_t code, uint16_t layer){
    sexy_shift_start(command_keycode,code,layer);
    sexy_shift_oneshot = true;
}

void sexy_shift_start(uint16_t command_keycode, uint16_t code, uint16_t layer){
    sexy_shift_layer = layer;
    sexy_shift_code = code;
    sexy_shift_command_keycode = command_keycode;
    layer_on(sexy_shift_layer);
    register_mods(MOD_BIT(sexy_shift_code));
    set_caps_led(true);
    sexy_shift_on = true;
    sexy_shift_tapped = true;
    sexy_shift_last_keycode = 0;        // reset last keycode
    sexy_shift_tap_timer = timer_read();
    sexy_shift_oneshot = false;
}

void sexy_shift_restart(){
    sexy_shift_start(sexy_shift_command_keycode, sexy_shift_code, sexy_shift_layer);
}

bool sexy_shift_is_tapped(){
    return sexy_shift_tapped && timer_elapsed(sexy_shift_tap_timer) < TAPPING_TERM;
}

bool sexy_shift_is_tapped_time(uint16_t term){
    return sexy_shift_tapped && timer_elapsed(sexy_shift_tap_timer) < term;
}

void sexy_shift_stop(){
    unregister_mods(MOD_BIT(sexy_shift_code));
    set_caps_led(false);
    layer_off(sexy_shift_layer);
    sexy_shift_on = false;
}

void sexy_shift_reset(){
    sexy_shift_tapped = false;
    sexy_shift_oneshot = false;
}

bool sexy_shift_ignore(uint16_t keycode){
    switch(keycode){  // Keycodes that won't stop sexy_shift
        case DP_LSFT:
        case DP_RSFT:
        case KC_1:
        case KC_8:
        case KC_9:
        case KC_SPC:
        case KC_RBRC:
        case KC_BSPC:
        case KC_UNSHIFT_DEL:
        case KC_COMM:
        case KC_DOT:
        case KC_SLSH:
            return true;
    }
    return false;
}

void sexy_shift_post_process(uint16_t keycode){
    // todo
}

void sexy_shift_process(uint16_t keycode){

    if(!sexy_shift_ignore(keycode)){
        if(sexy_shift_last_keycode == 0){
            sexy_shift_last_keycode = keycode;
            sexy_shift_tap_timer = timer_read();        // restart timer on first button
        }
        else if(keycode != sexy_shift_last_keycode && keycode != sexy_shift_command_keycode && sexy_shift_on && !sexy_shift_oneshot){
            if(timer_elapsed(sexy_shift_tap_timer) < SEXYSHIFT_TERM)
                sexy_shift_stop();
            else{
                set_caps_led(false);
            }
        }

        sexy_shift_last_keycode = keycode;
    }

    // if any key but the sexy_shift start key is pressed interrupt "tapped" mode
    if(keycode != sexy_shift_command_keycode)
        sexy_shift_tapped = false;

    sexy_shift_oneshot = false;
}


int get_dance_state (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
      if (state->interrupted && state->pressed)  return SINGLE_HOLD_INTERRUPTED;    // taste wurde während des gedrückthaltens unterbrochen
      else if (state->interrupted || !state->pressed)  return SINGLE_TAP;
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
  else return TAP_END; //magic number. At some point this method will expand to work for more presses
}


//instanalize an instance of 'tap' for the 'x' tap dance.
static tap tap_state = {
  .is_press_action = true,
  .state = 0
};



void super_AKZENT_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_EQL);
        break;
    case DOUBLE_HOLD:
        SEND_STRING("+ + + "); // ``` auf ISO-DE
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
        if(!sweet_caps_enabled && sweet_caps_was_enabled){  // enable sweet_caps when is enabled but not active
            sweet_caps_was_enabled = false;
            sweet_caps_enabled = true;
        }
        register_code(KC_CAPS);
        break;
    case SINGLE_HOLD:
        layer_on(FN_LAYER_2);
        break;
    case DOUBLE_TAP:
        if(sweet_caps_enabled){     // disable sweet_caps temporarily until next single tap
            sweet_caps_was_enabled = true;
            sweet_caps_enabled = false;
        }
        register_code(KC_CAPS);
        break;
    case DOUBLE_HOLD:
        break;
    case DOUBLE_SINGLE_TAP:
        break;
    case TRIPLE_TAP:
        break;
  }
}

void super_CAPS_reset (qk_tap_dance_state_t *state, void *user_data) {

  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_CAPS);
        break;
    case SINGLE_HOLD:
        break;
    case DOUBLE_TAP:
        unregister_code(KC_CAPS);
        break;
    case DOUBLE_HOLD:
        break;
    case DOUBLE_SINGLE_TAP:
        break;
    case TRIPLE_TAP:

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
        register_code(KC_HOME);
        break;
    case SINGLE_HOLD:
        register_code(KC_RCTL);
        break;
    case DOUBLE_TAP:
        register_code(KC_RGUI);
        break;
    case DOUBLE_HOLD:
        register_code(KC_LCTL); register_code(KC_LALT);
        break;
    case TRIPLE_TAP:
        register_code(KC_RGUI);
        break;
    default:
        register_code(KC_RCTL);
        break;
  }
}

void super_CTRL_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_HOME);
        break;
    case SINGLE_HOLD:
        unregister_code(KC_RCTL);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_RGUI);
        break;
    case DOUBLE_HOLD:
        unregister_code(KC_LCTL); unregister_code(KC_LALT);
        break;
    case TRIPLE_TAP:
        unregister_code(KC_RGUI);
        break;
    default:
        unregister_code(KC_RCTL);
        break;
  }
  tap_state.state = 0;
}

void super_PSCR_start (qk_tap_dance_state_t *state, void *user_data) {
  if (state->count == 1){
      layer_on(FUNKY_LAYER);
  }
}

void super_PSCR_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_PSCR);
        break;
    case SINGLE_HOLD:
        break;
    case DOUBLE_TAP:
        register_code(KC_LGUI); register_code(KC_LSFT); tap_code(KC_S);
        break;
    default:
        register_code(KC_PSCR);
        break;
  }
}

void super_PSCR_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_PSCR);
        break;
    case SINGLE_HOLD:
        break;
    case DOUBLE_TAP:
        unregister_code(KC_LSFT);unregister_code(KC_LGUI);
        break;
    default:
        unregister_code(KC_PSCR);
        break;
  }
    layer_off(FUNKY_LAYER);

  tap_state.state = 0;
}

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_CAPS]        = ACTION_TAP_DANCE_FN_ADVANCED(super_CAPS_start ,super_CAPS_finished, super_CAPS_reset),
    [TD_CTRL]        = ACTION_TAP_DANCE_FN_ADVANCED(NULL,super_CTRL_finished, super_CTRL_reset),
    [TD_AKZENT]         = ACTION_TAP_DANCE_FN_ADVANCED(NULL,super_AKZENT_finished, super_AKZENT_reset),
    [TD_PSCR]        = ACTION_TAP_DANCE_FN_ADVANCED(super_PSCR_start,super_PSCR_finished, super_PSCR_reset),
};



void keyboard_post_init_user(void) {  // Call the keymap level matrix init.

    // Read the user config from EEPROM
    user_config.raw = eeconfig_read_user();

    // Set RGB Layers
    rgblight_layers = my_rgb_layers;

    // Enable Default RGB Layer
    rgblight_set_layer_state(0, true);

    // Set functions from EEPRROM
    sexy_shift_enabled = user_config.sexy_shift_enabled;
    sweet_caps_enabled = user_config.sweet_caps_enabled;
    rshift_home_enabled = user_config.rshift_home_enabled;
    set_caps_led(false);

}



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // DEFAULT
        [DEFAULT_LAYER] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   TD(TD_PSCR),  KC_HOME,   KC_INS,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  TD(TD_AKZENT),   XXXXXXX,  KC_BSPC,    KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,              KC_PGDN ,
        TD(TD_CAPS),  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                      KC_ENT,      KC_DEL,
        DP_LSFT,  KC_NUBS,  KC_Z,   KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  DP_RSFT,       KC_UP,   KC_END,
        KC_LCTL,  KC_LGUI,  KC_LALT,                 KC_SPC,   KC_SPC,   KC_SPC,                KC_RALT,  LT(FN_LAYER_1 ,KC_APP),  TD(TD_CTRL),  KC_LEFT,  KC_DOWN,  KC_RGHT
    ),

    // GAMING / PLAIN
	[PLAIN_LAYER] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_PSCR,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  XXXXXXX,  KC_BSPC, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        KC_CAPS,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  KC_DEL,
        KC_LSFT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_RSFT,            _______,  _______,
        _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  LT(FN_LAYER_1 ,KC_APP),  _______,  _______,  _______,  _______
    ),

    // Shift Layer
	[SHIFT_LAYER] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  XXXXXXX,  KC_UNSHIFT_DEL, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,   _______
    ),

    // Right Shift Layer, enabled with sexy_shift
	[RSHIFT_LAYER] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  XXXXXXX,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        KC_ENT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  KC_UNSHIFT_HOME,
        _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,  _______
    ),

    // Funky Layer
	[FUNKY_LAYER] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  XXXXXXX,  _______, _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,      _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                      _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,
        _______,  _______,  _______,                      _______,  _______,  _______,                      _______,  _______,  _______,  _______,  _______,   _______
    ),

    // Disabled
	[DISABLED_LAYER] = LAYOUT(
        XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,  XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,                      XXXXXXX,  XXXXXXX,  XXXXXXX,            XXXXXXX,MO(FN_LAYER_1),XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX
    ),

    // Functions I, activated by GUI
	[FN_LAYER_1] = LAYOUT(
        DF(DEFAULT_LAYER), TG(PLAIN_LAYER), _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_PAUS, KC_SLCK , KC_NLCK,
        KC_WAKE,  _______, _______,  _______,  _______, _______,  _______,  KC_PSLS,  KC_PAST,  _______,  KC_NLCK,  _______,  MARKUP_CODE,  XXXXXXX, REMOVE_LINE, KC_ASUP,
        _______,  RGB_TOG,  RGB_MOD,  RGB_HUI,  RGB_HUD,  RGB_SAI,  RGB_SAD,  RGB_VAI,  RGB_VAD,  _______,  _______,  _______,  KC_VOLU,  KC_MUTE,       KC_ASDN,
        KC_CAPS,  BL_DEC,  BL_INC,  BL_STEP,  _______,  _______,  _______,  _______,  _______,  TG(DISABLED_LAYER),  _______,  KC_F20,            KC_PENT,             _______,
        KC_LSFT,  _______,  KC_BRIU,  KC_BRID,  _______,  _______,  _______,  _______,  _______,  _______,  KC_PDOT,  KC_VOLD,  KC_RSFT,                   KC_PGUP,  KC_ASRP,
        RESET  ,  DEBUG,  _______,                  KC_MPLY,  KC_MPLY,  KC_MPLY,                         _______,  _______,  KC_RGUI,           KC_HOME,   KC_PGDN,  KC_END
    ),

    // Functions II, activated by CAPS LOCK
    [FN_LAYER_2] = LAYOUT(
        DF(DEFAULT_LAYER),  TG_SESFT,  TG_SWCPS,  TG_RSFTHM,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  HYPR(KC_INS),
        KC_SLEP ,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______, _______,  MARKUP_CODE,  XXXXXXX, REMOVE_LINE,  KC_VOLU,
        _______,  _______,  KC_WH_U,  _______,  MEH(KC_F24),  _______,  _______,  _______,  _______,  _______,  _______,  _______, KC_VOLU,  _______,                    KC_VOLD,
        _______,  KC_WH_L,  KC_WH_D,  KC_WH_R,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,                  KC_CALC,             _______ ,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______,  _______,  KC_VOLD,  _______,                    KC_MS_UP, _______,
        _______,  KC_RGUI,  _______,                  KC_MPLY,  KC_MPLY,  KC_MPLY,                      _______,  _______,   KC_MSTP,             KC_MPRV, KC_MS_DOWN,KC_MNXT
    ),

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    if(sexy_shift_enabled && record->event.pressed)
        sexy_shift_process(keycode);

    if(sweet_caps_enabled && record->event.pressed && keycode != TD(TD_CAPS) && keycode != KC_CAPS && sweet_caps_break(keycode) && is_capslock_on()){
            tap_code(KC_CAPS);
            if(keycode == KC_ESC)   // Wenn Escape gedrückt, nicht weiter prozessieren...
                return false;
    }

    switch (keycode) {
        case DP_LSFT:   // custom left shift
            if (record->event.pressed) {
                if(sexy_shift_enabled){
                    sexy_shift_stop();
                    sexy_shift_start(keycode, KC_LSFT, SHIFT_LAYER);
                }
                else
                    register_code(KC_LSFT);
            }
            else if (!record->event.pressed) {
               if(sexy_shift_enabled && (sexy_shift_command_keycode == keycode)){
                    sexy_shift_stop();
                    /*
                    if(sexy_shift_is_tapped_time(TAPPING_TERM)){
                        tap_code(KC_HOME);
                    }
                    */
                    sexy_shift_reset();
                }
                else
                    unregister_code(KC_LSFT);
            }
            break;

        case DP_RSFT:   // custom right shift
            if (record->event.pressed) {
                if(sexy_shift_enabled){
                    sexy_shift_stop();
                    sexy_shift_start(keycode, KC_RSFT, RSHIFT_LAYER);
                }
                else
                    register_code(KC_RSFT);
            }
            else if (!record->event.pressed) {
                if(sexy_shift_enabled && (sexy_shift_command_keycode == keycode)){
                    sexy_shift_stop();
                    if(rshift_home_enabled && sexy_shift_is_tapped_time(TAPPING_TERM))
                        tap_code(KC_HOME);
                    sexy_shift_reset();
                }
                else
                    unregister_code(KC_RSFT);
            }
            break;

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

        // Shift + Backspace = Del
        case KC_UNSHIFT_DEL:
            if (record->event.pressed && get_mods() & MOD_BIT(KC_LSHIFT)) {
                unregister_code(KC_LSFT);
                register_code(KC_DEL);
                return false;
            }
            else if (!record->event.pressed) {
                unregister_code(KC_DEL);
                register_code(KC_LSFT);
            }
            break;

        // R_Shift + End = Home
        case KC_UNSHIFT_HOME:
            if (record->event.pressed && get_mods() & MOD_BIT(KC_RSHIFT)) {
                unregister_code(KC_RSFT);
                register_code(KC_HOME);
                return false;
            }
            else if (!record->event.pressed) {
                unregister_code(KC_HOME);
                register_code(KC_RSFT);
            }
            break;

        // toggle sexy shift
        case TG_SESFT:
            if (record->event.pressed) {
                sexy_shift_toggle();
            } else {

            }
            break;

        // toggle sweet caps
        case TG_SWCPS:
            if (record->event.pressed) {
                sweet_caps_toggle();
            } else {

            }
            break;

        // toggle rshift tap home
        case TG_RSFTHM:
            if (record->event.pressed) {
                rshift_home_toggle();
            } else {

            }
            break;

    }

    return true;
};

void post_process_record_user(uint16_t keycode, keyrecord_t *record){
    sexy_shift_post_process(keycode);
}

void eeconfig_init_user(void) {  // EEPROM is getting reset!
    user_config.raw = 0;
    user_config.sweet_caps_enabled = true; // We want this enabled by default
    user_config.sexy_shift_enabled = true; // We want this enabled by default
    user_config.rshift_home_enabled = true; // We want this enabled by default
    eeconfig_update_user(user_config.raw); // Write default value to EEPROM now
}

void print_keycode(uint16_t keycode) {
    char display[8];
    snprintf(display, 8, "\n%d\n", keycode);
    send_string((const char *)display);
}
