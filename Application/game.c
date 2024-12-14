// ----------- Include other modules (for private) -------------
#include <stdio.h>
#include <stdlib.h> 
#include "LED.h"                
#include "kbd.h"               
#include "SCI.h"                
#include "joystick.h"           
#include "lcd.h"               
#include "timing_utils.h"       
#include "game.h"               
#include "objects.h"            
#include "graphics.h"           

// ----- Definicija možnih stanj avtomatov --------
typedef enum GAME_states {
	GAME_INTRO_STATE, 
    GAME_PLAY_STATE, 
    GAME_OVER_STATE, 
    GAME_NUM_OF_STATES
} GAME_states_t;

typedef enum INTRO_states {
	INTRO_INIT, 
    INTRO_PRESS_ANY_KEY, 
    INTRO_WAIT_FOR_ANY_KEY, 
    INTRO_NUM_OF_STATES
} INTRO_states_t;

typedef enum GAMEPLAY_states {
	GAMEPLAY_INIT, 
    GAMEPLAY_JUMP
} GAMEPLAY_states_t;

typedef enum GAMEOVER_states {
	GAMEOVER_SCREEN, 
    GAMEOVER_WAIT_FOR_ANY_KEY, 
    GAMEOVER_NUM_OF_STATES
} GAMEOVER_states_t;

stopwatch_handle_t stopwatch;
location_t movement_area;
location_t location = { 
    .x_min = 0, 
    .x_max = 0, 
    .y_min = 350, 
    .y_max = 370 
};

buttons_enum_t key;
stopwatch_handle_t stopwatch_leds;
stopwatch_handle_t stopwatch_gameover;
buttons_enum_t pressed_button = BTN_NONE;
stopwatch_handle_t stopwatch_jump;
stopwatch_handle_t stopwatch_obstacle;

// ------------- Public function implementations --------------
void Game() {
	static GAME_states_t state = GAME_INTRO_STATE;
	uint8_t exit_value = 0;

	switch (state) {
	case GAME_INTRO_STATE:
		exit_value = Intro();
		if (exit_value != 0)
			state = GAME_PLAY_STATE;
		break;

	case GAME_PLAY_STATE:
		exit_value = GamePlay();
		if (exit_value != 0)
			state = GAME_OVER_STATE;
		break;

	case GAME_OVER_STATE:
		exit_value = GameOver();
		if (exit_value != 0)
			state = GAME_INTRO_STATE;
		break;

	default:
		printf("Game(): Error - undefined state (%d)", state);
		HAL_Delay(5000);
		state = GAME_INTRO_STATE;
		exit_value = 0;
		break;
	}
}

uint8_t Intro() {
	static INTRO_states_t state = INTRO_INIT;
	uint8_t exit_value = 0;

	switch (state) {
	case INTRO_INIT:
		OBJ_init();
		GFX_draw_gfx_object(&background);
		GFX_draw_one_gfx_object_on_background(&intro_sprite, &background);
		TIMUT_stopwatch_set_time_mark(&stopwatch_leds);
		int a = 0x01;

		while (1) {
			LEDs_write(a);
			HAL_Delay(100);
			a = a << 1;
			if (a == 0x80) {
				a = 0x01;
			}
			if (TIMUT_get_stopwatch_elapsed_time(&stopwatch_leds) > INTRO_DELAY_BEFORE_KBD_ACTIVE) {
				LEDs_off(0xFF);
				break;
			}
		}

		KBD_flush();
		state = INTRO_PRESS_ANY_KEY;
		exit_value = 0;
		break;

	case INTRO_PRESS_ANY_KEY:
		GFX_clear_gfx_object_on_background(&intro_sprite, &background);
		GFX_draw_one_gfx_object_on_background(&press_ok_sprite, &background);
		state = INTRO_WAIT_FOR_ANY_KEY;
		exit_value = 0;
		break;

	case INTRO_WAIT_FOR_ANY_KEY:
		key = KBD_get_pressed_key();
		if (key != BTN_NONE) {
			state = INTRO_INIT;
			exit_value = 1;
		}
		break;

	default:
		printf("Intro(): Error - unknown state (%d)", state);
		HAL_Delay(5000);
		state = INTRO_INIT;
		exit_value = 0;
		break;
	}

	return exit_value;
}

