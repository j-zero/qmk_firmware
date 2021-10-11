#include QMK_KEYBOARD_H


enum layer_names {
    _BASE
};

led_config_t g_led_config = { {
    // Key Matrix to LED Index

        { 6, 7, 8 },
        { 5, 4, 3 },
        { 0, 1, 2 }
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
    [_BASE] = LAYOUT_ortho_3x3(
        RGB_TOG,       RGB_MOD,       RESET,
        RGB_HUI,       RGB_SAI,       RGB_VAI,
        KC_1,           KC_2,       KC_3
    )
};
