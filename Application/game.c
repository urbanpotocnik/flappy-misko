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
#include "math_utils.h"          

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
stopwatch_handle_t stopwatch_obstacles;
obstacle_positions_t obstacle_positions;
obstacle_pair_t obstacle_pair1;
obstacle_pair_t obstacle_pair2;
obstacle_pair_t obstacle_pair3;

int obstacle_pair1_spawned = 0;
int obstacle_pair2_spawned = 0;
int obstacle_pair3_spawned = 0;
int obstacle_pair1_cleaned = 0;
int obstacle_pair2_cleaned = 0;
int obstacle_pair3_cleaned = 0;

int obstacle_pair1_scored = 0;
int obstacle_pair2_scored = 0;
int obstacle_pair3_scored = 0;

int moving_obstacles = 0; 
int time_mark = 0;
int obstacle_number = 0;



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

void GamePlay_UpdateChanges(void) {
    static stopwatch_handle_t update_stopwatch_misko;
    static uint8_t timers_initialized = 0;

    if (!timers_initialized) {
        TIMUT_stopwatch_set_time_mark(&update_stopwatch_misko);    
        timers_initialized = 1;
    }

	GFX_display_text_object(&score_text);
	GFX_display_text_object(&score_box_title);


    if (TIMUT_stopwatch_has_another_X_ms_passed(&update_stopwatch_misko, 10)) {
        GFX_update_moving_gfx_object_location(&misko);
        GFX_draw_one_gfx_object_on_background(&misko, &background);
		
		if(obstacle_pair1_spawned == 1) {
			GFX_update_obstacle_pair_location(&obstacle_pair1);
			GFX_draw_obstacle_pair_on_background(&obstacle_pair1, &background);
		}

		if(obstacle_pair2_spawned == 1) {
			GFX_update_obstacle_pair_location(&obstacle_pair2);
			GFX_draw_obstacle_pair_on_background(&obstacle_pair2, &background);
		}

		if(obstacle_pair3_spawned == 1) {
			GFX_update_obstacle_pair_location(&obstacle_pair3);
			GFX_draw_obstacle_pair_on_background(&obstacle_pair3, &background);
		}		
		
    }

}

