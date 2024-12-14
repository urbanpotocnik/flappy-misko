/*
 * objects.h
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_OBJECTS_H_
#define INCLUDE_OBJECTS_H_






// ----------- Include other modules (for public) -------------




// ------ Standard C libraries -----

#include "stdint.h"		// support for data type definitions
#include <stdio.h>		// add sprintf() functionality




// -------- Application level modules -------

#include "images.h"			// definitions for image objects
#include "graphics.h"		// support for graphic objects

#include "math_utils.h"		// different math functions (random number generation etc.)







// -------------------- Public definitions --------------------



typedef struct
{
	// min/max limits for velocity of moving objects
	velocity_t	abs_velocity_min;
	velocity_t	abs_velocity_max;
	uint32_t	game_play_update_period;		// duration between game play updates (in milliseconds)
} settings_t;



typedef struct
{
	int16_t	score;

} game_status_t;






// canvas object (parameters about area where graphics is drawn)
typedef struct
{
	location_t	whole_area;
	location_t	above_ground;
	location_t	below_ground;

} canvas_t;





// ------ COMPOUND OBJECTS TYPE DEFINITIONS ---------


// ---- Crosshair object type definitions ----

typedef struct
{
	// graphic object
	graphic_object_t	gfx_object;

	// crosshair specific parameters
	uint16_t	distance_to_target_center;		// distance in pixels

} crosshair_object_t;















// ------------- OBJECT ARE EXTERNAL GLOBAL VARIABLES  -----------

// This simplifies inter-module data transfer at the risk of exposing data.

extern settings_t 		settings;
extern game_status_t	game_status;
extern canvas_t			canvas;



extern graphic_object_t	intro_sprite;
extern graphic_object_t	background;

extern graphic_object_t	game_over_sprite;
extern graphic_object_t press_ok_sprite;

extern graphic_object_t bird;
extern graphic_object_t obstacleup;
extern graphic_object_t obstacledown;


extern text_object_t	score_box_title;
extern text_object_t	score_text;









// ---------------- Public function prototypes ----------------

void OBJ_init(void);

void OBJ_init_settings(void);
void OBJ_init_game_status(void);
void OBJ_init_canvas(void);

void OBJ_init_splash_screen(void);
void OBJ_init_background(void);

void OBJ_init_game_over_sprite(void);
void OBJ_init_press_any_key_sprite(void);
void OBJ_init_press_ok_sprite(void);

void OBJ_init_bird(void);


void OBJ_init_score_box_title(void);
void OBJ_init_score_text(void);
void OBJ_set_score_text_value(int16_t score);

void OBJ_spawn_obstacles(void);
void OBJ_init_obstacleup(void);
void OBJ_init_obstacledown(int x_cooridnate, int y_coordinate);



void OBJ_demo_spawn_target(void);


#endif /* INCLUDE_OBJECTS_H_ */
