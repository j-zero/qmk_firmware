#include QMK_KEYBOARD_H

#include "raw_hid.h"

#define PROTOCOL_VERSION 0x01
#define SUCCESS 0x01
#define FAILED 0xff

enum {
  DEFAULT_LAYER = 0,
  PLAIN_LAYER,
  FN_LAYER,
  CALC_LAYER,
  CALC_FN_LAYER,
  CONF_LAYER,
};

enum custom_keycodes {
  DP_CALC = SAFE_RANGE,
  DP_CLOSECALC
};

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

enum RAW_COMMAND_ID
{
    RAW_COMMAND_GET_PROTOCOL_VERSION=0x01,

    RAW_COMMAND_RGBLIGHT_SETRGB=0xC0,
    RAW_COMMAND_RGBLIGHT_SETHSV=0xC1,
    RAW_COMMAND_RGBLIGHT_STEP=0xC2,
    RAW_COMMAND_RGBLIGHT_SET=0xC3,
    RAW_COMMAND_RGBLIGHT_ENABLE=0xC4,
    RAW_COMMAND_RGBLIGHT_DISABLE=0xC5,

    RAW_COMMAND_LAYER_INVERT=0xB1,
    RAW_COMMAND_LAYER_ON=0xB2,
    RAW_COMMAND_LAYER_OFF=0xB3,
    RAW_COMMAND_LAYER_REPORT=0xB0,
    RAW_COMMAND_UNDEFINED=0xff,
};


int LED_RED = 1;
int LED_GREEN = 0;
int LED_OFF = 0;

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

void dance_KC6_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code (KC_F);
        break;
    case DOUBLE_TAP:
        register_code(KC_LSFT); register_code(KC_NUBS);
        break;
    default:
        break;
  }
}

void dance_KC6_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code (KC_F);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_NUBS); unregister_code(KC_LSFT);
        break;
    default:
        break;
  }
  tap_state.state = 0;
}

void dance_KC8_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_LSFT); register_code(KC_8);
        break;
    case DOUBLE_TAP:
        register_code (KC_P8);
        break;
    default:
        //register_code(KC_LSFT); register_code(KC_8);
        break;
  }
}

void dance_KC8_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_8); unregister_code(KC_LSFT);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_P8);
        break;
    default:
        //unregister_code(KC_8); unregister_code(KC_LSFT);
        break;
  }
  tap_state.state = 0;
}

void dance_KC9_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        register_code(KC_LSFT); register_code(KC_9);
        break;
    case DOUBLE_TAP:
        register_code(KC_P9);
        break;
    default:
        //register_code (KC_P9);
        break;
  }
}

void dance_KC9_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        unregister_code(KC_9); unregister_code(KC_LSFT);
        break;
    case DOUBLE_TAP:
        unregister_code(KC_P9);
        break;
    default:
        //unregister_code (KC_P9);
        break;
  }
  tap_state.state = 0;
}

void dance_NUM_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        layer_invert(CALC_LAYER);
        break;
    case SINGLE_HOLD:
        layer_invert(PLAIN_LAYER);
        break;
    case DOUBLE_TAP:
        register_code (KC_NLCK);
        break;
    case DOUBLE_HOLD:
        break;
    case TRIPLE_TAP:
        layer_invert(CONF_LAYER);
        break;
    case TRIPLE_HOLD:
        reset_keyboard();
        break;
    default:
        break;
  }
}

void dance_NUM_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (tap_state.state) {
    case SINGLE_TAP:
        break;
    case SINGLE_HOLD:
        break;
    case DOUBLE_TAP:
        unregister_code (KC_NLCK);
        break;
    case DOUBLE_HOLD:
        break;
    case TRIPLE_TAP:
        break;
    default:

        break;
  }
  tap_state.state = 0;
}

//Tap Dance Declarations
enum {
    TD_DOT_COMMAS = 0,
    TD_NUM,
    TD_KC1,
    TD_KC2,
    TD_KC3,
    TD_KC4,
    TD_KC5,
    TD_KC6,
    TD_KC7,
    TD_KC8,
    TD_KC9,
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //Tap once for Esc, twice for Caps Lock
  [TD_DOT_COMMAS]  = ACTION_TAP_DANCE_DOUBLE(KC_PDOT, KC_DOT),
  [TD_NUM] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_NUM_finished, dance_NUM_reset),
  [TD_KC1] = ACTION_TAP_DANCE_DOUBLE(KC_A, KC_X),
  [TD_KC2] = ACTION_TAP_DANCE_DOUBLE(KC_B, KC_X),
  [TD_KC3] = ACTION_TAP_DANCE_DOUBLE(KC_C, KC_X),
  [TD_KC4] = ACTION_TAP_DANCE_DOUBLE(KC_D, KC_NUBS),
  [TD_KC5] = ACTION_TAP_DANCE_DOUBLE(KC_E, KC_X),
  [TD_KC6] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_KC6_finished, dance_KC6_reset),
  [TD_KC7] = ACTION_TAP_DANCE_DOUBLE(KC_P7, KC_X),
  [TD_KC8] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_KC8_finished, dance_KC8_reset),
  [TD_KC9] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_KC9_finished, dance_KC9_reset)
