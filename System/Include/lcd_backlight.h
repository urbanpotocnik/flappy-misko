/*
 * lcd_backlight.h
 *
 *  Created on:
 *      Author:
 */

#ifndef INCLUDE_LCD_BACKLIGHT_H_
#define INCLUDE_LCD_BACKLIGHT_H_




// ----------- Include other modules (for public) -------------


// Vključimo nizko-nivojsko LL knjižnjico, da dobimo podporo za upravljanje časovnikov.
#include "stm32g4xx_ll_tim.h"


// Pri implementaciji sistemskih funkcij za upravljanje časovnika bomo potrebovali sledeče nizko-nivojske funkcije:
//		- void LL_TIM_EnableCounter (TIM_TypeDef * TIMx)
// 		- void LL_TIM_CC_EnableChannel (TIM_TypeDef * TIMx, uint32_t Channels)
//		- void LL_TIM_OC_SetCompareCH1 (TIM_TypeDef * TIMx, uint32_t CompareValue)
//		- uint32_t LL_TIM_OC_GetCompareCH1 (TIM_TypeDef * TIMx)





// -------------------- Public definitions --------------------



// ---------------- Public function prototypes ----------------



void LCD_BKLT_init(void);

void LCD_BKLT_set_brightness(uint8_t brightness);
uint8_t LCD_BKLT_get_brightness(void);

void LCD_BKLT_on(void);
void LCD_BKLT_off(void);

void LCD_BKLT_demo(void);






#endif /* INCLUDE_LCD_BACKLIGHT_H_ */
