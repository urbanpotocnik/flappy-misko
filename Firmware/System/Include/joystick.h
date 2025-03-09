#ifndef INCLUDE_JOYSTICK_H_
#define INCLUDE_JOYSTICK_H_

// ----------- Include other modules (for public) -------------
#include "stm32g4xx_hal.h"			
#include "stm32g4xx_ll_gpio.h"		

// -------------------- Public definitions --------------------
typedef enum
{
	X,
	Y,
	NUM_OF_AXES

} joystick_axes_enum_t;

typedef enum {JOY_BTN_FIRE , JOY_NUM_OF_BTNS, 	JOY_BTN_NONE}		joystick_buttons_enum_t;
typedef enum {JOY_BTN_SIG_VALUE_PRESSED = 0, JOY_BTN_SIG_VALUE_RELEASED = 1}	joystick_button_sig_value_t;

// ---------------- Public function prototypes ----------------
void JOY_init(ADC_HandleTypeDef *adc, TIM_HandleTypeDef *tim);
void JOY_scan_button(void);
joystick_buttons_enum_t JOY_get_pressed_key(void);
joystick_button_sig_value_t JOY_get_button_state(void);
void JOY_calibrate(void);
uint8_t JOY_get_axis_position(joystick_axes_enum_t axis);
void JOY_button_demo(void);
void JOY_LED_demo(void);
void JOY_SCI_send_status(void);

#endif 
