// ----------- Include other modules (for private) -------------
#include "LED.h"

// ---------------------- Private definitions ------------------

typedef struct
{
	GPIO_TypeDef *	port;		
	uint32_t 		pin;		

} LED_handle_t;


typedef struct
{
	LED_handle_t 	LEDs[NUM_OF_LEDS];

} LED_group_handle_t;

LED_group_handle_t		LED_group;		

// -------------- Public function implementations --------------

void LED_init(void)
{
		LED_group.LEDs[LED0].pin = LL_GPIO_PIN_2;
		LED_group.LEDs[LED0].port = GPIOF;

		LED_group.LEDs[LED1].pin = LL_GPIO_PIN_3;
		LED_group.LEDs[LED1].port = GPIOF;

		LED_group.LEDs[LED2].pin = LL_GPIO_PIN_4;
		LED_group.LEDs[LED2].port = GPIOF;

		LED_group.LEDs[LED3].pin = LL_GPIO_PIN_5;
		LED_group.LEDs[LED3].port = GPIOF;

		LED_group.LEDs[LED4].pin = LL_GPIO_PIN_0;
		LED_group.LEDs[LED4].port = GPIOC;

		LED_group.LEDs[LED5].pin = LL_GPIO_PIN_1;
		LED_group.LEDs[LED5].port = GPIOC;

		LED_group.LEDs[LED6].pin = LL_GPIO_PIN_2;
		LED_group.LEDs[LED6].port = GPIOC;

		LED_group.LEDs[LED7].pin = LL_GPIO_PIN_3;
		LED_group.LEDs[LED7].port = GPIOC;

		LEDs_write(0x00);
}

void LED_on(LEDs_enum_t LEDn)
{
	LL_GPIO_SetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}

void LED_off(LEDs_enum_t LEDn)
{
	LL_GPIO_ResetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}

void LED_toggle( LEDs_enum_t LEDn )
{
	LL_GPIO_TogglePin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}

void LED_set(LEDs_enum_t LEDn, uint8_t state)
{
	if (state)
		LL_GPIO_SetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
	else
		LL_GPIO_ResetOutputPin( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}

void LEDs_on(uint8_t LED_bitmask)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LED_bitmask & bitmask )
		{
			LL_GPIO_SetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}

		bitmask <<= 1;
	}
}

void LEDs_off(uint8_t LED_bitmask)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LED_bitmask & bitmask )
		{
			LL_GPIO_ResetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}

		bitmask <<= 1;
	}
}

void LEDs_write(uint8_t value)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if (value & bitmask)
		{
			LL_GPIO_SetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}
		else
		{
			LL_GPIO_ResetOutputPin( LED_group.LEDs[i].port, LED_group.LEDs[i].pin );
		}

		bitmask <<= 1;
	}
}

uint8_t LED_is_on(LEDs_enum_t LEDn)
{
	return LL_GPIO_IsOutputPinSet( LED_group.LEDs[LEDn].port, LED_group.LEDs[LEDn].pin );
}

uint8_t LEDs_read(void)
{
	uint8_t value = 0;
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LL_GPIO_IsOutputPinSet( LED_group.LEDs[i].port, LED_group.LEDs[i].pin ) )
			value |= bitmask;

		bitmask <<= 1;
	}

	return value;
}

void LED_demo(void)
{

	LEDs_on(255);
	HAL_Delay(250);
	LEDs_off(255);
	HAL_Delay(250);

	LEDs_on(255);
	HAL_Delay(250);
	LEDs_off(255);
	HAL_Delay(250);

	LEDs_on(170);
	HAL_Delay(1000);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LED_toggle(LED0);
	LED_toggle(LED1);
	LED_toggle(LED2);
	LED_toggle(LED3);
	LED_toggle(LED4);
	LED_toggle(LED5);
	LED_toggle(LED6);
	LED_toggle(LED7);
	HAL_Delay(100);

	LEDs_on(255);
	HAL_Delay(1000);

	LEDs_off(129);
	HAL_Delay(1000);
	LEDs_off(195);
	HAL_Delay(1000);
	LEDs_off(231);
	HAL_Delay(1000);
	LEDs_off(255);
	HAL_Delay(1000);

	LEDs_on(24);
	HAL_Delay(1000);
	LEDs_on(60);
	HAL_Delay(1000);
	LEDs_on(126);
	HAL_Delay(1000);
	LEDs_on(255);
	HAL_Delay(1000);
}

// -------------- Private function implementations -------------
