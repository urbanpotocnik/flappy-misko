#ifndef INCLUDE_LED_H_
#define INCLUDE_LED_H_

// ----------- Include other modules (for public) -------------
#include "stm32g4xx_ll_gpio.h"

// -------------------- Public definitions --------------------

typedef enum {LED0,LED1,LED2,LED3,LED4,LED5,LED6,LED7, NUM_OF_LEDS} LEDs_enum_t;


void LED_init(void);

void LED_on(LEDs_enum_t LEDn);		
void LED_off(LEDs_enum_t LEDn);
void LED_toggle(LEDs_enum_t LEDn);
void LED_set(LEDs_enum_t LEDn, uint8_t state);
void LEDs_on(uint8_t LED_bitmask);
void LEDs_off(uint8_t LED_bitmask);
uint8_t LED_is_on(LEDs_enum_t LEDn);
void LEDs_write(uint8_t value);
uint8_t LEDs_read(void);
void LED_demo(void);

#endif 
