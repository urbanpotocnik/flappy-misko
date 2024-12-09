
// ----------- Include other modules (for private) -------------
#include "lcd_backlight.h"		
#include "SCI.h"				

// ---------------------- Private definitions ------------------
typedef struct
{
	
	TIM_TypeDef *	timer;
	uint32_t		timer_channel;
	void	(* SetCompare) (TIM_TypeDef * TIMx, uint32_t CompareValue);		
	uint32_t (* GetCompare) (TIM_TypeDef * TIMx);				
	uint8_t		default_brightness;

} LCD_BKLT_handle_t;

LCD_BKLT_handle_t  LCD_backlight;

// -------------- Public function implementations --------------
void LCD_BKLT_init(void)
{
	LCD_backlight.timer = TIM4 ;
	LCD_backlight.timer_channel = LL_TIM_CHANNEL_CH1 ;		

	LCD_backlight.SetCompare = LL_TIM_OC_SetCompareCH1;
	LCD_backlight.GetCompare = LL_TIM_OC_GetCompareCH1;

	LCD_backlight.default_brightness = 50;		

	LL_TIM_EnableCounter (TIM4);

	LCD_BKLT_set_brightness( LCD_backlight.default_brightness);

    LL_TIM_CC_EnableChannel ( TIM4, LL_TIM_CHANNEL_CH1);
}

uint8_t LCD_BKLT_get_brightness(void)
{
	return LCD_backlight.GetCompare(LCD_backlight.timer);
}

void LCD_BKLT_set_brightness(uint8_t brightness)
{
	return LCD_backlight.SetCompare( LCD_backlight.timer, brightness );
}

void LCD_BKLT_on(void)
{
	LCD_BKLT_set_brightness(LCD_backlight.default_brightness );
}

void LCD_BKLT_off(void)
{
	LCD_BKLT_set_brightness(0);
}

// -------------- Test functions --------------
void LCD_BKLT_demo(void)
{
	uint8_t	compare_value;
	LCD_BKLT_on();
	HAL_Delay(2000);

	LCD_BKLT_off();
	HAL_Delay(2000);

	while(1)
	{
		for(int val=0; val <= 100; val++)
		{
			LCD_BKLT_set_brightness(val);
			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			HAL_Delay(10);
		}

		HAL_Delay(1000);

		for(int val=100; val >= 0; val--)
		{
			LCD_BKLT_set_brightness(val);

			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			HAL_Delay(10);
		}
		
		HAL_Delay(1000);
	}
}





