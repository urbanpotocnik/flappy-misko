#ifndef INCLUDE_OBJECTS_H_
#define INCLUDE_OBJECTS_H_

#include "stdint.h"		
#include <stdio.h>		
#include "images.h"			
#include "graphics.h"		
#include "math_utils.h"		


// -------------------- Public definitions --------------------

typedef struct
{
	velocity_t	abs_velocity_min;
	velocity_t	abs_velocity_max;
	uint32_t	game_play_update_period;		

} settings_t;

typedef struct
{
	int16_t	score;

} game_status_t;

typedef struct
{
	location_t	whole_area;
	location_t	above_ground;
	location_t	below_ground;

} canvas_t;





// ------ COMPOUND OBJECTS TYPE DEFINITIONS ---------

typedef struct
{
	graphic_object_t	gfx_object;
	uint16_t	distance_to_target_center;	

} crosshair_object_t;




// ------------- OBJECT ARE EXTERNAL GLOBAL VARIABLES  -----------

extern settings_t 		settings;
extern game_status_t	game_status;
extern canvas_t			canvas;

extern graphic_object_t	intro_sprite;
extern graphic_object_t	background;
extern graphic_object_t	misko2;
extern graphic_object_t	game_over_sprite;
extern graphic_object_t press_ok_sprite;
extern graphic_object_t small_sprite;
extern graphic_object_t big_sprite;
extern graphic_object_t misko;
extern graphic_object_t obstacle_top;
extern graphic_object_t obstacle_bottom;
extern graphic_object_t	start_game_sprite;
extern graphic_object_t	choose_theme_sprite;
extern graphic_object_t	high_scores_sprite;
extern graphic_object_t	finger_or_button_sprite;
extern graphic_object_t	loading_sprite;
extern graphic_object_t high_score_sprite_large;
extern graphic_object_t dark_theme_sprite;
extern graphic_object_t light_theme_sprite;
extern graphic_object_t press_any_key_sprite;
extern graphic_object_t play_with_sprite;
extern graphic_object_t buttons_sprite;
extern graphic_object_t touchscreen_sprite;

extern text_object_t	press_any_key_text;
extern text_object_t	play_with_text;
extern text_object_t	buttons_text;
extern text_object_t	touchscreen_text;
extern text_object_t	score_box_title;
extern text_object_t	score_text;
extern text_object_t	choose_theme_text;
extern text_object_t	high_scores_text;
extern text_object_t	touch_or_button_text;
extern text_object_t 	flappy_misko_text;
extern text_object_t 	loading_text;
extern text_object_t	text_selector;
extern text_object_t 	start_game_text;
extern text_object_t	pause_text1;
extern text_object_t	pause_text2;
extern text_object_t	text_selector;

extern text_object_t	high_scores_menu_text;
extern text_object_t	high_score1_text;
extern text_object_t	high_score2_text;
extern text_object_t	high_score3_text;
extern text_object_t	press_to_go_back_text;
extern text_object_t	high_score1_text_value;
extern text_object_t	high_score2_text_value;
extern text_object_t	high_score3_text_value;
extern text_object_t	dark_theme_text;
extern text_object_t	light_theme_text;
extern text_object_t	high_score_menu_text;
extern text_object_t	your_hs_text;
extern text_object_t	biggest_hs_text;
extern text_object_t	your_hs_value_text;
extern text_object_t	biggest_hs_value_text;
extern text_object_t	press_to_go_back_text2;
extern graphic_object_t small_sprite;

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
void OBJ_init_misko(void);
void OBJ_init_score_box_title(void);
void OBJ_init_score_text(void);
void OBJ_set_score_text_value(int16_t score);
void OBJ_init_obstacle_top(void);
void OBJ_init_obstacle_bottom(void);
void OBJ_demo_spawn_target(void);
void OBJ_init_obstacle_pair(obstacle_pair_t *obstacle_pair);
void OBJ_init_small_sprite(int x, int y);
void OBJ_init_start_game_text(int x_min, int y_min);
void OBJ_init_choose_theme_text(int x_min, int y_min);
void OBJ_init_high_scores_text(int x_min, int y_min);
void OBJ_init_null_text(int x_min, int y_min);
void OBJ_init_flappy_misko_text(int x_min, int y_min);
void OBJ_init_big_sprite(int x, int y);
void OBJ_init_small_sprite_object(graphic_object_t *sprite, int x, int y);
void OBJ_init_loading_text(int x_min, int y_min, const char* str);
void OBJ_init_text_small(int x_min, int y_min, const char* input_str, text_object_t *txt_object);
void OBJ_init_high_score_sprite_large(int x, int y);
void OBJ_init_text_big(int x_min, int y_min, const char* input_str, text_object_t *txt_object);
void OBJ_init_text_tiny(int x_min, int y_min, const char* input_str, text_object_t *txt_object);

#endif /* INCLUDE_OBJECTS_H_ */