uint8_t GamePlay() {
    static GAMEPLAY_states_t gameplay_state = GAMEPLAY_INIT;
    uint8_t exit_value = 0;

    switch (gameplay_state) {
    case GAMEPLAY_INIT:
        OBJ_init();
        OBJ_set_score_text_value(game_status.score);
        GFX_display_text_object(&score_box_title);
        GFX_display_text_object(&score_text);
        GFX_draw_one_gfx_object_on_background(&misko, &background);
        GFX_set_gfx_object_velocity(&misko, 0, 0);
        gameplay_state = GAMEPLAY_JUMP;
        exit_value = 0;
		game_status.score = 0;
        break;

    case GAMEPLAY_JUMP:
        KBD_flush();
        GFX_clear_gfx_object_on_background(&press_ok_sprite, &background);

        while (1) {
            KBD_scan();
            pressed_button = KBD_get_pressed_key();

            if (pressed_button == BTN_OK) {
                if (moving_obstacles == 0) {
                    moving_obstacles = 1;
					TIMUT_stopwatch_set_time_mark(&stopwatch_obstacles);
					obstacle_number = 1;
                }

                TIMUT_stopwatch_set_time_mark(&stopwatch_jump);
                GFX_set_gfx_object_velocity(&misko, 0, 2);
            }

            if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch_jump, 200)) {
                GFX_set_gfx_object_velocity(&misko, 0, -2);
            }


			if (TIMUT_stopwatch_has_another_X_ms_passed(&stopwatch_obstacles, 1500)) {

				if (obstacle_number == 1) {
					obstacle_positions = MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair1);
					GFX_init_obstacle_pair_location(&obstacle_pair1, 269, obstacle_positions.obstacle_top_y, obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair1, -1);
					obstacle_pair1_spawned = 1;
					obstacle_pair1_cleaned = 0;
					obstacle_number = 2;
					//printf("Obstacle 1 spawned\n");
				} else if (obstacle_number == 2) {
					obstacle_positions = MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair2);
					GFX_init_obstacle_pair_location(&obstacle_pair2, 269, obstacle_positions.obstacle_top_y, obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair2, -1);
					obstacle_pair2_spawned = 1;
					obstacle_pair2_cleaned = 0;
					obstacle_number = 3;
					//printf("Obstacle 2 spawned\n");
				} else if (obstacle_number == 3) {
					obstacle_positions = MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair3);
					GFX_init_obstacle_pair_location(&obstacle_pair3, 269, obstacle_positions.obstacle_top_y, obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair3, -1);
					obstacle_pair3_spawned = 1;
					obstacle_pair3_cleaned = 0;
					obstacle_number = 1;
					//printf("Obstacle 3 spawned\n");
				}
			}	


            

			// TO DO:
			// Popravi da se score tabela ne zabrisuje
			// Dodaj razne koordinate in tako v define
			// Naredi vse skup mogoce bolj modularno


            GamePlay_UpdateChanges();


			
            GFX_get_object_movement_area(&misko, &movement_area);
            if (movement_area.y_max == 239) {
                GFX_set_gfx_object_velocity(&misko, 0, 0);
                exit_value = 1;
                break;
            }


			GFX_get_obstacle_pair_movement_area(&obstacle_pair1, &movement_area);
			GFX_get_obstacle_pair_movement_area(&obstacle_pair2, &movement_area);
			GFX_get_obstacle_pair_movement_area(&obstacle_pair3, &movement_area);
			
			if (obstacle_pair1.bottom.location.x_min == 1 && obstacle_pair1_cleaned == 0) {
				obstacle_pair1_spawned = 0;
				obstacle_pair1_cleaned = 1;
				obstacle_pair1_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair1, &background);
            }

			if (obstacle_pair2.bottom.location.x_min == 1 && obstacle_pair2_cleaned == 0) {
            	obstacle_pair2_spawned = 0;
            	obstacle_pair2_cleaned = 1;
				obstacle_pair2_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair2, &background);
            }

			if (obstacle_pair3.bottom.location.x_min == 1 && obstacle_pair3_cleaned == 0) {
            	obstacle_pair3_spawned = 0;
            	obstacle_pair3_cleaned = 1;
				obstacle_pair3_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair3, &background);
            }


			// Naredi bolj modularno?
			//GFX_get_obstacle_pair_movement_area(&obstacle_pair1, &movement_area);
			//GFX_get_obstacle_pair_movement_area(&obstacle_pair2, &movement_area);
			//GFX_get_obstacle_pair_movement_area(&obstacle_pair3, &movement_area);
			if ((GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair1.top) && obstacle_pair1_spawned == 1) ||
			(GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair1.bottom)&& obstacle_pair1_spawned == 1) ||
			(GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair2.top) && obstacle_pair2_spawned == 1) ||
			(GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair2.bottom)&& obstacle_pair2_spawned == 1)  ||
			(GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair3.top) && obstacle_pair3_spawned == 1) ||
			(GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair3.bottom)&& obstacle_pair3_spawned == 1)) {

				GFX_set_gfx_object_velocity(&misko, 0, 0);
                exit_value = 1;
                break;
			}
			
			

			if (misko.location.x_center > obstacle_pair1.top.location.x_min && obstacle_pair1_spawned == 1 && !obstacle_pair1_scored) {
				if (misko.location.x_center > obstacle_pair1.top.location.x_center) {
					game_status.score += 1;
					OBJ_set_score_text_value(game_status.score);
					obstacle_pair1_scored = 1; 
				}	
			}

			if (misko.location.x_center > obstacle_pair2.top.location.x_min && obstacle_pair2_spawned == 1 && !obstacle_pair2_scored) {
				if (misko.location.x_center > obstacle_pair2.top.location.x_center) {
					game_status.score += 1;
					OBJ_set_score_text_value(game_status.score);
					obstacle_pair2_scored = 1; 
				}
			}

			if (misko.location.x_center > obstacle_pair3.top.location.x_min && obstacle_pair3_spawned == 1 && !obstacle_pair3_scored) {
				if (misko.location.x_center > obstacle_pair3.top.location.x_center) {
					game_status.score += 1;
					OBJ_set_score_text_value(game_status.score);
					obstacle_pair3_scored = 1; 
				}
			}			
			
			/*
			if (pressed_button == BTN_ESC) {
				exit_value = 1;
				break;
			}
			*/

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

		// tukaj je treba vse variable z game podavtomata treba dati na 0

		KBD_flush();
		HAL_Delay(2000);
		game_status.score = 0;
		exit_value = 1;
		//state = GAMEPLAY_JUMP;

		obstacle_pair1_spawned = 0;
		obstacle_pair2_spawned = 0;
		obstacle_pair3_spawned = 0;
		obstacle_pair1_cleaned = 0;
		obstacle_pair2_cleaned = 0;
		obstacle_pair3_cleaned = 0;

		obstacle_pair1_scored = 0;
		obstacle_pair2_scored = 0;
		obstacle_pair3_scored = 0;

		moving_obstacles = 0; 
		time_mark = 0;
		obstacle_number = 0;

		GFX_clear_obstacle_pair_on_background(&obstacle_pair1, &background);
		GFX_clear_obstacle_pair_on_background(&obstacle_pair2, &background);
		GFX_clear_obstacle_pair_on_background(&obstacle_pair3, &background);
		GFX_clear_gfx_object_on_background(&misko, &background);
		GFX_clear_gfx_object_on_background(&game_over_sprite, &background);


		/*
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
		*/


		//state = GAMEOVER_WAIT_FOR_ANY_KEY;
		//exit_value = 0;
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
