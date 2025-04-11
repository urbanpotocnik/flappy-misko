#include "math_utils.h"
#include <stdlib.h>		
#include <math.h>
#include "timing_utils.h"
#include "SCI.h"	
#include "kbd.h"	


// ------------- Public function implementations --------------

void MATH_init_random_generator(void)
{
	srand( HAL_GetTick() );
}

int32_t MATH_random_integer_number_from_interval(int32_t x_min, int32_t x_max)
{
	uint32_t	interval_length;
	interval_length = x_max - x_min;
	return   ( x_min + ( rand() % (interval_length + 1) ) );
}


uint32_t MATH_calculate_distance_between_points(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint32_t dx, dy, distance;
	dx = x1 - x2;
	dy = y1 - y2;
	distance = ( (uint32_t) sqrt( dx*dx + dy*dy ) );

	return distance;
}


obstacle_positions_t MATH_randomise_distance_between_obstacles(void)
{
	obstacle_positions_t obstacle_distances;
	MATH_init_random_generator();
	obstacle_distances.obstacle_top_y = MATH_random_integer_number_from_interval(-210, -120);
	int16_t distance_between_obstacles = 100;
	int16_t obstacle_length = 240;
	obstacle_distances.obstacle_bottom_y = obstacle_distances.obstacle_top_y + distance_between_obstacles + obstacle_length;

	return obstacle_distances;
}



// ------- Test functions ---------

void MATH_demo_random_integer_number_from_interval(void)
{
	int32_t rnd;
	while( KBD_get_pressed_key() == BTN_NONE );	
	MATH_init_random_generator();


	while(1)
	{
		rnd = MATH_random_integer_number_from_interval(0,2);
		printf("random number = %li\n" ,rnd);
		HAL_Delay(500);

	}

}