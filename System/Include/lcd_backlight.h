#ifndef INCLUDE_LCD_BACKLIGHT_H_
#define INCLUDE_LCD_BACKLIGHT_H_

// ----------- Include other modules (for public) -------------
#include "stm32g4xx_ll_tim.h"

// ---------------- Public function prototypes ----------------
void LCD_BKLT_init(void);
void LCD_BKLT_set_brightness(uint8_t brightness);
uint8_t LCD_BKLT_get_brightness(void);
void LCD_BKLT_on(void);
void LCD_BKLT_off(void);
void LCD_BKLT_demo(void);

#endif
