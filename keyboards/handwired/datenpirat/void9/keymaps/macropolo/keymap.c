
#include QMK_KEYBOARD_H

#include "raw_hid.h"

#define PROTOCOL_VERSION 0x01
#define SUCCESS 0x01
#define FAILED 0xff


#define CMD_VOL_UP      0x0a
#define CMD_VOL_DOWN    0x0b

static uint8_t raw_data[RAW_EPSIZE];
void send_0xdeadbabe(uint8_t);

bool encoder_rawhid = false;

enum RAW_COMMAND_ID
{
    RAW_COMMAND_GET_PROTOCOL_VERSION=0x01,

    RAW_COMMAND_RGB_MATRIX_SETRGB=0xC0,
    RAW_COMMAND_RGB_MATRIX_SETHSV=0xC1,
    RAW_COMMAND_RGB_MATRIX_STEP=0xC2,
    RAW_COMMAND_RGB_MATRIX_SET=0xC3,
    RAW_COMMAND_RGB_MATRIX_ENABLE=0xC4,
    RAW_COMMAND_RGB_MATRIX_DISABLE=0xC5,
    RAW_COMMAND_RGB_MATRIX_SET_SINGLE=0xC6,

    RAW_COMMAND_LAYER_INVERT=0xB1,
    RAW_COMMAND_LAYER_ON=0xB2,
    RAW_COMMAND_LAYER_OFF=0xB3,
    RAW_COMMAND_LAYER_REPORT=0xB0,
    RAW_COMMAND_UNDEFINED=0xff,
};

enum layer_names {
    _BASE,
    _MEDIA
};

enum custom_keycodes {
  DP_TGMUTE = SAFE_RANGE,
  DP_MMUTE,
  DP_TMUTE,
  ENC_BUTTON,
  DP_SWITCH,
};