// Other declarations would go here, separated by commas, if you have them
};

const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 0, HSV_RED}
);
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_GREEN}
);
const rgblight_segment_t PROGMEM my_layer3_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_BLUE}
);
const rgblight_segment_t PROGMEM my_layer4_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_ORANGE}
);
const rgblight_segment_t PROGMEM my_layer5_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_PURPLE}
);
const rgblight_segment_t PROGMEM my_layer6_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_CYAN}
);

// etc..

// Now define the array of layers. Later layers take precedence
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_layer1_layer,    // Overrides caps lock layer
    my_layer2_layer,     // Overrides other layers
    my_layer3_layer,     // Overrides other layers
    my_layer4_layer,     // Overrides other layers
    my_layer5_layer,     // Overrides other layers
    my_layer6_layer     // Overrides other layers
);

void keyboard_post_init_user(void) {
    // Enable the LED layers
    rgblight_layers = my_rgb_layers;
    rgblight_set_layer_state(0, true);
    writePin(B4, true);
    writePin(B5, false);
}


layer_state_t layer_state_set_user(layer_state_t state) {

#ifdef RAW_ENABLE
    uint8_t *report = malloc(sizeof(uint8_t) * RAW_EPSIZE);
    report[0] = RAW_COMMAND_LAYER_REPORT;
    report[1] = 0x01;
    report[2] = biton32(state);
    raw_hid_send(report,RAW_EPSIZE);
    free(report);
#endif

    // Both layers will light up if both kb layers are active
    rgblight_set_layer_state(0, layer_state_cmp(state, DEFAULT_LAYER));
    rgblight_set_layer_state(1, layer_state_cmp(state, PLAIN_LAYER));
    rgblight_set_layer_state(2, layer_state_cmp(state, FN_LAYER));
    rgblight_set_layer_state(3, layer_state_cmp(state, CALC_LAYER));
    rgblight_set_layer_state(4, layer_state_cmp(state, CALC_FN_LAYER));
    rgblight_set_layer_state(5, layer_state_cmp(state, CONF_LAYER));

    writePinLow(B4);
    writePinLow(B5);

    LED_RED = !layer_state_cmp(state, CALC_LAYER);
    LED_GREEN = layer_state_cmp(state, CALC_LAYER);

    if(!LED_OFF){
        writePin(B4, LED_RED);
        writePin(B5, LED_GREEN);
    }



    return state;
}

bool led_update_user(led_t led_state) {
    writePinLow(B4);
    writePinLow(B5);
    //rgblight_set_layer_state(1, !led_state.num_lock);

    if(!led_state.num_lock){
        writePinLow(B4);
        writePinLow(B5);
        LED_OFF = 1;
    }
    else{
        writePin(B4, LED_RED);
        writePin(B5, LED_GREEN);
        LED_OFF = 0;
    }




    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

/*
#ifdef RAW_ENABLE
    if(key_event_report && record -> event.pressed) // only sent when pressed
    {
        keypos_t key = record->event.key;
        uint8_t *report = malloc(sizeof(uint8_t) * RAW_EPSIZE);
        report[0] = RAW_COMMAND_REPORT_KEY_EVENT;
        report[1] = 0x02;
        report[2] = key.col;
        report[3] = key.row;
        raw_hid_send(report,RAW_EPSIZE);
        free(report);
    }
#endif
*/
  switch (keycode) {
    case DP_CALC:
        if (record->event.pressed) {
            layer_invert(CALC_LAYER);
            register_code(KC_CALC);
        } else {
            unregister_code(KC_CALC);
        }

    case DP_CLOSECALC:
        if (record->event.pressed) {
            layer_invert(CALC_LAYER);
            register_code(KC_LALT); tap_code(KC_F4);
        } else {
            unregister_code(KC_LALT);
        }

        return false;
  }
  return true;

};

//In Layer declaration, add tap dance item in place of a key code

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// Custom Macro-Pad
  [DEFAULT_LAYER] = LAYOUT_numpad_5x4(
    TD(TD_NUM),   KC_MPLY, KC_MUTE, KC_VOLD,
    KC_P7,   KC_P8,   KC_P9,
    KC_P4,   KC_P5,   KC_P6,        KC_VOLU,
    KC_P1,   KC_P2,   KC_P3,
    LT(FN_LAYER, KC_P0), KC_PDOT, KC_PENT
  ),