uint8_t GamePlay() {
	static GAMEPLAY_states_t gameplay_state = GAMEPLAY_INIT;
	uint8_t exit_value = 0;
	
	void GamePlay_UpdateChanges(void) {
		static stopwatch_handle_t update_stopwatch;

		// IMPORTANT: tweak the refresh time, best is 20ms
		if (TIMUT_stopwatch_has_another_X_ms_passed(&update_stopwatch, settings.game_play_update_period)) {
			
			
			OBJ_set_score_text_value(game_status.score);
			GFX_display_text_object(&score_text);

			//GFX_draw_gfx_object(&background);
			// To prepreci da se vlece crta za objektom, vendar pa zelo upocasni renderiranje igrice

			
			GFX_clear_gfx_object_on_background(&bird, &background);
			GFX_update_moving_gfx_object_location(&bird);
			GFX_draw_one_gfx_object_on_background(&bird, &background);

			GFX_clear_gfx_object_on_background(&obstacleup, &background);
			GFX_update_moving_gfx_object_location(&obstacleup);
			GFX_draw_one_gfx_object_on_background(&obstacleup, &background);
		}
	}

	switch (gameplay_state) {
	case GAMEPLAY_INIT:
		OBJ_init();
		OBJ_set_score_text_value(game_status.score);
		GFX_display_text_object(&score_box_title);
		GFX_display_text_object(&score_text);
		GFX_draw_one_gfx_object_on_background(&bird, &background);
        GFX_set_gfx_object_velocity(&bird, 0, 0);
		//
		gameplay_state = GAMEPLAY_JUMP;
		exit_value = 0;
		break;

	case GAMEPLAY_JUMP:
		KBD_flush();
		GFX_clear_gfx_object_on_background(&press_ok_sprite, &background);
		int moving_obstacles = 0; 
		int obstacle_spawned = 0;  

		while (1) {
			KBD_scan();
			pressed_button = KBD_get_pressed_key();

			if (pressed_button == BTN_OK) {
				if (moving_obstacles == 0) {
					moving_obstacles = 1;
					TIMUT_stopwatch_set_time_mark(&stopwatch_obstacle);
				}
				TIMUT_stopwatch_set_time_mark(&stopwatch_jump);
				GFX_set_gfx_object_velocity(&bird, 0, 4);
			}

			if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch_jump, 500)) {
				GFX_set_gfx_object_velocity(&bird, 0, -4);
			}

			if (moving_obstacles == 1 && TIMUT_stopwatch_has_X_ms_passed(&stopwatch_obstacle, 4000) && obstacle_spawned == 0) {
				OBJ_init_obstacleup();
				GFX_set_gfx_object_velocity(&obstacleup, -2, 0);
				GFX_init_gfx_object_location( &obstacleup, 220, 220);
				obstacle_spawned = 1;
			}

			// zaslon je 320x240

			// konec igre, pade na tla
			GFX_get_object_movement_area(&bird, &movement_area);
			if (movement_area.y_max == 239) {
				GFX_set_gfx_object_velocity(&bird, 0, 0);
				exit_value = 1;
				break;
			}

			// updejti gameplay update changes da dodas se refresh za stolpe
			GamePlay_UpdateChanges();

			//OBJ_spawn_obstacles(void);  // funkcija ki spawna ovire ------------- je še za pregledati
			//HAL_Delay(neki);
			// tukaj bo moral priti delay med generacijami ovir

			//------------- priševanje scora
			//uint8_t GFX_are_locations_overlapping(location_t *location_A, location_t *location_B);
			//uint8_t GFX_is_point_inside_location(location_t *location, int16_t x, int16_t y);
			// game_status.score += target.points;
			// to bom realiziral z enostavnim if stavkom

			//------ če se zadadne se while loop prekine in program gre v naslednje stanje avtomata
			//if ((GFX_are_gfx_objects_overlapping(&bird, &obstacleup) == 1) || ((GFX_are_gfx_objects_overlapping(&bird, &obstacledown) == 1)  {
			//	break;
			//}

		}

		break;

	default:
		printf("GamePlay(): Error - unknown state (%d)", gameplay_state);
		HAL_Delay(5000);
		gameplay_state = GAMEPLAY_INIT;
		exit_value = 0;
		break;
	}

	return exit_value;
}

uint8_t GameOver() {
	static GAMEOVER_states_t state = GAMEOVER_SCREEN;
	uint8_t exit_value = 0;

	switch (state) {
	case GAMEOVER_SCREEN:
		GFX_draw_one_gfx_object_on_background(&game_over_sprite, &background);
		KBD_flush();
		int a = 0x01;
		TIMUT_stopwatch_set_time_mark(&stopwatch_gameover);
		while (1) {
			LEDs_write(a);
			HAL_Delay(100);
			a = a << 1;
			if (a == 0x80) {
				a = 0x01;
			}
			if (TIMUT_get_stopwatch_elapsed_time(&stopwatch_gameover) > GAMEOVER_DELAY_BEFORE_KBD_ACTIVE) {
				LEDs_off(0xFF);
				break;
			}
		}
		state = GAMEOVER_WAIT_FOR_ANY_KEY;
		exit_value = 0;
		break;

	case GAMEOVER_WAIT_FOR_ANY_KEY:
		key = KBD_get_pressed_key();
		if ((TIMUT_get_stopwatch_elapsed_time(&stopwatch_gameover) > 10000) || (key != BTN_NONE)) {
			state = GAMEOVER_SCREEN;
			exit_value = 1;
		}
		break;

	default:
		printf("GameOver(): Error - unknown state (%d)", state);
		HAL_Delay(5000);
		state = GAMEOVER_SCREEN;
		exit_value = 0;
		break;
	}

	return exit_value;
}