led_config_t g_led_config = { {
    // Key Matrix to LED Index
        { 8, 7, 6 },
        { 3, 4, 5 },
        { 2, 1, 0 }
    }, {
    // LED Index to Physical Position
        { 0,   0 }, { 112,  0 }, { 224,  0 },
        { 0,  32 }, { 112, 32 }, { 224, 32 },
        { 0,  64 }, { 112, 64 }, { 224, 64 }
    },
    {
        // LED Index to Flag
        4,4,4,4,4,4,4,4,4
    } 
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base
 * ,--------------------.
 * |   7  |   8  |   9  |
 * |------+------+------|
 * |   4  |   5  |   6  |
 * |------+------+------|
 * |   1  |   2  |   3  |
 * `--------------------'
 */
 /*
    [_BASE] = LAYOUT_ortho_3x3(
        RESET,       KC_MSTP,    KC_VOLU,
        DP_TMUTE,    KC_MPLY,    DP_SWITCH,
        DP_MMUTE,    DP_TGMUTE,  ENC_BUTTON
    ),
    */
    [_BASE] = LAYOUT_ortho_3x3(
        RESET,       KC_MSTP,    RESET,
        DP_TMUTE,    KC_MPLY,    DP_SWITCH,
        DP_MMUTE,    DP_TGMUTE,  ENC_BUTTON
    ),
    [_MEDIA] = LAYOUT_ortho_3x3(
        RESET,       KC_MSTP,    KC_VOLU,
        DP_TMUTE,    KC_MPLY,    KC_VOLD,
        DP_MMUTE,    DP_TGMUTE,  ENC_BUTTON
    )
};

//Tap Dance Definitions
qk_tap_dance_action_t tap_dance_actions[] = {
  //Tap once for Esc, twice for Caps Lock
  //[TD_DOT_COMMAS]  = ACTION_TAP_DANCE_DOUBLE(KC_PDOT, KC_DOT),
  //[TD_NUM] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_NUM_finished, dance_NUM_reset),
};


void send_0xdeadbabe(uint8_t cmd){
        memset(raw_data,0,RAW_EPSIZE);
        raw_data[0] = 0xde;
        raw_data[1] = 0xad;
        raw_data[2] = 0xba;
        raw_data[3] = 0xbe;
        raw_data[4] = cmd;
        raw_hid_send(raw_data, sizeof(raw_data));
}

layer_state_t layer_state_set_user(layer_state_t state) {

#ifdef RAW_ENABLE
    uint8_t *report = malloc(sizeof(uint8_t) * RAW_EPSIZE);
    report[0] = RAW_COMMAND_LAYER_REPORT;
    report[1] = 0x01;
    report[2] = biton32(state);
    raw_hid_send(report,RAW_EPSIZE);
    free(report);
    encoder_rawhid = state == 0 ? true : false;
#endif

    
    return state;
}

void rgb_matrix_indicators_kb(void) {    
    //rgb_matrix_set_color(index, red, green, blue);
}

void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {    
    
    if (host_keyboard_led_state().caps_lock) {    
        /*    
        for (uint8_t i = led_min; i <= led_max; i++) {            
            if (g_led_config.flags[i] & LED_FLAG_KEYLIGHT) {                
                rgb_matrix_set_color(i, RGB_RED);            
            }        
        } 
        */   
        //rgb_matrix_set_color(0, RGB_RED); 
    }
    
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
            //tap_code(KC_VOLU);
            //if(encoder_rawhid)
            //    send_0xdeadbabe(CMD_VOL_UP);
            //else
                tap_code(KC_VOLU);
        } else {
            //if(encoder_rawhid)
            //    send_0xdeadbabe(CMD_VOL_DOWN);
            //else
                tap_code(KC_VOLD);
        }
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

/*
#ifdef RAW_ENABLE
    if(key_event_report){
        if(record -> event.pressed) // only sent when pressed
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
        else{
            keypos_t key = record->event.key;
            uint8_t *report = malloc(sizeof(uint8_t) * RAW_EPSIZE);
            report[0] = RAW_COMMAND_REPORT_KEY_EVENT;
            report[1] = 0x03;
            report[2] = key.col;
            report[3] = key.row;
            raw_hid_send(report,RAW_EPSIZE);
            free(report);
        }
    }
#endif
*/
  switch (keycode) {

    // sends raw hid 0xdeadbabe[00|01]
    case DP_MMUTE:
        if (record->event.pressed) {
            send_0xdeadbabe(0x00);
            //rgb_matrix_set_color(0, RGB_GREEN); 

        } else {
            send_0xdeadbabe(0x01);
            //rgb_matrix_set_color(0, RGB_RED); 
        }
        break;

        // sends raw hid 0xdeadbabe[00|01]
    case DP_TMUTE:
        if (record->event.pressed) {
            send_0xdeadbabe(0x05);
            //rgb_matrix_set_color(0, RGB_GREEN); 

        } else {
            send_0xdeadbabe(0x06);
            //rgb_matrix_set_color(0, RGB_RED); 
        }
        break;

    case ENC_BUTTON:
        if (record->event.pressed) {
            send_0xdeadbabe(0x0c);
            //rgb_matrix_set_color(0, RGB_GREEN); 

        } else {
            send_0xdeadbabe(0x0d);
            //rgb_matrix_set_color(0, RGB_RED); 
        }
        break;
        
    case DP_SWITCH:
        if (record->event.pressed) {
            send_0xdeadbabe(0x0e);
            //rgb_matrix_set_color(0, RGB_GREEN); 

        } else {
            send_0xdeadbabe(0x0f);
            //rgb_matrix_set_color(0, RGB_RED); 
        }
        break;

    // sends raw hid 0xdeadbabe[03]
    case DP_TGMUTE:
        if (record->event.pressed) {
            send_0xdeadbabe(0x03);
            //rgb_matrix_mode(0);
            //rgb_matrix_set_color_all(255, 0, 0);

        } else {
        }
        break;
    

    return false;
  }

  return true;

};

void raw_hid_receive( uint8_t *data, uint8_t length )
{

    uint8_t *command_id = &(data[0]);
    uint8_t *command_data = &(data[1]);

    command_data[8]=rgb_matrix_get_mode();

    switch ( *command_id )
    {
        case RAW_COMMAND_GET_PROTOCOL_VERSION:
        {
            command_data[0]=0x01;
            command_data[1]=PROTOCOL_VERSION;
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_SETHSV:
        {
            if(command_data[0] != 3 )
            {
                command_data[0]=0x01;
                command_data[1]=FAILED;
            }
            else
            {
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_dummy_effect);
                rgb_matrix_set_color_all(command_data[1], command_data[2], command_data[3]);
                command_data[0]=0x04;
                command_data[4]=FAILED;
            }
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_SETRGB:
        {
            if(command_data[0] != 3 )
            {
                command_data[0]=0x01;
                command_data[1]=FAILED;
            }
            else
            {
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_dummy_effect);
                rgb_matrix_set_color_all(command_data[1], command_data[2], command_data[3]);
                command_data[0]=0x04;
                command_data[4]=SUCCESS;
            }
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_SET_SINGLE:
        {
            if(command_data[0] != 4 )
            {
                //command_data[0]=0x01;
                command_data[1]=FAILED;
            }
            else
            {
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_dummy_effect);
                rgb_matrix_set_color(command_data[1], command_data[2], command_data[3],command_data[4]);
                command_data[0]=0x05;
                command_data[5]=SUCCESS;
            }
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_STEP:
        {
            rgb_matrix_step();
            command_data[0]=0x03;
            command_data[1]=rgb_matrix_is_enabled();
            command_data[2]=rgb_matrix_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_SET:
        {
            rgb_matrix_mode(command_data[1]);
            command_data[0]=0x03;
            command_data[1]=rgb_matrix_is_enabled();
            command_data[2]=rgb_matrix_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_ENABLE:
        {
            rgb_matrix_enable();
            command_data[0]=0x03;
            command_data[1]=rgb_matrix_is_enabled();
            command_data[2]=rgb_matrix_get_mode();
            command_data[3]=SUCCESS;
            break;
        }
        case RAW_COMMAND_RGB_MATRIX_DISABLE:
        {
            rgb_matrix_disable();
            command_data[0]=0x03;
            command_data[1]=rgb_matrix_is_enabled();
            command_data[2]=rgb_matrix_get_mode();
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