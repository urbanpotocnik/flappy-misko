/*
 * game.c
 *
 *  Created on: 20.05.2022
 *      Author:
 */

/* **************** MODULE DESCRIPTION *************************

V tem modulu bom implementiral igrico s pomočjo avtomatov stanj

************************************************************* */

// ----------- Include other modules (for private) -------------

// ------ Standard C libraries -----
#include <stdio.h>
#include <stdlib.h> // support for random number generation

// vključitev modulov strojnega nivoja
#include "LED.h"                // ledice
#include "kbd.h"                // tipkovnica
#include "SCI.h"                // serijski umesnik
#include "joystick.h"           // joystick
#include "lcd.h"                // lcd
#include "timing_utils.h"       // merjenje časa

// vključitev modulov aplikacijskega nivoja
#include "game.h"               // lastne definicije game.c modula
#include "objects.h"            // modul za delo z objekti igre
#include "graphics.h"           // modul za prikaz objektov na zaslon

// ----- Definicija možnih stanj avtomatov --------

stopwatch_handle_t   stopwatch;


// stanja avtomata Game()
typedef enum GAME_states {
    GAME_INTRO_STATE,           // izris pozdravne slike
    GAME_PLAY_STATE,            // igranje igre
    GAME_OVER_STATE,            // izris zaključne slike in scora
    GAME_NUM_OF_STATES
} GAME_states_t;

// stanja avtomata Intro()
typedef enum INTRO_states {
    INTRO_INIT,                 // inicializacija podatkovnih struktur, nariše se splash screen, 3x blink vseh ledic, čiščenje bufferja tipkovnice
    INTRO_PRESS_ANY_KEY,        // izpis napisa press any key
    INTRO_WAIT_FOR_ANY_KEY,     // čakanje na pritisk tipke
    INTRO_NUM_OF_STATES
} INTRO_states_t;

// stanja avtomata GamePlay()
typedef enum GAMEPLAY_states {
    GAMEPLAY_INIT,
    GAMEPLAY_JUMP
} GAMEPLAY_states_t;

// stanja avtomata GameOver()
typedef enum GAMEOVER_states {
    GAMEOVER_SCREEN,            // izpis game over in scora
    GAMEOVER_WAIT_FOR_ANY_KEY,  // čakanje na pritisk tipke ali pa 10s
    GAMEOVER_NUM_OF_STATES
} GAMEOVER_states_t;

// ------------- Public function implementations --------------

// Funkcija, ki implementira nad-avtomat Game().
void Game() {
    static GAME_states_t state = GAME_INTRO_STATE; // shranimo stanje avtomata
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

// implementacija podavtomata intro
uint8_t Intro() {
    #define INTRO_DELAY_BEFORE_KBD_ACTIVE 3000
    static INTRO_states_t state = INTRO_INIT;
    uint8_t exit_value = 0;
    buttons_enum_t key;
    static stopwatch_handle_t stopwatch;

    switch (state) {
        case INTRO_INIT:
            OBJ_init();
            GFX_draw_gfx_object(&background);
            //

            GFX_draw_one_gfx_object_on_background(&intro_sprite, &background);
            TIMUT_stopwatch_set_time_mark(&stopwatch);
            int a = 0x01;
            while (1) {
                LEDs_write(a);
                HAL_Delay(100);
                a = a << 1;
                if (a == 0x80) {
                    a = 0x01;
                }
                if (TIMUT_get_stopwatch_elapsed_time(&stopwatch) > INTRO_DELAY_BEFORE_KBD_ACTIVE) {
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

// implementacija podavtomata GamePlay
uint8_t GamePlay() {
    static GAMEPLAY_states_t gameplay_state = GAMEPLAY_INIT;
    uint8_t exit_value = 0;
    static stopwatch_handle_t stopwatch;
    buttons_enum_t pressed_button = BTN_NONE;

    void GamePlay_UpdateChanges(void) {
        static stopwatch_handle_t update_stopwatch;
        if (TIMUT_stopwatch_has_another_X_ms_passed(&update_stopwatch, settings.game_play_update_period)) {
            OBJ_set_score_text_value(game_status.score);
            GFX_display_text_object(&score_text);
            GFX_draw_one_gfx_object_on_background(&bird, &background);
            GFX_update_moving_gfx_object_location(&bird);
        }
    }

    switch (gameplay_state) {
        case GAMEPLAY_INIT:
            OBJ_init();
            OBJ_set_score_text_value(game_status.score);
            GFX_display_text_object(&score_box_title);
            GFX_display_text_object(&score_text);
            OBJ_init_settings();
            MATH_init_random_generator();
            //int x = 3;
            //int y = 4;
            GFX_draw_one_gfx_object_on_background(&bird, &background);
            GFX_set_gfx_object_velocity(&bird, 0, 1);
            gameplay_state = GAMEPLAY_JUMP;
            exit_value = 0;
            break;

        case GAMEPLAY_JUMP:

			KBD_flush();
			GFX_clear_gfx_object_on_background(&press_ok_sprite, &background);

            GFX_set_gfx_object_velocity(&bird, 0, 0);

			while(1){
                KBD_scan();
                pressed_button = KBD_get_pressed_key();

               

                if(pressed_button == BTN_OK){
                    TIMUT_stopwatch_set_time_mark(&stopwatch);	
                    while ( !TIMUT_stopwatch_has_X_ms_passed(&stopwatch, 500) )
                    {
                       GFX_set_gfx_object_velocity(&bird, 0, 1);   
                       GamePlay_UpdateChanges();  
                    }
                    GFX_set_gfx_object_velocity(&bird, 0, -1);
                    
			    }

                				

                


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

// implementacija podavtomata game over
uint8_t GameOver() {
    #define GAMEOVER_DELAY_BEFORE_KBD_ACTIVE 3000
    static GAMEOVER_states_t state = GAMEOVER_SCREEN;
    uint8_t exit_value = 0;
    static stopwatch_handle_t stopwatch;
    buttons_enum_t key;

    switch (state) {
        case GAMEOVER_SCREEN:
            GFX_draw_one_gfx_object_on_background(&game_over_sprite, &background);
            KBD_flush();
            int a = 0x01;
            TIMUT_stopwatch_set_time_mark(&stopwatch);
            while (1) {
                LEDs_write(a);
                HAL_Delay(100);
                a = a << 1;
                if (a == 0x80) {
                    a = 0x01;
                }
                if (TIMUT_get_stopwatch_elapsed_time(&stopwatch) > GAMEOVER_DELAY_BEFORE_KBD_ACTIVE) {
                    LEDs_off(0xFF);
                    break;
                }
            }
            state = GAMEOVER_WAIT_FOR_ANY_KEY;
            exit_value = 0;
            break;

        case GAMEOVER_WAIT_FOR_ANY_KEY:
            key = KBD_get_pressed_key();
            if ((TIMUT_get_stopwatch_elapsed_time(&stopwatch) > 10000) || (key != BTN_NONE)) {
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
