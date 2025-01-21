/*
 * objects.c
 *
 *  Created on: 10 May 2022
 *      Author: Gasper
 */



/* **************** MODULE DESCRIPTION *************************

This module defines application objects and functions that manipulate
the object parameters.


************************************************************* */





// ----------- Include other modules (for private) -------------


// ------ Standard C libraries -----

#include <string.h>		// support for memory copy





// -------- System level modules -------

#include "joystick.h"

#include "kbd.h"				// for demo only
#include "timing_utils.h"		// for demo only



// -------- Application level modules -------

#include "objects.h"






// ---------------------- Private definitions ------------------



// ---- Object for defining application settings and status ------


settings_t 		settings;		//objekti ki hranijo nastavitve in status igrice
game_status_t	game_status;

canvas_t		canvas;



// --- Graphical objects ----

graphic_object_t	intro_sprite;			// grafični objekti aplikacije
graphic_object_t	background;
graphic_object_t	game_over_sprite;
graphic_object_t	press_ok_sprite;

// ------- Compound objects -------

graphic_object_t	misko;		// sestavljeni objekti aplikacije
graphic_object_t 	obstacle_top;
graphic_object_t 	obstacle_bottom;

// ----- Text objects ------

text_object_t	score_box_title;		//tekstovni objekti
text_object_t	score_text;


//obstacle_pair_t obstacle_pair1;
//obstacle_pair_t obstacle_pair2;
//obstacle_pair_t obstacle_pair3;

// ------------- Public function implementations --------------

// init all the objects
void OBJ_init(void)
{
	// init settings
	OBJ_init_settings();
	OBJ_init_game_status();
	OBJ_init_canvas();

	// full screen images
	OBJ_init_background();

	// sprites
	OBJ_init_game_over_sprite();
	OBJ_init_press_ok_sprite();
	OBJ_init_intro_sprite();

	// compound objects
	OBJ_init_misko();

	// text boxes
	OBJ_init_score_box_title();
	OBJ_init_score_text();

}



// ---------------- OBJECTS for SETTINGS ----------------

void OBJ_init_settings(void)
{
	// set the min/max limits for velocity of moving objects
	settings.abs_velocity_min.x = 1;
	settings.abs_velocity_min.y = 1;

	settings.abs_velocity_max.x = 4;		// nastavitev max in min hitrosti
	settings.abs_velocity_max.y = 4;

	//settings.upmax = 10;		// max in min za generator kako visoko se generira ovira
	//settings.upmin = 110;
	//settings.downmax = 230;
	//settings.downmin = 110;
}




void OBJ_init_game_status(void)
{
	game_status.score = 0;					// tukaj se bo hranil score igre
}


// object "constructor"
void OBJ_init_canvas(void)
{
	// whole area
		canvas.whole_area.x_min = 0;
		canvas.whole_area.y_min = 0;

		canvas.whole_area.x_max = DISPLAY_SIZE_X - 1;
		canvas.whole_area.y_max = DISPLAY_SIZE_Y - 1;

		canvas.whole_area.x_center = DISPLAY_CENTER_X;
		canvas.whole_area.y_center = DISPLAY_CENTER_Y;



	// above ground area
		canvas.above_ground.x_min = 0;
		canvas.above_ground.y_min = 0;

		canvas.above_ground.x_max = canvas.whole_area.x_max;
		canvas.above_ground.y_max = 240;						//prej je bilo 200, kar je na ekranu je above ground

		canvas.above_ground.x_center = DISPLAY_CENTER_X;
		canvas.above_ground.y_center = canvas.above_ground.y_max/2;


	// below ground area
		canvas.below_ground.x_min = 0;
		canvas.below_ground.y_min = canvas.above_ground.y_max + 1;

		canvas.below_ground.x_max = canvas.whole_area.x_max;
		canvas.below_ground.y_max = canvas.whole_area.y_max;

		canvas.below_ground.x_center = DISPLAY_CENTER_X;
		canvas.below_ground.y_center = (canvas.below_ground.y_max - canvas.below_ground.y_min) / 2;
}




// ------------------ FULL SCREEN IMAGES ----------------------


// object "constructor"
void OBJ_init_background(void)
{
	// init graphic object

		// init image
		background.image.image_array = (uint16_t*) background_img;

		background.image.size_x = 320;
		background.image.size_y = 240;
		background.image.size = background.image.size_x * background.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&background, &canvas.whole_area );
		GFX_init_gfx_object_location( &background, 0, 0);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&background, 0, 0);

		// set edge parameters
		background.edge_behavior = EDGE_IGNORE;
}





