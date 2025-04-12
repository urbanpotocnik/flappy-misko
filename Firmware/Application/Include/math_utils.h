#ifndef INCLUDE_MATH_UTILS_H_
#define INCLUDE_MATH_UTILS_H_

#include <stdio.h>		
#include <stdint.h>

// ---------------- Public function prototypes ----------------

typedef struct 
{
    int16_t obstacle_top_y;
    int16_t obstacle_bottom_y;

} obstacle_positions_t;


void MATH_init_random_generator(void);
int32_t MATH_random_integer_number_from_interval(int32_t x_min, int32_t x_max);
int8_t MATH_random_sign(void);
uint32_t MATH_calculate_distance_between_points(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void MATH_demo_random_integer_number_from_interval(void);
obstacle_positions_t MATH_randomise_distance_between_obstacles(void);

#endif /* INCLUDE_MATH_UTILS_H_ */