// Default numpad
   [PLAIN_LAYER] = LAYOUT_numpad_5x4(
    _______,  KC_PSLS,   KC_PAST,  KC_PMNS,
    _______,  _______,   _______,
    _______,  _______,   _______,  KC_PPLS,
    _______,  _______,   _______,
    _______,             _______,  _______
  ),

  [FN_LAYER] = LAYOUT_numpad_5x4(
     KC_F19,  KC_PSLS,   KC_PAST,  KC_PMNS,
    _______,  _______,   _______,
    _______,  _______,   _______,  KC_PPLS,
    _______,  _______,   _______,
    _______,             KC_DOT,   DP_CALC
  ),


  [CALC_LAYER] = LAYOUT_numpad_5x4(
    _______,  KC_PSLS,   KC_PAST,   KC_PMNS,
    _______,  _______,   _______,
    _______,  _______,   _______, KC_PPLS,
    _______,     _______,      _______,
    LT(CALC_FN_LAYER, KC_P0),  KC_PDOT,  _______
  ),

    [CALC_FN_LAYER] = LAYOUT_numpad_5x4(
    LALT(KC_2),  LSFT(KC_5),   KC_ESC,      KC_F6,
    TD(TD_KC7), TD(TD_KC8), TD(TD_KC9),
    TD(TD_KC4), TD(TD_KC5), TD(TD_KC6),  KC_F5,
    TD(TD_KC1), TD(TD_KC2), TD(TD_KC3),
    _______,             KC_BSPC,        LALT(KC_4)
  ),

  [CONF_LAYER] = LAYOUT_numpad_5x4(
    KC_TRNS,   RGB_MOD,  RGB_TOG, RGB_HUD,
    RGB_VAI,   RGB_SAI,  KC_PGUP,
    KC_BSPC,   KC_ESC,   KC_MPLY, RGB_HUI,
    RGB_VAD,   RGB_SAD,  KC_PGDN,
    KC_NO,     KC_PDOT,           DP_CALC
  )
};

void raw_hid_receive( uint8_t *data, uint8_t length )
{
    uint8_t *command_id = &(data[0]);
    uint8_t *command_data = &(data[1]);
    switch ( *command_id )
    {
        case RAW_COMMAND_GET_PROTOCOL_VERSION:
        {
            command_data[0]=0x01;
            command_data[1]=PROTOCOL_VERSION;
            break;
        }
        case RAW_COMMAND_RGBLIGHT_SETHSV:
        {
            if(command_data[0] != 3 )
            {
                command_data[0]=0x01;
                command_data[1]=FAILED;
            }
            else
            {
                rgblight_sethsv(command_data[1], command_data[2], command_data[3]);
                command_data[0]=0x04;
                command_data[4]=SUCCESS;
            }
            break;
        }
        case RAW_COMMAND_RGBLIGHT_SETRGB:
        {
            if(command_data[0] != 3 )
            {
                command_data[0]=0x01;
                command_data[1]=FAILED;
            }
            else
            {
                rgblight_setrgb(command_data[1], command_data[2], command_data[3]);
                command_data[0]=0x04;
                command_data[4]=SUCCESS;
            }
            break;
        }
        case RAW_COMMAND_RGBLIGHT_STEP:
        {
            rgblight_step();
            command_data[0]=0x03;
            command_data[1]=rgblight_is_enabled();
            command_data[2]=rgblight_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGBLIGHT_SET:
        {
            rgblight_mode(command_data[1]);
            command_data[0]=0x03;
            command_data[1]=rgblight_is_enabled();
            command_data[2]=rgblight_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGBLIGHT_ENABLE:
        {
            rgblight_enable();
            command_data[0]=0x03;
            command_data[1]=rgblight_is_enabled();
            command_data[2]=rgblight_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGBLIGHT_DISABLE:
        {
            rgblight_disable();
            command_data[0]=0x03;
            command_data[1]=rgblight_is_enabled();
            command_data[2]=rgblight_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_LAYER_REPORT:
        {
            command_data[0]=0x01;
            command_data[1]=biton32(layer_state);
            break;
        }
        case RAW_COMMAND_LAYER_INVERT:
        case RAW_COMMAND_LAYER_ON:
        case RAW_COMMAND_LAYER_OFF:
        {

            command_data[0]=0x02;   // length

            uint8_t new_layer = command_data[1];

            if(*command_id == RAW_COMMAND_LAYER_INVERT)
                layer_invert(new_layer);
            else if(*command_id == RAW_COMMAND_LAYER_ON)
                layer_on(new_layer);
            else if(*command_id == RAW_COMMAND_LAYER_OFF)
                layer_off(new_layer);


            command_data[1]=biton32(layer_state);

            if(biton32(layer_state) == new_layer)
                command_data[2]=SUCCESS;
            else{
                //command_data[1] = *command_id;
                command_data[2]=FAILED;
            }

            *command_id=RAW_COMMAND_LAYER_REPORT;

            break;
        }
        default: //0xff ...
        {
            *command_id=RAW_COMMAND_UNDEFINED;
            command_data[0]=0x01;
            command_data[1]=FAILED;
            break;
        }
    }
    raw_hid_send(data,length);
}
