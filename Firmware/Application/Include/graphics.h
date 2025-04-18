#ifndef INCLUDE_GRAPHICS_H_
#define INCLUDE_GRAPHICS_H_

#include "LCD.h"

#define DISPLAY_SIZE_X	320
#define DISPLAY_SIZE_Y	240
#define DISPLAY_CENTER_X	((DISPLAY_SIZE_X/2) - 1 )
#define DISPLAY_CENTER_Y	((DISPLAY_SIZE_Y/2) - 1 )

// --------------- Definition of enumerated types ----------------

typedef enum { EDGE_BOUNCE, EDGE_CROSS, EDGE_IGNORE } edge_behavior_t;
typedef enum { TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE, LEFT_EDGE, NO_EDGE, ALL_EDGES } edge_t;
typedef enum { HIDDEN, VISIBLE } visibility_t;

// ----- GEOMETRIC OBJECT TYPES -------

typedef struct
{
	int16_t x_min;
	int16_t y_min;
	int16_t x_max;
	int16_t y_max;
	int16_t x_center;
	int16_t y_center;

} location_t;

typedef struct
{
	int8_t x;
	int8_t y;

} velocity_t;

typedef struct
{
	uint16_t X_MIN;
	uint16_t X_MAX;
	uint16_t X_RANGE;
	uint16_t Y_MIN;
	uint16_t Y_MAX;
	uint16_t Y_RANGE;

} location_restriction_t;


// ------ IMAGE AND GRAPHIC OBJECT TYPES ------

typedef struct
{
	uint16_t 	*image_array;
	uint16_t 	size_x;
	uint16_t	size_y;
	uint32_t	size;

} image_object_t;


typedef struct
{
	image_object_t  image;
	location_t	location;		 
	location_t	location_old;		
	location_t	location_new;		
	location_restriction_t	top_left_limits;		
	location_restriction_t	center_limits;			
	velocity_t	velocity;			
	edge_behavior_t		edge_behavior;

} graphic_object_t;


// ----- SHAPE OBJECT TYPES ------

typedef struct
{
	int16_t	x1;
	int16_t	y1;
	int16_t	x2;
	int16_t	y2;
	UG_COLOR	color;

} shape_line_t;

typedef struct
{
	int16_t	x_min;
	int16_t	y_min;
	int16_t	x_max;
	int16_t	y_max;
	uint8_t	length;
	uint8_t	height;
	UG_COLOR	color;

} shape_rectangle_t;

typedef struct
{
	int16_t	x;
	int16_t	y;
	uint8_t	r;
	UG_COLOR	color;

} shape_circle_t;


// ---- PROGRESS BAR ----

typedef struct
{
	uint8_t		value_percent;
	uint8_t		full_length_px;
	shape_rectangle_t	bar;
	UG_COLOR	background_color;

} progress_bar_t;


// ------ TEXT OBJECT TYPES ------

typedef struct
{
	char 		*text;
	int16_t		x_min;
	int16_t		y_min;
	UG_FONT		*font;
	UG_COLOR	fore_color;
	UG_COLOR	back_color;

} text_object_t;

typedef struct
{
	text_object_t			txt_object;
	shape_rectangle_t		box;

} text_box_t;

typedef struct 
{
    graphic_object_t top;
    graphic_object_t bottom;

} obstacle_pair_t;

extern obstacle_pair_t obstacle_pair1;
extern obstacle_pair_t obstacle_pair2;
extern obstacle_pair_t obstacle_pair3;


// ---------------- Public function prototypes ----------------

// --- Image related -----
void GFX_display_image_array(uint16_t *img_array, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y);
void GFX_display_image_object(image_object_t *image, int16_t x, int16_t y);
uint16_t GFX_get_image_pixel(image_object_t *img, int16_t x_img, int16_t y_img);
void GFX_get_image_part(image_object_t *img, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y, uint16_t *sub_img_array);
uint16_t* GFX_allocate_image_buffer(uint32_t size_px);
void GFX_deallocate_image_buffer(uint16_t *ptr);

