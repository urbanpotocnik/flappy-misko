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
#include "flash.h"         

// ----- Definicija možnih stanj avtomatov --------

typedef enum GAME_states {
	GAME_INTRO_STATE, GAME_PLAY_STATE, GAME_OVER_STATE, GAME_NUM_OF_STATES
} GAME_states_t;

typedef enum INTRO_states {
	INTRO_INIT,
	INTRO_PRESS_ANY_KEY,
	INTRO_MAIN_MENU,
	INTRO_START_GAME,
	INTRO_CHOOSE_THEME,
	INTRO_HIGH_SCORES,
	INTRO_PLAY_WITH,
	INTRO_WAIT_FOR_ANY_KEY,
	INTRO_NUM_OF_STATES
} INTRO_states_t;

typedef enum GAMEPLAY_states {
	GAMEPLAY_INIT, GAMEPLAY_JUMP
} GAMEPLAY_states_t;

typedef enum GAMEOVER_states {
	GAMEOVER_SCREEN, GAMEOVER_WAIT_FOR_ANY_KEY, GAMEOVER_NUM_OF_STATES
} GAMEOVER_states_t;

typedef enum MENU_ITEMS {
	MENU_START_GAME,
	MENU_CHOOSE_THEME,
	MENU_HIGH_SCORES,
	MENU_PLAY_WITH,
	MENU_ITEMS_COUNT
} MENU_ITEMS_t;

typedef enum INPUT_MODE {
    INPUT_BUTTONS,
    INPUT_TOUCHSCREEN
} INPUT_MODE_t;

static INPUT_MODE_t current_input_mode = INPUT_BUTTONS;
static MENU_ITEMS_t selected_menu_item = MENU_START_GAME;

stopwatch_handle_t stopwatch;
location_t movement_area;

location_t location = { .x_min = 0, .x_max = 0, .y_min = 350, .y_max = 370 };

buttons_enum_t key;
stopwatch_handle_t stopwatch_leds;
stopwatch_handle_t stopwatch_gameover;
buttons_enum_t pressed_button = BTN_NONE;
stopwatch_handle_t stopwatch_jump;
stopwatch_handle_t update_stopwatch_intro;
stopwatch_handle_t update_stopwatch_intro_exit;
stopwatch_handle_t stopwatch_obstacles;
stopwatch_handle_t stopwatch_touchscreen;
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
uint8_t menu_initialized = 0;
int previous_selected_item = -1;
uint8_t play_style_menu_initialized = 0;
int selected_play_style = 0;
int previous_selected_play_style = -1;
static bool touch_init = 0;
static bool press_enable = 1;

static INTRO_states_t intro_state = INTRO_INIT;

static stopwatch_handle_t touch_polling_stopwatch;
static int touch_initialized = 0;

// ------------- Public function implementations --------------
void Game() {
    static GAME_states_t state = GAME_INTRO_STATE;
    uint8_t exit_value = 0;

    switch (state) {
    case GAME_INTRO_STATE:
        exit_value = Intro();
        if (exit_value != 0) {
            state = GAME_PLAY_STATE;
        }
        break;

    case GAME_PLAY_STATE:
        exit_value = GamePlay();
        if (exit_value != 0) {
            state = GAME_OVER_STATE;
        }
        break;

    case GAME_OVER_STATE:
        exit_value = GameOver();
        if (exit_value == 1) {  
            state = GAME_INTRO_STATE;
            intro_state = INTRO_MAIN_MENU;  
            menu_initialized = 0;  
            previous_selected_item = -1;
        }
        else if (exit_value == 2) {  
            state = GAME_INTRO_STATE;
            intro_state = INTRO_PRESS_ANY_KEY;  
        }
        break;

    default:
        printf("Game(): Error - undefined state (%d)", state);
        HAL_Delay(5000);
        state = GAME_INTRO_STATE;
        break;
    }
}

