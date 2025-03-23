// ----------- Include other modules (for private) -------------
#include "joystick.h"
#include "LED.h"
#include "SCI.h"
#include <stdio.h>
#include "buf.h"

// ---------------------- Private definitions ------------------
typedef struct
{
	GPIO_TypeDef 						*port;
	uint32_t 							pin;
	joystick_button_sig_value_t			state_new;
	joystick_button_sig_value_t			state_old;

} joystick_button_handle_t;

typedef struct
{
	joystick_button_handle_t	button;
	uint16_t 	position_raw[NUM_OF_AXES];			
	uint16_t 	position_raw_min[NUM_OF_AXES];			
	uint16_t 	position_raw_max[NUM_OF_AXES];			
	uint16_t 	position_raw_range[NUM_OF_AXES];			
	TIM_HandleTypeDef 	*timer;
	ADC_HandleTypeDef 	*ADC;

} joystick_handle_t;

joystick_handle_t joystick;

#define JOY_BTN_BUF_LEN 	16
uint8_t 		joy_btn_buffer[JOY_BTN_BUF_LEN];			
buf_handle_t 	joy_btn_buf_handle;					

// ------------- Public function implementations --------------
void JOY_init(ADC_HandleTypeDef *ADC_handle, TIM_HandleTypeDef *timer_handle)
{

	joystick.button.pin = LL_GPIO_PIN_13;
	joystick.button.port = GPIOC;

	joystick.button.state_old = JOY_BTN_SIG_VALUE_RELEASED;
	joystick.button.state_new = JOY_BTN_SIG_VALUE_RELEASED;


		
	joystick.position_raw_min[X] = 820;
	joystick.position_raw_min[Y] = 739;
	joystick.position_raw_max[X] = 3101;
	joystick.position_raw_max[Y] = 3100;

	joystick.ADC = ADC_handle;
	joystick.timer = timer_handle;

	BUF_init( &joy_btn_buf_handle, joy_btn_buffer, JOY_BTN_BUF_LEN);

	HAL_ADCEx_Calibration_Start( joystick.ADC, ADC_SINGLE_ENDED );
	HAL_ADC_Start_DMA( joystick.ADC, (uint32_t *) joystick.position_raw , 2);
	HAL_TIM_Base_Start( joystick.timer);
	HAL_Delay(11);
}

void JOY_scan_button(void)
{
	joystick.button.state_old = joystick.button.state_new;
	joystick.button.state_new = LL_GPIO_IsInputPinSet( joystick.button.port, joystick.button.pin );

	if ( ( joystick.button.state_old == JOY_BTN_SIG_VALUE_RELEASED) && ( joystick.button.state_new == JOY_BTN_SIG_VALUE_PRESSED ) )
	{
		BUF_store_byte( &joy_btn_buf_handle, JOY_BTN_FIRE);
	}
}

joystick_buttons_enum_t JOY_get_pressed_key(void)
{
	joystick_buttons_enum_t 		pressed_button;
	buf_rtrn_codes_t				return_code;

	return_code = BUF_get_byte(&joy_btn_buf_handle, &pressed_button);

	if ( return_code == BUFFER_OK )
	{
		return pressed_button;
	}
	else
	{
		return JOY_BTN_NONE;
	}

}

joystick_button_sig_value_t JOY_get_button_state(void)
{
	return joystick.button.state_new;
}

void JOY_calibrate(void)
{
	joystick_buttons_enum_t pressed_key = JOY_BTN_NONE;

	while (pressed_key == JOY_BTN_NONE)
	{

		if ( joystick.position_raw[X]<joystick.position_raw_min[X] )
		{
			joystick.position_raw_min[X] =  joystick.position_raw[X];
		}

		if (joystick.position_raw[Y]<joystick.position_raw_min[Y] )
		{
			joystick.position_raw_min[Y] = joystick.position_raw[Y];
		}

		if ( joystick.position_raw[X]>joystick.position_raw_max[X] )
		{
			joystick.position_raw_max[X] = joystick.position_raw[X];

		}

		if ( joystick.position_raw[Y]>joystick.position_raw_max[Y] )
		{
			joystick.position_raw_max[Y] = joystick.position_raw[Y];
		}



		joystick.position_raw_range[X] = joystick.position_raw_max[X]-joystick.position_raw_min[X];
		joystick.position_raw_range[Y] = joystick.position_raw_max[Y]-joystick.position_raw_min[Y];

		JOY_scan_button(); 
		pressed_key = JOY_get_pressed_key();

		printf("******** CALIBRATION IN PROGRESS ********\n");
		JOY_SCI_send_status();

		HAL_Delay(100);
	}
}

uint8_t JOY_get_axis_position(joystick_axes_enum_t axis)
{
	int16_t	relative_position;
	relative_position = ((joystick.position_raw[axis]-joystick.position_raw_min[axis])*100)/(joystick.position_raw_range[axis]);

	if (relative_position > 100  )
	{
		return 100 ;
	}
	else if ( relative_position < 0 )
	{
		return 0 ;
	}
	else
	{
		return relative_position;
	}

}

// ------- Test functions ---------
void JOY_SCI_send_status(void)
{
	printf("--- Joystick status ---\n");
	printf("X_RAW = %d\n", joystick.position_raw[X] );
	printf("Y_RAW = %d\n", joystick.position_raw[Y] );
	printf("X_RAW_MIN = %d\n", joystick.position_raw_min[X] );
	printf("X_RAW_MAX = %d\n", joystick.position_raw_max[X] );
	printf("Y_RAW_MIN = %d\n", joystick.position_raw_min[Y] );
	printf("Y_RAW_MAX = %d\n", joystick.position_raw_max[Y] );
	printf("X_RAW_RANGE = %d\n", joystick.position_raw_range[X] );
	printf("Y_RAW_RANGE = %d\n", joystick.position_raw_range[Y] );

	printf("X = %d\n",  JOY_get_axis_position(X) );
	printf("Y = %d\n", JOY_get_axis_position(Y)  );

	printf("\n");
}

void JOY_button_demo(void)
{

	joystick_buttons_enum_t pressed_key;
	JOY_scan_button();
	pressed_key = JOY_get_pressed_key();

	while (pressed_key != JOY_BTN_NONE)
	{
		switch (pressed_key)
		{
		  case JOY_BTN_FIRE:

			  LED_toggle(LED7);
			  break;

		  default:
			  break;
		}

		pressed_key = JOY_get_pressed_key();
	}

	LED_set(LED7, JOY_get_button_state() );
	HAL_Delay(10);
}

void JOY_LED_demo(void)
{

}