// --- Graphical object related ----
void GFX_draw_gfx_object(graphic_object_t *gfx_object);
void GFX_save_gfx_object_location(graphic_object_t *gfx_object);
void GFX_restore_gfx_object_old_location(graphic_object_t *gfx_object);
uint8_t GFX_is_point_inside_location(location_t *location, int16_t x, int16_t y);
uint8_t GFX_is_location_inside_object_restrictions(location_restriction_t *restrictions, int16_t x, int16_t y);
uint8_t GFX_set_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y);
uint8_t GFX_init_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y);
uint8_t GFX_set_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y);
uint8_t GFX_init_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y);
uint8_t GFX_move_gfx_object(graphic_object_t *gfx_object, int16_t x, int16_t y);
uint8_t GFX_move_gfx_object_center(graphic_object_t *gfx_object, int16_t x, int16_t y);
void GFX_init_location_restrictions(graphic_object_t *gfx_object, location_t *canvas_location);
void GFX_set_gfx_object_velocity(graphic_object_t *gfx_object, int8_t velocity_x, int8_t velocity_y);
void GFX_move_location(location_t *location, location_t *location_new, int16_t dx, int16_t dy);
uint8_t GFX_update_moving_gfx_object_location(graphic_object_t *gfx_object);
uint8_t GFX_bounce_moving_object_from_edge(graphic_object_t *gfx_object);
void GFX_get_object_movement_area(graphic_object_t *gfx_object, location_t *object_movement_area);
void GFX_absolute_coordinates_to_object_coordinates(graphic_object_t *object, int16_t x_abs, int16_t y_abs, int16_t *x_obj, int16_t *y_obj);
uint8_t GFX_are_locations_overlapping(location_t *location_A, location_t *location_B);
uint8_t GFX_are_gfx_objects_overlapping(graphic_object_t *object_A, graphic_object_t *object_B);
void GFX_allocate_partial_frame_buffer_for_display_area(location_t	*area_location );
void GFX_copy_image_part_to_partial_frame_buffer( image_object_t *image );
void GFX_deallocate_partial_frame_buffer(void);
void GFX_draw_one_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd);
void GFX_draw_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd);
void GFX_clear_area_on_background(int16_t x_min, int16_t y_min, int16_t x_max, int16_t y_max, graphic_object_t *bckgnd);
void GFX_clear_location_on_background( location_t *location, graphic_object_t *bckgnd );
void GFX_clear_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd);
void GFX_demo_update_two_gfx_objects_on_background_delete_draw(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd);
void GFX_update_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd);
void GFX_display_text_object(text_object_t *text_box);
void GFX_display_progress_bar(progress_bar_t *progress_bar);
void GFX_init_obstacle_pair_location(obstacle_pair_t *pair, int16_t x, int16_t top_y, int16_t bottom_y);
void GFX_set_obstacle_pair_x_axis_velocity(obstacle_pair_t *pair, int8_t x_velocity);
void GFX_update_obstacle_pair_location(obstacle_pair_t *pair);
void GFX_draw_obstacle_pair_on_background(obstacle_pair_t *pair, graphic_object_t *background);
void GFX_get_obstacle_pair_movement_area(obstacle_pair_t *pair, location_t *object_movement_area);
void GFX_clear_obstacle_pair_on_background(obstacle_pair_t *pair, location_t *object_movement_area);

// Drawing via DMA functions
void GFX_draw_gfx_object_via_DMA(graphic_object_t *gfx_object);
void GFX_draw_one_gfx_object_on_background_via_DMA(graphic_object_t *object, graphic_object_t *bckgnd);
void GFX_draw_two_gfx_objects_on_background_via_DMA(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd);	

// --- Demo ---
void GFX_demo_sub_image(void);
void GFX_demo_sub_image_with_malloc(void);
void GFX_demo_draw_one_object_on_backround(graphic_object_t *object);
void GFX_demo_arduino_objects(void);
void GFX_demo_objects_overlapping_delete_draw(void);
void GFX_demo_move_one_object_delete_draw(graphic_object_t *object);
void GFX_demo_two_objects_moving_delete_draw(void);
void GFX_demo_move_one_object_using_movement_area(graphic_object_t *object);
void GFX_demo_two_objects_moving_using_movement_areas(void);
void GFX_demo_joystick_controlled_crosshair_arduino_bouncing(void);


#endif /* INCLUDE_GRAPHICS_H_ */
