#ifndef INCLUDE_TIMING_UTILS_H_
#define INCLUDE_TIMING_UTILS_H_

// ----------- Include other modules (for public) -------------
#include "stdint.h"		

// -------------------- Public definitions --------------------
typedef struct
{
	uint32_t	time_mark;
	uint32_t	elapsed_time;

} stopwatch_handle_t;

// ---------------- Public function prototypes ----------------
void TIMUT_stopwatch_set_time_mark(stopwatch_handle_t *stopwatch);
void TIMUT_stopwatch_update(stopwatch_handle_t *stopwatch);
uint8_t TIMUT_stopwatch_has_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x);
uint8_t TIMUT_stopwatch_has_another_X_ms_passed(stopwatch_handle_t *stopwatch, uint32_t x);

void TIMUT_stopwatch_demo(void);

uint32_t TIMUT_get_stopwatch_elapsed_time(stopwatch_handle_t *stopwatch); //vaja 13

#endif 