// ------------------ SPRITES ----------------------

// In computer graphics, a sprite is a two-dimensional bitmap that is
// integrated into a larger scene, most often in a 2D video game.

void OBJ_init_intro_sprite(void)
{
	// init graphic object

		// init image
		intro_sprite.image.image_array = (uint16_t*) intro_img;

		intro_sprite.image.size_x = 130;
		intro_sprite.image.size_y = 100;
		intro_sprite.image.size = intro_sprite.image.size_x * intro_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&intro_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &intro_sprite, 95, 20);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&intro_sprite, 0, 0);


		// set edge parameters
		intro_sprite.edge_behavior = EDGE_IGNORE;
}



// object "constructor"
void OBJ_init_game_over_sprite(void)
{
	// init graphic object

		// init image
		game_over_sprite.image.image_array = (uint16_t*) game_over_img;

		game_over_sprite.image.size_x = 250;
		game_over_sprite.image.size_y = 170;
		game_over_sprite.image.size = game_over_sprite.image.size_x * game_over_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions ( &game_over_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &game_over_sprite, 35, 35 );



		// set the initial object velocity
		GFX_set_gfx_object_velocity(&game_over_sprite, 0, 0);


		// set edge parameters
		game_over_sprite.edge_behavior = EDGE_IGNORE;
}


/*
// object "constructor"
void OBJ_init_press_any_key_sprite(void)
{
	// init graphic object

		// init image
		press_any_key_sprite.image.image_array = (uint16_t*) press_any_key_img;

		press_any_key_sprite.image.size_x = 260;
		press_any_key_sprite.image.size_y = 60;
		press_any_key_sprite.image.size = press_any_key_sprite.image.size_x * press_any_key_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&press_any_key_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &press_any_key_sprite, 40, 150);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&press_any_key_sprite, 0, 0);


		// set edge parameters
		press_any_key_sprite.edge_behavior = EDGE_IGNORE;

}



void OBJ_spawn_target(void)
{
	arduino_types_enum_t arduino_type;
	int16_t x, y;

	// generate a target from a random arduino
	arduino_type = MATH_random_integer_number_from_interval(0, NUM_OF_ARDUINO_TYPES - 1);

	OBJ_init_target(arduino_type);


	// randomize the target position
	x = MATH_random_integer_number_from_interval(target.gfx_object.center_limits.X_MIN, target.gfx_object.center_limits.X_MAX);
	y = MATH_random_integer_number_from_interval(target.gfx_object.center_limits.Y_MIN, target.gfx_object.center_limits.Y_MAX);

	GFX_init_gfx_object_center_location( &target.gfx_object, x, y );


	// randomize the target velocity
	x = MATH_random_sign() * MATH_random_integer_number_from_interval(settings.abs_velocity_min.x, settings.abs_velocity_max.x);
	y = MATH_random_sign() * MATH_random_integer_number_from_interval(settings.abs_velocity_min.y, settings.abs_velocity_max.y);

	GFX_set_gfx_object_velocity( &target.gfx_object, x, y );

}


*/


void OBJ_init_press_ok_sprite(void)
{
	// init graphic object

		// init image
		press_ok_sprite.image.image_array = (uint16_t*) press_ok_img;

		press_ok_sprite.image.size_x = 260;
		press_ok_sprite.image.size_y = 30;
		press_ok_sprite.image.size = press_ok_sprite.image.size_x * press_ok_sprite.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&press_ok_sprite, &canvas.whole_area );
		GFX_init_gfx_object_location( &press_ok_sprite, 40, 170);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&press_ok_sprite, 0, 0);


		// set edge parameters
		press_ok_sprite.edge_behavior = EDGE_IGNORE;
}


void OBJ_init_misko(void)
{
	// init graphic object

		// init image
		misko.image.image_array = (uint16_t*) misko_img;

		misko.image.size_x = 50;
		misko.image.size_y = 25;
		misko.image.size = misko.image.size_x * misko.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&misko, &canvas.whole_area );
		GFX_init_gfx_object_location( &misko, 50, 120);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&misko, 0, 0);

		// set edge parameters
		background.edge_behavior = EDGE_IGNORE;
}


