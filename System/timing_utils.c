// ----------- Include other modules (for private) -------------
#include "timing_utils.h"		
#include "stm32g4xx_hal.h"		
#include "LED.h"				

// ------------- Public function implementations --------------
void TIMUT_stopwatch_set_time_mark(stopwatch_handle_t *stopwatch)
{
	stopwatch->time_mark = HAL_GetTick() ;
}

void TIMUT_stopwatch_update(stopwatch_handle_t *stopwatch)
{
	stopwatch->elapsed_time = HAL_GetTick() - stopwatch->time_mark ;
}

uint8_t TIMUT_stopwatch_has_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x)
{
	TIMUT_stopwatch_update(stopwatch);

	if ( x < stopwatch->elapsed_time )
	{
		return 1;	
	}
	else
	{
		return 0;	
	}
}

uint8_t TIMUT_stopwatch_has_another_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x)
{
	if ( TIMUT_stopwatch_has_X_ms_passed(stopwatch, x) )
	{
		if (stopwatch->elapsed_time > 2*x )
		{
			TIMUT_stopwatch_set_time_mark(stopwatch);
			return 1;
		}
		else
		{
			stopwatch->time_mark += x;
			return 1;
		}
	}

	else
	{
		return 0;
	}
}

void TIMUT_stopwatch_demo(void)
{
	stopwatch_handle_t   stopwatch;
	LED_on(LED0);			
	TIMUT_stopwatch_set_time_mark(&stopwatch);					

	while ( !TIMUT_stopwatch_has_X_ms_passed(&stopwatch, 1000) )
	{

	}

	LED_off(LED0);
	TIMUT_stopwatch_set_time_mark(&stopwatch);

	while(1)
	{
		if ( TIMUT_stopwatch_has_another_X_ms_passed( &stopwatch, 1000) )
		{
			LED_toggle(LED1);
		}
	}
}

uint32_t TIMUT_get_stopwatch_elapsed_time(stopwatch_handle_t *stopwatch)
{   
	TIMUT_stopwatch_update(stopwatch);
	return stopwatch->elapsed_time;
}

// -------------- Private function implementations -------------

