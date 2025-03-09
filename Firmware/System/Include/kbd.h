
#ifndef INCLUDE_KBD_H_
#define INCLUDE_KBD_H_

// ----------- Include other modules (for public) -------------
#include "stm32g4xx_ll_gpio.h"		

// -------------------- Public definitions --------------------
typedef enum { BTN_UP,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_OK,BTN_ESC,BTN_NUM,BTN_NONE } buttons_enum_t;
typedef enum {BTN_SIG_VALUE_PRESSED=0, BTN_SIG_VALUE_RELEASED=1} button_sig_value_t;

// ---------------- Public function prototypes ----------------
void KBD_init(void);
void KBD_scan(void);
buttons_enum_t KBD_get_pressed_key(void);
button_sig_value_t KBD_get_button_state(buttons_enum_t button);
void KBD_demo_toggle_LEDs_if_buttons_pressed(void);
void KBD_demo(void);
void KBD_flush(void); 

#endif 