uint8_t Intro() {
	uint8_t exit_value = 0;
	int a = 0x01;

	switch (intro_state) {
	case INTRO_INIT:
		OBJ_init();
		OBJ_init_big_sprite(20, 20);



	    GFX_draw_gfx_object(&background);
		GFX_draw_one_gfx_object_on_background(&big_sprite, &background);
		OBJ_init_flappy_misko_text(59, 32);
		GFX_display_text_object(&flappy_misko_text);

		GFX_set_gfx_object_location(&misko, 0, 120);
		GFX_draw_one_gfx_object_on_background(&misko, &background);
		GFX_set_gfx_object_velocity(&misko, 2, 0);

		TIMUT_stopwatch_set_time_mark(&stopwatch_leds);
		TIMUT_stopwatch_set_time_mark(&update_stopwatch_intro);
		TIMUT_stopwatch_set_time_mark(&update_stopwatch_intro_exit);

		OBJ_init_small_sprite_object(&loading_sprite, 50, 190);
		GFX_draw_one_gfx_object_on_background(&loading_sprite, &background);

		int loading_state = 0;
		int loading_direction = 1;
		stopwatch_handle_t loading_text_stopwatch;
		TIMUT_stopwatch_set_time_mark(&loading_text_stopwatch);
		OBJ_init_loading_text(96, 196, "LOADING");
		GFX_display_text_object(&flappy_misko_text);

		while (1) {
			if (TIMUT_stopwatch_has_X_ms_passed(&update_stopwatch_intro_exit, 4000)) {
				break;
			}

			if (TIMUT_stopwatch_has_another_X_ms_passed(&update_stopwatch_intro, 7)) {
				GFX_update_moving_gfx_object_location(&misko);
				GFX_draw_one_gfx_object_on_background(&misko, &background);
			}

			if (TIMUT_stopwatch_has_another_X_ms_passed(&loading_text_stopwatch, 100)) {
				switch (loading_state) {
				case 0:
					GFX_draw_one_gfx_object_on_background(&loading_sprite, &background);
					OBJ_init_loading_text(96, 196, "LOADING");
					GFX_display_text_object(&loading_text);
					break;
				case 1:
					OBJ_init_loading_text(96, 196, "LOADING.");
					GFX_display_text_object(&loading_text);
					break;
				case 2:
					OBJ_init_loading_text(96, 196, "LOADING..");
					GFX_display_text_object(&loading_text);
					break;
				case 3:
					OBJ_init_loading_text(96, 196, "LOADING...");
					GFX_display_text_object(&loading_text);
					break;
				}
				GFX_display_text_object(&flappy_misko_text);

				if (loading_direction == 1) {
					loading_state++;
					if (loading_state == 3) {
						loading_direction = -1;
					}
				} else if (loading_direction == -1) {
					loading_state--;
					if (loading_state == 0) {
						loading_direction = 1;
					}
				}
			}

			LEDs_write(a);
			if (TIMUT_stopwatch_has_another_X_ms_passed(&stopwatch_leds, 100)) {
				a = a << 1;

				if (a > 0x80) {
					a = 0x01;
				}
			}
		}

		LEDs_off(0xFF);
		GFX_clear_gfx_object_on_background(&misko, &background);
		GFX_clear_gfx_object_on_background(&big_sprite, &background);
		KBD_flush();
		intro_state = INTRO_MAIN_MENU;
		exit_value = 0;
		break;

	case INTRO_MAIN_MENU: {

		if (!menu_initialized) {
			GFX_draw_gfx_object(&background);
			OBJ_init_big_sprite(20, 10);
			GFX_draw_one_gfx_object_on_background(&big_sprite, &background);
			OBJ_init_flappy_misko_text(30, 22);
			GFX_display_text_object(&flappy_misko_text);

			GFX_set_gfx_object_location(&misko2, 240, 22);
			GFX_draw_one_gfx_object_on_background(&misko2, &background);

			OBJ_init_small_sprite_object(&start_game_sprite, 50, 70);
			OBJ_init_small_sprite_object(&choose_theme_sprite, 50, 110);
			OBJ_init_small_sprite_object(&high_scores_sprite, 50, 150);
			OBJ_init_small_sprite_object(&finger_or_button_sprite, 50, 190);

			GFX_draw_one_gfx_object_on_background(&start_game_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&choose_theme_sprite,&background);
			GFX_draw_one_gfx_object_on_background(&high_scores_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&finger_or_button_sprite, &background);

			OBJ_init_text_small(60, 80, "START GAME", &start_game_text);
			OBJ_init_text_small(60, 120, "CHOOSE THEME", &choose_theme_text);
			OBJ_init_text_small(60, 160, "HIGH SCORES", &high_scores_text);
			OBJ_init_text_small(60, 200, "PLAY WITH", &touch_or_button_text);

			GFX_display_text_object(&start_game_text);
			GFX_display_text_object(&choose_theme_text);
			GFX_display_text_object(&high_scores_text);
			GFX_display_text_object(&touch_or_button_text);

			menu_initialized = 1;
		}

		if (previous_selected_item != selected_menu_item) {
			GFX_draw_one_gfx_object_on_background(&start_game_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&choose_theme_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&high_scores_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&finger_or_button_sprite, &background);

			GFX_display_text_object(&start_game_text);
			GFX_display_text_object(&choose_theme_text);
			GFX_display_text_object(&high_scores_text);
			GFX_display_text_object(&touch_or_button_text);

			switch (selected_menu_item) {
			case 0:
				OBJ_init_text_small(230, 80, "->", &text_selector);
				break;
			case 1:
				OBJ_init_text_small(230, 120, "->", &text_selector);
				break;
			case 2:
				OBJ_init_text_small(230, 160, "->", &text_selector);
				break;
			case 3:
				OBJ_init_text_small(230, 200, "->", &text_selector);
				break;
			}
			GFX_display_text_object(&text_selector);
			previous_selected_item = selected_menu_item;
		}

		key = KBD_get_pressed_key();
		if (key == BTN_UP) {
			if (selected_menu_item == 0) {
				selected_menu_item = MENU_ITEMS_COUNT - 1;
			} else {
				selected_menu_item--;
			}
		} else if (key == BTN_DOWN) {
			if (selected_menu_item == MENU_ITEMS_COUNT - 1) {
				selected_menu_item = 0;
			} else {
				selected_menu_item++;
			}
		} else if (key == BTN_OK) {
			switch (selected_menu_item) {
			case 0: // Start Game
				menu_initialized = 0;
				intro_state = INTRO_PRESS_ANY_KEY;
				exit_value = 0;  
				break;
			case 1: // Choose Theme
				intro_state = INTRO_CHOOSE_THEME;
				menu_initialized = 0;
				break;

			case 2: // High Scores
				intro_state = INTRO_HIGH_SCORES;
				menu_initialized = 0;
				break;

			case 3: // Play With
				intro_state = INTRO_PLAY_WITH;
				menu_initialized = 0;
				break;
			}
		}
		break;
	}

	case INTRO_START_GAME:
		//state = INTRO_WAIT_FOR_ANY_KEY;
		//exit_value = 0;
		break;

	case INTRO_CHOOSE_THEME: {
		static uint8_t theme_menu_initialized = 0;
		static int selected_theme = 0;
		static int previous_selected_theme = -1;

		if (!theme_menu_initialized) {
			GFX_draw_gfx_object(&background);
			OBJ_init_big_sprite(20, 10);
			GFX_draw_one_gfx_object_on_background(&big_sprite, &background);
			OBJ_init_flappy_misko_text(30, 22);
			GFX_display_text_object(&flappy_misko_text);

			GFX_set_gfx_object_location(&misko, 240, 22);
			GFX_draw_one_gfx_object_on_background(&misko, &background);

			OBJ_init_small_sprite_object(&choose_theme_sprite, 50, 70);
			OBJ_init_small_sprite_object(&dark_theme_sprite, 50, 110);
			OBJ_init_small_sprite_object(&light_theme_sprite, 50, 150);

			GFX_draw_one_gfx_object_on_background(&choose_theme_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&dark_theme_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&light_theme_sprite, &background);

			OBJ_init_text_small(60, 80, "CHOOSE THEME:", &choose_theme_text);
			OBJ_init_text_small(60, 120, "DARK", &dark_theme_text);
			OBJ_init_text_small(60, 160, "LIGHT", &light_theme_text);

			GFX_display_text_object(&choose_theme_text);
			GFX_display_text_object(&dark_theme_text);
			GFX_display_text_object(&light_theme_text);

			OBJ_init_text_small(230, 120, "->", &text_selector);
			GFX_display_text_object(&text_selector);
			previous_selected_theme = selected_theme;

			theme_menu_initialized = 1;
		}

		if (previous_selected_theme != selected_theme) {
			GFX_draw_one_gfx_object_on_background(&dark_theme_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&light_theme_sprite, &background);

			GFX_display_text_object(&dark_theme_text);
			GFX_display_text_object(&light_theme_text);

			switch (selected_theme) {
			case 0:
				OBJ_init_text_small(230, 120, "->", &text_selector);
				break;
			case 1:
				OBJ_init_text_small(230, 160, "->", &text_selector);
				break;
			}
			GFX_display_text_object(&text_selector);
			previous_selected_theme = selected_theme;
		}

		key = KBD_get_pressed_key();
		if (key == BTN_UP || key == BTN_DOWN) {
			selected_theme = !selected_theme;
		} else if (key == BTN_OK || key == BTN_ESC) {

			if (key == BTN_OK) {
				if (selected_theme == 0) {
                    // Apply dark theme                                 
				} else {
                    // Apply light theme
				}
			}
			theme_menu_initialized = 0;
			intro_state = INTRO_MAIN_MENU;
			menu_initialized = 0;
			previous_selected_item = -1;
		}
		break;
	}

	case INTRO_HIGH_SCORES: {
		static uint8_t high_scores_initialized = 0;

		if (!high_scores_initialized) {
			GFX_draw_gfx_object(&background);
			OBJ_init_high_score_sprite_large(30, 30);
			GFX_draw_one_gfx_object_on_background(&high_score_sprite_large,&background);

			uint16_t *high_scores = Get_High_Scores();
			char score_text[20];

			OBJ_init_text_big(67, 40, "HIGH SCORES:", &high_scores_menu_text);
			GFX_display_text_object(&high_scores_menu_text);

			sprintf(score_text, "%d", high_scores[0]);
			OBJ_init_text_big(50, 75, "1.", &high_score1_text);
			OBJ_init_text_big(95, 75, score_text, &high_score1_text_value);
			GFX_display_text_object(&high_score1_text);
			GFX_display_text_object(&high_score1_text_value);

			sprintf(score_text, "%d", high_scores[1]);
			OBJ_init_text_big(50, 110, "2.", &high_score2_text);
			OBJ_init_text_big(95, 110, score_text, &high_score2_text_value);
			GFX_display_text_object(&high_score2_text);
			GFX_display_text_object(&high_score2_text_value);

			sprintf(score_text, "%d", high_scores[2]);
			OBJ_init_text_big(50, 145, "3.", &high_score3_text);
			OBJ_init_text_big(95, 145, score_text, &high_score3_text_value);
			GFX_display_text_object(&high_score3_text);
			GFX_display_text_object(&high_score3_text_value);

			OBJ_init_text_tiny(50, 185, "PRESS ANY KEY TO GO BACK", &press_to_go_back_text);
			GFX_display_text_object(&press_to_go_back_text);

			high_scores_initialized = 1;
		}

		key = KBD_get_pressed_key();
		if (key != BTN_NONE) {
			intro_state = INTRO_MAIN_MENU;
			high_scores_initialized = 0;
			menu_initialized = 0;
			previous_selected_item = -1;
			GFX_draw_gfx_object(&background);
		}
		break;
	}

	case INTRO_PLAY_WITH: {
		if (!play_style_menu_initialized) {
			GFX_draw_gfx_object(&background);
			OBJ_init_big_sprite(20, 10);
			GFX_draw_one_gfx_object_on_background(&big_sprite, &background);
			OBJ_init_flappy_misko_text(30, 22);
			GFX_display_text_object(&flappy_misko_text);

			GFX_set_gfx_object_location(&misko, 240, 22);
			GFX_draw_one_gfx_object_on_background(&misko, &background);

			OBJ_init_small_sprite_object(&play_with_sprite, 50, 70);
			OBJ_init_small_sprite_object(&buttons_sprite, 50, 110);
			OBJ_init_small_sprite_object(&touchscreen_sprite, 50, 150);

			GFX_draw_one_gfx_object_on_background(&play_with_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&buttons_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&touchscreen_sprite, &background);

			OBJ_init_text_small(60, 80, "PLAY WITH:", &choose_theme_text);
			OBJ_init_text_small(60, 120, "BUTTONS", &dark_theme_text);
			OBJ_init_text_small(60, 160, "TOUCHSCREEN", &light_theme_text);

			GFX_display_text_object(&choose_theme_text);
			GFX_display_text_object(&dark_theme_text);
			GFX_display_text_object(&light_theme_text);

			OBJ_init_text_small(230, 120, "->", &text_selector);
			GFX_display_text_object(&text_selector);
			previous_selected_play_style = selected_play_style;

			play_style_menu_initialized = 1;
		}

		if (previous_selected_play_style != selected_play_style) {
			GFX_draw_one_gfx_object_on_background(&dark_theme_sprite, &background);
			GFX_draw_one_gfx_object_on_background(&light_theme_sprite, &background);

			GFX_display_text_object(&dark_theme_text);
			GFX_display_text_object(&light_theme_text);

			switch (selected_play_style) {
			case 0:
				OBJ_init_text_small(230, 120, "->", &text_selector);
				break;
			case 1:
				OBJ_init_text_small(230, 160, "->", &text_selector);
				break;
			}
			GFX_display_text_object(&text_selector);
			previous_selected_play_style = selected_play_style;
		}

		key = KBD_get_pressed_key();
		if (key == BTN_UP || key == BTN_DOWN) {
			selected_play_style = !selected_play_style;
		} else if (key == BTN_OK || key == BTN_ESC) {
			if (key == BTN_OK) {
				if (selected_play_style == 0) {
					current_input_mode = INPUT_BUTTONS;
				} else {
					current_input_mode = INPUT_TOUCHSCREEN;
				}
			}
			play_style_menu_initialized = 0;
			intro_state = INTRO_MAIN_MENU;
			menu_initialized = 0;
			previous_selected_item = -1;
		}
		break;
	}

	case INTRO_PRESS_ANY_KEY:
		GFX_draw_gfx_object(&background);
		GFX_clear_gfx_object_on_background(&misko, &background);
		GFX_clear_gfx_object_on_background(&big_sprite, &background);

		OBJ_init_small_sprite_object(&press_any_key_sprite, 50, 180);
		GFX_draw_one_gfx_object_on_background(&press_any_key_sprite, &background);
		
		OBJ_init_text_tiny(61, 190, "PRESS ANY KEY TO START", &press_any_key_text);
		GFX_display_text_object(&press_any_key_text);
		
		touch_init = 0;
		press_enable = 1;
		
		intro_state = INTRO_WAIT_FOR_ANY_KEY;
		exit_value = 0;
		break;

	case INTRO_WAIT_FOR_ANY_KEY:
		key = KBD_get_pressed_key();
		static bool touch_init = 0;  
		static bool press_enable = 1;
		static stopwatch_handle_t touch_polling_stopwatch;
		static stopwatch_handle_t touch_debounce_stopwatch;
		
		if (current_input_mode == INPUT_TOUCHSCREEN) {
			if (!touch_init) {
				TIMUT_stopwatch_set_time_mark(&touch_polling_stopwatch);
				TIMUT_stopwatch_set_time_mark(&touch_debounce_stopwatch);
				touch_init = 1;
			}

			if (TIMUT_stopwatch_has_another_X_ms_passed(&touch_polling_stopwatch, 60)) {
				int x, y; 
				XPT2046_touch_get_coordinates(&x, &y);
				
				if (y < 240 && press_enable == 1) {
					key = BTN_OK;
					press_enable = 0;
					TIMUT_stopwatch_set_time_mark(&touch_debounce_stopwatch);
				}
			}

			if (TIMUT_stopwatch_has_X_ms_passed(&touch_debounce_stopwatch, 60)) {
				press_enable = 1;
			}
		}
		
		if (key != BTN_NONE) {
			intro_state = INTRO_INIT;
			GFX_draw_gfx_object(&background);
			exit_value = 1;
			touch_init = 0;  
			press_enable = 1;  // Reset for next time
		}
    	break;

	default:
		printf("Intro(): Error - unknown state (%d)", intro_state);
		HAL_Delay(5000);
		intro_state = INTRO_INIT;
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

		if (obstacle_pair1_spawned == 1) {
			GFX_update_obstacle_pair_location(&obstacle_pair1);
			GFX_draw_obstacle_pair_on_background(&obstacle_pair1, &background);
		}

		if (obstacle_pair2_spawned == 1) {
			GFX_update_obstacle_pair_location(&obstacle_pair2);
			GFX_draw_obstacle_pair_on_background(&obstacle_pair2, &background);
		}

		if (obstacle_pair3_spawned == 1) {
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
    game_status.score = 0;
    OBJ_set_score_text_value(game_status.score);
    GFX_display_text_object(&score_box_title);
    GFX_display_text_object(&score_text);
    
    moving_obstacles = 0;
    time_mark = 0;
    obstacle_number = 0;
    obstacle_pair1_spawned = 0;
    obstacle_pair2_spawned = 0;
    obstacle_pair3_spawned = 0;
    obstacle_pair1_cleaned = 0;
    obstacle_pair2_cleaned = 0;
    obstacle_pair3_cleaned = 0;
    obstacle_pair1_scored = 0;
    obstacle_pair2_scored = 0;
    obstacle_pair3_scored = 0;

	bool press_enable = 1;
    
    GFX_clear_gfx_object_on_background(&misko, &background);
    GFX_set_gfx_object_location(&misko, 80, 120);  
    GFX_set_gfx_object_velocity(&misko, 0, 0);
    GFX_draw_one_gfx_object_on_background(&misko, &background);
    
    gameplay_state = GAMEPLAY_JUMP;
    exit_value = 0;
    break;

	case GAMEPLAY_JUMP:
		KBD_flush();
		GFX_clear_gfx_object_on_background(&press_ok_sprite, &background);

		if (!touch_initialized) {
			TIMUT_stopwatch_set_time_mark(&touch_polling_stopwatch);
			touch_initialized = 1;
		}

		while (1) {
			KBD_scan();
			pressed_button = KBD_get_pressed_key();

			if (TIMUT_stopwatch_has_another_X_ms_passed(&touch_polling_stopwatch, 60)) {
				if (current_input_mode == INPUT_TOUCHSCREEN) {  
					int x, y; 
					XPT2046_touch_get_coordinates(&x, &y);
					
					if (y < 240 && press_enable == 1) {
						pressed_button = BTN_OK;
						press_enable = 0;
						TIMUT_stopwatch_set_time_mark(&stopwatch_touchscreen);
					}
				}
			}

			if (TIMUT_stopwatch_has_X_ms_passed(&stopwatch_touchscreen, 60)) {
				press_enable = 1;
			}

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

			if (TIMUT_stopwatch_has_another_X_ms_passed(&stopwatch_obstacles,
					1500)) {

				if (obstacle_number == 1) {
					obstacle_positions =
							MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair1);
					GFX_init_obstacle_pair_location(&obstacle_pair1, 269,
							obstacle_positions.obstacle_top_y,
							obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair1, -1);
					obstacle_pair1_spawned = 1;
					obstacle_pair1_cleaned = 0;
					obstacle_number = 2;
					//printf("Obstacle 1 spawned\n");
				} else if (obstacle_number == 2) {
					obstacle_positions =
							MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair2);
					GFX_init_obstacle_pair_location(&obstacle_pair2, 269,
							obstacle_positions.obstacle_top_y,
							obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair2, -1);
					obstacle_pair2_spawned = 1;
					obstacle_pair2_cleaned = 0;
					obstacle_number = 3;
					//printf("Obstacle 2 spawned\n");
				} else if (obstacle_number == 3) {
					obstacle_positions =
							MATH_randomise_distance_between_obstacles();
					OBJ_init_obstacle_pair(&obstacle_pair3);
					GFX_init_obstacle_pair_location(&obstacle_pair3, 269,
							obstacle_positions.obstacle_top_y,
							obstacle_positions.obstacle_bottom_y);
					GFX_set_obstacle_pair_x_axis_velocity(&obstacle_pair3, -1);
					obstacle_pair3_spawned = 1;
					obstacle_pair3_cleaned = 0;
					obstacle_number = 1;
					//printf("Obstacle 3 spawned\n");
				}
			}

			GamePlay_UpdateChanges();

			GFX_get_object_movement_area(&misko, &movement_area);
			if (movement_area.y_max == 239) {
				GFX_set_gfx_object_velocity(&misko, 0, 0);
				exit_value = 1;
				break;
			}

			GFX_get_obstacle_pair_movement_area(&obstacle_pair1,
					&movement_area);
			GFX_get_obstacle_pair_movement_area(&obstacle_pair2,
					&movement_area);
			GFX_get_obstacle_pair_movement_area(&obstacle_pair3,
					&movement_area);

			if (obstacle_pair1.bottom.location.x_min == 1
					&& obstacle_pair1_cleaned == 0) {
				obstacle_pair1_spawned = 0;
				obstacle_pair1_cleaned = 1;
				obstacle_pair1_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair1,
						&background);
			}

			if (obstacle_pair2.bottom.location.x_min == 1
					&& obstacle_pair2_cleaned == 0) {
				obstacle_pair2_spawned = 0;
				obstacle_pair2_cleaned = 1;
				obstacle_pair2_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair2,
						&background);
			}

			if (obstacle_pair3.bottom.location.x_min == 1
					&& obstacle_pair3_cleaned == 0) {
				obstacle_pair3_spawned = 0;
				obstacle_pair3_cleaned = 1;
				obstacle_pair3_scored = 0;
				GFX_clear_obstacle_pair_on_background(&obstacle_pair3,
						&background);
			}

			if ((GFX_are_gfx_objects_overlapping(&misko, &obstacle_pair1.top)
					&& obstacle_pair1_spawned == 1)
					|| (GFX_are_gfx_objects_overlapping(&misko,
							&obstacle_pair1.bottom)
							&& obstacle_pair1_spawned == 1)
					|| (GFX_are_gfx_objects_overlapping(&misko,
							&obstacle_pair2.top) && obstacle_pair2_spawned == 1)
					|| (GFX_are_gfx_objects_overlapping(&misko,
							&obstacle_pair2.bottom)
							&& obstacle_pair2_spawned == 1)
					|| (GFX_are_gfx_objects_overlapping(&misko,
							&obstacle_pair3.top) && obstacle_pair3_spawned == 1)
					|| (GFX_are_gfx_objects_overlapping(&misko,
							&obstacle_pair3.bottom)
							&& obstacle_pair3_spawned == 1)) {

				Update_High_Scores(game_status.score);
				uint16_t *high_scores = Get_High_Scores();
				printf("High Scores: 1. %d   2. %d   3. %d\n", high_scores[0],
						high_scores[1], high_scores[2]);

				GFX_set_gfx_object_velocity(&misko, 0, 0);
				exit_value = 1;
				break;
			}

			if (misko.location.x_center > obstacle_pair1.top.location.x_min
					&& obstacle_pair1_spawned == 1 && !obstacle_pair1_scored) {
				if (misko.location.x_center
						> obstacle_pair1.top.location.x_center) {
					game_status.score += 1;
					OBJ_set_score_text_value(game_status.score);
					obstacle_pair1_scored = 1;
				}
			}

			if (misko.location.x_center > obstacle_pair2.top.location.x_min
					&& obstacle_pair2_spawned == 1 && !obstacle_pair2_scored) {
				if (misko.location.x_center
						> obstacle_pair2.top.location.x_center) {
					game_status.score += 1;
					OBJ_set_score_text_value(game_status.score);
					obstacle_pair2_scored = 1;
				}
			}

			if (misko.location.x_center > obstacle_pair3.top.location.x_min
					&& obstacle_pair3_spawned == 1 && !obstacle_pair3_scored) {
				if (misko.location.x_center
						> obstacle_pair3.top.location.x_center) {
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
        KBD_flush();
        HAL_Delay(300);

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

        GFX_draw_gfx_object(&background);
        OBJ_init_high_score_sprite_large(30, 30);
        GFX_draw_one_gfx_object_on_background(&high_score_sprite_large,&background);

        uint16_t *high_scores_2 = Get_High_Scores();
        char score_text_2[20];

        OBJ_init_text_big(75, 40, "GAME OVER!", &high_score_menu_text);
        GFX_display_text_object(&high_score_menu_text);

        OBJ_init_text_small(50, 85, "YOUR SCORE:", &your_hs_text);
        GFX_display_text_object(&your_hs_text);
        sprintf(score_text_2, "%d", game_status.score);
        OBJ_init_text_small(195, 85, score_text_2, &your_hs_value_text);
        GFX_display_text_object(&your_hs_value_text);

        OBJ_init_text_small(50, 115, "HIGHEST SCORE:", &biggest_hs_text); 
        GFX_display_text_object(&biggest_hs_text);
        sprintf(score_text_2, "%d", high_scores_2[0]);
        OBJ_init_text_small(233, 115, score_text_2, &high_score1_text_value);
        GFX_display_text_object(&high_score1_text_value);

        OBJ_init_text_tiny(43, 170, "PRESS OK TO RESTART OR ESC", &press_to_go_back_text);
        GFX_display_text_object(&press_to_go_back_text);
        OBJ_init_text_tiny(43, 185, "TO GO BACK TO THE MAIN MENU", &press_to_go_back_text2);
        GFX_display_text_object(&press_to_go_back_text2);

        game_status.score = 0;

        state = GAMEOVER_WAIT_FOR_ANY_KEY;
        exit_value = 0;
        break;

    case GAMEOVER_WAIT_FOR_ANY_KEY:
        key = KBD_get_pressed_key();
		static stopwatch_handle_t touch_polling_stopwatch;
		static stopwatch_handle_t touch_debounce_stopwatch;
		
		if (current_input_mode == INPUT_TOUCHSCREEN) {
			if (!touch_init) {
				TIMUT_stopwatch_set_time_mark(&touch_polling_stopwatch);
				TIMUT_stopwatch_set_time_mark(&touch_debounce_stopwatch);
				touch_init = 1;
			}

			if (TIMUT_stopwatch_has_another_X_ms_passed(&touch_polling_stopwatch, 60)) {
				int x, y; 
				XPT2046_touch_get_coordinates(&x, &y);
				
				if (y < 240 && press_enable == 1) {
					key = BTN_OK;  
					press_enable = 0;
					TIMUT_stopwatch_set_time_mark(&touch_debounce_stopwatch);
				}
			}

			if (TIMUT_stopwatch_has_X_ms_passed(&touch_debounce_stopwatch, 60)) {
				press_enable = 1;
			}
		}

		if (key == BTN_OK || key == BTN_ESC) {
			GFX_clear_gfx_object_on_background(&misko, &background);
			GFX_set_gfx_object_location(&misko, 80, 120);  
			GFX_set_gfx_object_velocity(&misko, 0, 0);
			game_status.score = 0;
			OBJ_set_score_text_value(game_status.score); 
			GFX_display_text_object(&score_text);        
			state = GAMEOVER_SCREEN;  
			exit_value = (key == BTN_ESC) ? 1 : 2;
			touch_init = 0;  
			press_enable = 1;  
			touch_initialized = 0;  
		}
		break;

    default:
        state = GAMEOVER_SCREEN;
        break;
    }

    return exit_value;
}