void OBJ_init_obstacle_top(void)
{
	// init graphic object

		// init image
		obstacle_top.image.image_array = (uint16_t*) obstacle_top_img;

		obstacle_top.image.size_x = 50;
		obstacle_top.image.size_y = 240;
		obstacle_top.image.size = obstacle_top.image.size_x * obstacle_top.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&obstacle_top, &canvas.whole_area );

		// set the initial object velocity
		GFX_set_gfx_object_velocity(&obstacle_top, 0, 0);

		// set edge parameters
		background.edge_behavior = EDGE_IGNORE;
}


void OBJ_init_obstacle_bottom(void)
{
	// init graphic object

		// init image
		obstacle_bottom.image.image_array = (uint16_t*) obstacle_bottom_img;

		obstacle_bottom.image.size_x = 50;
		obstacle_bottom.image.size_y = 240;
		obstacle_bottom.image.size = obstacle_bottom.image.size_x * obstacle_bottom.image.size_y;

		// init the graphic object location
		GFX_init_location_restrictions (&obstacle_bottom, &canvas.whole_area );
		//GFX_init_gfx_object_location( &obstacle_bottom, x_cooridnate, y_coordinate);


		// set the initial object velocity
		GFX_set_gfx_object_velocity(&obstacle_bottom, 0, 0);

		// set edge parameters
		background.edge_behavior = EDGE_IGNORE;
}


void OBJ_init_obstacle_pair(obstacle_pair_t *obstacle_pair)
{
    // Initialize top obstacle
    obstacle_pair->top.image.image_array = (uint16_t*) obstacle_top_img;
    obstacle_pair->top.image.size_x = 50;
    obstacle_pair->top.image.size_y = 240;
    obstacle_pair->top.image.size = obstacle_pair->top.image.size_x * obstacle_pair->top.image.size_y;
    GFX_init_location_restrictions(&obstacle_pair->top, &canvas.whole_area);
    obstacle_pair->top.edge_behavior = EDGE_IGNORE;

    // Initialize bottom obstacle
    obstacle_pair->bottom.image.image_array = (uint16_t*) obstacle_bottom_img;
    obstacle_pair->bottom.image.size_x = 50;
    obstacle_pair->bottom.image.size_y = 240;
    obstacle_pair->bottom.image.size = obstacle_pair->bottom.image.size_x * obstacle_pair->bottom.image.size_y;
    GFX_init_location_restrictions(&obstacle_pair->bottom, &canvas.whole_area);
    obstacle_pair->bottom.edge_behavior = EDGE_IGNORE;
}


//int32_t MATH_random_integer_number_from_interval(int32_t x_min, int32_t x_max);


int randomgenerator(void){
		int yup = MATH_random_integer_number_from_interval(120, 240);
		return yup;
}

//------------- tale random generator nevem če je prav        vmes mora biti 80 pikslov prostora za ptico


// TO DO: delete


void OBJ_spawn_obstacles(void){

	int yup = randomgenerator();
	int ydown = yup - 100;

	// spremeni v funkcijah da se lahko nastavlja samo y ovir
	//OBJ_init_obstacle_bottom(320, ydown);
	OBJ_init_obstacle_top();

	GFX_draw_one_gfx_object_on_background(&obstacle_top, &background);
	GFX_draw_one_gfx_object_on_background(&obstacle_bottom, &background);
	//GFX_draw_two_gfx_objects_on_background(&obstacle_top, &obstacle_bottom, &background);

}






// ------------ TEXT OBJECTS -------------


// ------- Score text --------

void OBJ_init_score_box_title(void)
{
	static char str[]= "SCORE";

	score_box_title.text = str;

	score_box_title.x_min = 243;
	score_box_title.y_min = 205;

	score_box_title.fore_color = C_WHITE;
	score_box_title.back_color = C_BLACK;

	score_box_title.font = (UG_FONT*) &FONT_8X12;     // pred errorjem je bil FONT_8X12 nimam pojma zakaj
}



void OBJ_set_score_text_value(int16_t score)
{
	static char str[6];

	sprintf(str, "%5i", score);
	score_text.text = str;

}


void OBJ_init_score_text(void)
{
	OBJ_set_score_text_value( game_status.score );

	score_text.x_min = 242;
	score_text.y_min = 219;

	score_text.fore_color = C_WHITE;
	score_text.back_color = C_BLACK;

	score_text.font = (UG_FONT*) &FONT_8X12;    // pred errorjem je bil FONT_8X8 nimam pojma zakaj
}


// -------------- Private function implementations -------------
