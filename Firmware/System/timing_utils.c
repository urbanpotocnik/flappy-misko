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

// Update the existing has_X_ms_passed function to store the interval
uint8_t TIMUT_stopwatch_has_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x)
{
    stopwatch->interval = x; // Store the interval for pause/resume functionality
    TIMUT_stopwatch_update(stopwatch);

    if (x < stopwatch->elapsed_time)
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

uint32_t TIMUT_stopwatch_get_remaining_time(stopwatch_handle_t *stopwatch)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t elapsed = current_time - stopwatch->time_mark;
    // Calculate remaining time considering the last interval used in has_X_ms_passed
    return (elapsed >= stopwatch->interval) ? 0 : (stopwatch->interval - elapsed);
}

void TIMUT_stopwatch_set_time_mark_with_remaining_time(stopwatch_handle_t *stopwatch, uint32_t remaining_time)
{
    uint32_t current_time = HAL_GetTick();
    // Set time mark by subtracting the remaining time from current time
    stopwatch->time_mark = current_time - (stopwatch->interval - remaining_time);
}

void TIMUT_stopwatch_reset(stopwatch_handle_t *stopwatch) {
    TIMUT_stopwatch_set_time_mark(stopwatch);
}
// -------------- Private function implementations -------------

