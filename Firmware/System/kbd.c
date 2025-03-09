// ----------- Include other modules (for private) -------------
#include "LED.h"		
#include "kbd.h"
#include "buf.h"
#define KBD_BUF_LEN 	32

typedef struct
{
	GPIO_TypeDef *		port;
	uint32_t 			pin;
	button_sig_value_t	state_old;
	button_sig_value_t	state_new;

} button_handle_t;

typedef struct
{
	button_handle_t buttons[BTN_NUM];

} keyboard_handle_t;

keyboard_handle_t 		keyboard;		
uint8_t 		kbd_buffer[KBD_BUF_LEN];	
buf_handle_t 	kbd_buf_handle;				

// -------------- Public function implementations --------------
void KBD_init(void)
{
	keyboard.buttons[ BTN_ESC ].pin = LL_GPIO_PIN_14 ;
	keyboard.buttons[ BTN_ESC ].port = GPIOC;

	keyboard.buttons[ BTN_OK ].pin = LL_GPIO_PIN_15 ;
	keyboard.buttons[ BTN_OK ].port = GPIOC;

	keyboard.buttons[ BTN_RIGHT ].pin = LL_GPIO_PIN_8 ;
	keyboard.buttons[ BTN_RIGHT ].port = GPIOG;

	keyboard.buttons[ BTN_LEFT ].pin = LL_GPIO_PIN_6 ;
	keyboard.buttons[ BTN_LEFT ].port = GPIOG;

	keyboard.buttons[ BTN_DOWN ].pin = LL_GPIO_PIN_1 ;
	keyboard.buttons[ BTN_DOWN ].port = GPIOG;

	keyboard.buttons[ BTN_UP ].pin = LL_GPIO_PIN_0 ;
	keyboard.buttons[ BTN_UP ].port = GPIOG;

	for(int i=0; i < BTN_NUM; i++)
	{
		keyboard.buttons[i].state_new = BTN_SIG_VALUE_RELEASED;
		keyboard.buttons[i].state_old = BTN_SIG_VALUE_RELEASED;
	}

	BUF_init( &kbd_buf_handle, kbd_buffer, KBD_BUF_LEN);
}

void KBD_scan(void)
{
	for(int i=0; i < BTN_NUM; i++)
	{
		
		keyboard.buttons[i].state_old = keyboard.buttons[i].state_new ;
		keyboard.buttons[i].state_new = LL_GPIO_IsInputPinSet( keyboard.buttons[i].port, keyboard.buttons[i].pin);

		if ((keyboard.buttons[i].state_new != keyboard.buttons[i].state_old) && (keyboard.buttons[i].state_new==BTN_SIG_VALUE_PRESSED ))
		{
			BUF_store_byte( &kbd_buf_handle, i);
		}
	}
}

buttons_enum_t KBD_get_pressed_key(void)
{
	buttons_enum_t pressed_button;
	if ( BUF_get_byte( &kbd_buf_handle, &pressed_button ) == BUFFER_OK )
	{
		return pressed_button;
	}
	else
	{
		return BTN_NONE;
	}

}

button_sig_value_t KBD_get_button_state(buttons_enum_t button)
{
	return keyboard.buttons[button].state_new;
}

void KBD_demo_toggle_LEDs_if_buttons_pressed(void)
{
	buttons_enum_t pressed_key;
	pressed_key = KBD_get_pressed_key() ;

	while (pressed_key != BTN_NONE)
	{
		switch ( pressed_key )
		{
		  case BTN_OK:
			  LED_toggle(LED2);
			  break;

		  case BTN_ESC:
			  LED_toggle(LED3);
			  break;

		  case BTN_UP:
			  LED_toggle(LED4);
			  break;

		  case BTN_DOWN:
			  LED_toggle(LED5);
			  break;

		  case BTN_LEFT:
			  LED_toggle(LED6);
			  break;

		  case BTN_RIGHT:
			  LED_toggle(LED7);
			  break;

		  default:
			  break;
		}
		pressed_key = KBD_get_pressed_key();
	}
}

// ------- Test functions ---------
void KBD_demo(void)
{

	buttons_enum_t pressed_key;

	LED_init();
	KBD_init();
	KBD_scan();
	pressed_key = KBD_get_pressed_key();

	while (pressed_key != BTN_NONE)
	{
		switch (pressed_key)
		{
		case BTN_UP:

			LEDs_on(60);

			break;

		case BTN_DOWN:

			LEDs_on(195);

			break;

		case BTN_LEFT:

			LED_toggle(LED4);
			LED_toggle(LED5);
			LED_toggle(LED6);
			LED_toggle(LED7);

			break;

		case BTN_RIGHT:

			LED_toggle(LED0);
			LED_toggle(LED1);
			LED_toggle(LED2);
			LED_toggle(LED3);

			break;

		case BTN_OK:

			LEDs_on(255);

			break;

		case BTN_ESC:

			LEDs_on(255);

			break;

		  default:
			  break;
		}
		pressed_key = KBD_get_pressed_key();
	}
	if (KBD_get_button_state(BTN_OK) == BTN_SIG_VALUE_PRESSED){
		LED_demo();
	}
	HAL_Delay(10);
}


void KBD_flush(void){ 

	BUF_flush(&kbd_buf_handle);
}

// -------------- Private function implementations -------------


