#include QMK_KEYBOARD_H

#include "raw_hid.h"

#define PROTOCOL_VERSION 0x01
#define SUCCESS 0x01
#define FAILED 0xff

enum {
  PLAIN_LAYER = 0,
  FN_LAYER
};

enum custom_keycodes {
  DP_CALC = SAFE_RANGE,
  DP_CLOSECALC,
  DP_MMUTE,
  DP_TGMUTE
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

static uint8_t raw_data[RAW_EPSIZE];

int LED_OFF = 0;
int PLAIN_ENABLED = 0;

void set_num_led_red(void);
void set_num_led_green(void);
void set_num_led_off(void);
void send_0xdeadbabe(uint8_t);


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

void dance_NUM_finished (qk_tap_dance_state_t *state, void *user_data) {
  tap_state.state = get_dance_state(state);
  switch (tap_state.state) {
    case SINGLE_TAP:
        layer_invert(FN_LAYER);
        break;
    case SINGLE_HOLD:
        register_code (KC_NLCK);
        break;
    case DOUBLE_TAP:
        break;
    case DOUBLE_HOLD:
        break;
    case TRIPLE_TAP:
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
        unregister_code (KC_NLCK);
        break;
    case DOUBLE_TAP:
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
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //Tap once for Esc, twice for Caps Lock
  [TD_DOT_COMMAS]  = ACTION_TAP_DANCE_DOUBLE(KC_PDOT, KC_DOT),
  [TD_NUM] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_NUM_finished, dance_NUM_reset),
};


const rgblight_segment_t PROGMEM rgb_raw_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 0, HSV_BLACK}
);

const rgblight_segment_t PROGMEM rgb_plain_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    { 0, 15, HSV_WHITE}
);





// Now define the array of layers. Later layers take precedence
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    rgb_plain_layer,
    rgb_raw_layer
);

void set_num_led_red(void){
    writePin(B4, true);
    writePin(B5, false);
}

void set_num_led_green(void){
    writePin(B4, false);
    writePin(B5, true);
}

void set_num_led_off(void){
    writePinLow(B4);
    writePinLow(B5);
}

void send_0xdeadbabe(uint8_t cmd){
        memset(raw_data,0,RAW_EPSIZE);
        raw_data[0] = 0xde;
        raw_data[1] = 0xad;
        raw_data[2] = 0xba;
        raw_data[3] = 0xbe;
        raw_data[4] = cmd;
        raw_hid_send(raw_data, sizeof(raw_data));
}

void keyboard_post_init_user(void) {
    // Enable the LED layers
    rgblight_layers = my_rgb_layers;
    rgblight_set_layer_state(0, true);
    rgblight_setrgb(0xff,0xff,0xff);
    set_num_led_green();
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
    //rgblight_set_layer_state(0, layer_state_cmp(state, DEFAULT_LAYER));
    //rgblight_set_layer_state(0, layer_state_cmp(state, PLAIN_LAYER));
    rgblight_set_layer_state(0, layer_state_cmp(state, PLAIN_LAYER));


    PLAIN_ENABLED = layer_state_cmp(state, PLAIN_LAYER);

    if(layer_state_cmp(state, FN_LAYER)){
        set_num_led_red();
    }
    else if(PLAIN_ENABLED)
    {
        if(!LED_OFF)
            set_num_led_green();
        else
            set_num_led_off();
    }

    return state;
}

bool led_update_user(led_t led_state) {

    if(led_state.num_lock){
        LED_OFF = 0;
    }
    else{
        LED_OFF = 1;
    }

    if(PLAIN_ENABLED){
        if(led_state.num_lock)
            set_num_led_green();
        else
            set_num_led_off();
    }
    //else
    //    set_num_led_off();

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

    // sends raw hid 0xdeadbabe[00|01]
    case DP_MMUTE:
        if (record->event.pressed) {
            send_0xdeadbabe(0x00);

        } else {
            send_0xdeadbabe(0x01);
        }
        break;


    // sends raw hid 0xdeadbabe[03]
    case DP_TGMUTE:
        if (record->event.pressed) {
            send_0xdeadbabe(0x03);

        } else {
        }
        break;

    return false;
  }

  return true;

};

//In Layer declaration, add tap dance item in place of a key code

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Default numpad
   [PLAIN_LAYER] = LAYOUT_numpad_5x4(
    TD(TD_NUM),  KC_PSLS,   KC_PAST,  KC_PMNS,
    KC_P7,    KC_P8,     KC_P9,
    KC_P4,    KC_P5,     KC_P6,  KC_PPLS,
    KC_P1,    KC_P2,     KC_P3,
    KC_P0,               KC_PDOT,  KC_PENT
  ),
    // Custom Macro-Pad
   [FN_LAYER] = LAYOUT_numpad_5x4(
    _______,   KC_MPLY, KC_MUTE,   KC_VOLD,
    KC_F19,   KC_F20,   KC_F21,
    KC_F16,   KC_F17,   KC_F18,     KC_VOLU,
    KC_F13,   KC_F14,   KC_F15,
    DP_MMUTE,           DP_TGMUTE,     KC_CALC
  ),

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
