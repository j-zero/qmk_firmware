#undef MANUFACTURER
#undef VENDOR_ID
#undef PRODUCT

/* USB Device descriptor parameter */
#define VENDOR_ID       0x4B42
#define MANUFACTURER    datenpirat
#define PRODUCT         KBD75

#undef RGBLIGHT_ANIMATIONS
// #define RGBLIGHT_ANIMATIONS // save some space!
//#define RGBLIGHT_EFFECT_ALTERNATING
//#define RGBLIGHT_EFFECT_BREATHING
//#define RGBLIGHT_EFFECT_CHRISTMAS
//#define RGBLIGHT_EFFECT_KNIGHT
//#define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#define RGBLIGHT_EFFECT_RGB_TEST
//#define RGBLIGHT_EFFECT_SNAKE
#define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#define RGBLIGHT_EFFECT_TWINKLE

#define RGBLIGHT_EFFECT_KNIGHT_LED_NUM 8
#define RGBLIGHT_EFFECT_KNIGHT_OFFSET 8
#define RGBLIGHT_LAYERS
#define RGBLIGHT_LAYER_BLINK

#define TAPPING_TERM            175
#define SEXYSHIFT_TERM          100
#define SEXYSHIFT_TAPPING_MIN_TERM  0
#define SEXYSHIFT_TAPPING_MAX_TERM  125
#define RGB_ACK_BLINK_TIME      500
#define RGB_ACK_ON_LAYER        5
#define RGB_ACK_OFF_LAYER       6
#define RGBLIGHT_SLEEP
//#define NO_ACTION_ONESHOT

#define MOUSEKEY_MOVE_DELTA 10
#define MOUSEKEY_MAX_SPEED 10


#define CAPS_LED_PIN B2

//#define PS2_CLOCK_PIN   B6
//#define PS2_DATA_PIN    F7
//#undef PS2_CLOCK_PIN
//#undef PS2_DATA_PIN

//#define PS2_CLOCK_PORT  PORTB
//#define PS2_CLOCK_PIN   PINB
//#define PS2_CLOCK_DDR   DDRB
//#define PS2_CLOCK_BIT   6
//#define PS2_DATA_PORT   PORTF
//#define PS2_DATA_PIN    PINF
//#define PS2_DATA_DDR    DDRF
//#define PS2_DATA_BIT    7
/*
//#ifdef PS2_USE_INT
//    #define PS2_INT_INIT()  do {    \
//            PCICR |= (1<<PCIE0);    \
//        } while (0)
//    #define PS2_INT_ON()  do {      \
//            PCMSK0 |= (1<<PCINT6);         \
//        } while (0)
//    #define PS2_INT_OFF() do {      \
//           PCMSK0 &= ~(1<<PCINT6);        \
//        } while (0)
//    #define PS2_INT_VECT   PCINT0_vect
//#endif
*/
//#define PS2_MOUSE_SCROLL_DIVISOR_V 2
//#define PS2_MOUSE_SCROLL_DIVISOR_H 2

//#define PS2_MOUSE_SCROLL_BTN_MASK (1<<PS2_MOUSE_BTN_MIDDLE) /* Default */

//#define AUTO_BUTTONS_TIMEOUT 750