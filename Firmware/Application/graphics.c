#include <stdlib.h>		
#include <math.h>		
#include "kbd.h"		
#include "graphics.h"
#include "objects.h"
#include "images.h"

// ---------------------- Private definitions ------------------
graphic_object_t partial_frame_buffer;


// ----------- IMAGE RELATED FUNCTIONALITY ------------
void GFX_display_image_array(uint16_t *img_array, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y) {
	ILI9341_SetDisplayWindow(x, y, size_x, size_y);
	ILI9341_SendData((LCD_IO_Data_t*) img_array, size_x * size_y);
}

void GFX_display_image_object(image_object_t *image, int16_t x, int16_t y) {
	ILI9341_SetDisplayWindow(x, y, image->size_x, image->size_y);
	ILI9341_SendData((LCD_IO_Data_t*) image->image_array, image->size);
}

uint16_t GFX_get_image_pixel(image_object_t *img, int16_t x_img, int16_t y_img) {
	uint32_t i;

	if (((x_img >= 0) && (x_img < img->size_x))
			&& ((y_img >= 0) && (y_img < img->size_y))) {
		i = y_img * img->size_x + x_img;

		return img->image_array[i];
	}

	else {
		return IMG_TRANSPARENT_COLOR_CODE;
	}

}

void GFX_get_image_part(image_object_t *img, int16_t x, int16_t y, uint16_t size_x, uint16_t size_y, uint16_t *sub_image_array) {
	uint32_t i = 0;

	if (y + size_y > img->size_y)
		return;

	if (x + size_x > img->size_x)
		return;

	for (uint16_t row = y; row < (y + size_y); row++) {
		for (uint16_t column = x; column < (x + size_x); column++) {
			sub_image_array[i] = GFX_get_image_pixel(img, column, row);
			i++;
		}
	}
}

uint16_t* GFX_allocate_image_buffer(uint32_t size_px) {
	return ((uint16_t*) malloc(size_px * sizeof(uint16_t)));
}

void GFX_deallocate_image_buffer(uint16_t *ptr) {
	free(ptr);
}


// ----------- GRAPHICAL OBJECT RELATED FUNCTIONALITY ---------
void GFX_draw_gfx_object(graphic_object_t *gfx_object) {

	ILI9341_SetDisplayWindow(gfx_object->location.x_min, gfx_object->location.y_min, gfx_object->image.size_x, gfx_object->image.size_y);

	ILI9341_SendData((LCD_IO_Data_t*) gfx_object->image.image_array, gfx_object->image.size);
}

// Object drawing function that sends data via DMA, so it speeds up the drawing process
void GFX_draw_gfx_object_via_DMA(graphic_object_t *gfx_object) {
	ILI9341_SetDisplayWindow(gfx_object->location.x_min, gfx_object->location.y_min, gfx_object->image.size_x, gfx_object->image.size_y);
	ILI9341_SendDataDMA((LCD_IO_Data_t*) gfx_object->image.image_array, gfx_object->image.size);
}

void GFX_save_gfx_object_location(graphic_object_t *gfx_object) {
	gfx_object->location_old.x_min = gfx_object->location.x_min;
	gfx_object->location_old.y_min = gfx_object->location.y_min;

	gfx_object->location_old.x_max = gfx_object->location.x_max;
	gfx_object->location_old.y_max = gfx_object->location.y_max;

	gfx_object->location_old.x_center = gfx_object->location.x_center;
	gfx_object->location_old.y_center = gfx_object->location.y_center;

}

void GFX_restore_gfx_object_old_location(graphic_object_t *gfx_object) {
	gfx_object->location.x_min = gfx_object->location_old.x_min;
	gfx_object->location.y_min = gfx_object->location_old.y_min;

	gfx_object->location.x_max = gfx_object->location_old.x_max;
	gfx_object->location.y_max = gfx_object->location_old.y_max;

	gfx_object->location.x_center = gfx_object->location_old.x_center;
	gfx_object->location.y_center = gfx_object->location_old.y_center;

}

void GFX_init_location_restrictions(graphic_object_t *gfx_object, location_t *canvas_location) {
	gfx_object->top_left_limits.X_MIN = canvas_location->x_min;
	gfx_object->top_left_limits.Y_MIN = canvas_location->y_min;

	gfx_object->top_left_limits.X_MAX = canvas_location->x_max - gfx_object->image.size_x;
	gfx_object->top_left_limits.Y_MAX = canvas_location->y_max - gfx_object->image.size_y;

	gfx_object->top_left_limits.X_RANGE = gfx_object->top_left_limits.X_MAX - gfx_object->top_left_limits.X_MIN;
	gfx_object->top_left_limits.Y_RANGE = gfx_object->top_left_limits.Y_MAX - gfx_object->top_left_limits.Y_MIN;

	gfx_object->center_limits.X_MIN = canvas_location->x_min + ceil( gfx_object->image.size_x / 2 );
	gfx_object->center_limits.Y_MIN = canvas_location->y_min + ceil( gfx_object->image.size_y / 2 );

	gfx_object->center_limits.X_MAX = canvas_location->x_max - ceil( gfx_object->image.size_x / 2 );
	gfx_object->center_limits.Y_MAX = canvas_location->y_max - ceil( gfx_object->image.size_y / 2 );

	gfx_object->center_limits.X_RANGE = gfx_object->center_limits.X_MAX - gfx_object->center_limits.X_MIN;
	gfx_object->center_limits.Y_RANGE = gfx_object->center_limits.Y_MAX - gfx_object->center_limits.Y_MIN;

}

uint8_t GFX_is_point_inside_location(location_t *location, int16_t x, int16_t y) {
	if ((x >= location->x_min) && (x <= location->x_max) && (y >= location->y_min) && (y <= location->y_max)) {
		return 1;	
	} else
		return 0;	
}

uint8_t GFX_is_location_inside_object_restrictions(
		location_restriction_t *restrictions, int16_t x, int16_t y) {
	if ((x >= restrictions->X_MIN) && (x <= restrictions->X_MAX) && (y >= restrictions->Y_MIN) && (y <= restrictions->Y_MAX)) {
		return 1;	
	} else
		return 0; 
}

uint8_t GFX_init_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y) {
	uint8_t return_value;

	return_value = GFX_set_gfx_object_location(gfx_object, x, y);

	if (return_value) {
		GFX_save_gfx_object_location(gfx_object);
	}

	return return_value;

}

uint8_t GFX_set_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y) {

	if (GFX_is_location_inside_object_restrictions(&gfx_object->center_limits, x, y)) {

		gfx_object->location.x_center = x;
		gfx_object->location.y_center = y;


		gfx_object->location.x_min = gfx_object->location.x_center - gfx_object->image.size_x/2;
		gfx_object->location.y_min = gfx_object->location.y_center - gfx_object->image.size_y/2;

		gfx_object->location.x_max = gfx_object->location.x_min +  gfx_object->image.size_x;
		gfx_object->location.y_max = gfx_object->location.y_min +  gfx_object->image.size_y;

		return 1;	

	} else {

		return 0;	
	}
}

uint8_t GFX_init_gfx_object_center_location(graphic_object_t *gfx_object, int16_t x, int16_t y) {
	uint8_t return_value;

	return_value = GFX_set_gfx_object_center_location(gfx_object, x, y);

	if (return_value) {
		GFX_save_gfx_object_location(gfx_object);
	}

	return return_value;
}

uint8_t GFX_move_gfx_object(graphic_object_t *gfx_object, int16_t x, int16_t y) {
	GFX_save_gfx_object_location(gfx_object);
	return GFX_set_gfx_object_location(gfx_object, x, y);
}

uint8_t GFX_move_gfx_object_center(graphic_object_t *gfx_object, int16_t x, int16_t y) {
	GFX_save_gfx_object_location(gfx_object);
	return GFX_set_gfx_object_center_location(gfx_object, x, y);
}

uint8_t GFX_set_gfx_object_location(graphic_object_t *gfx_object, int16_t x, int16_t y) {
	
	if (GFX_is_location_inside_object_restrictions(&gfx_object->top_left_limits, x, y)) {
		gfx_object->location.x_min = x;
		gfx_object->location.y_min = y;

        gfx_object->location.x_max = gfx_object->location.x_min + gfx_object->image.size_x;
        gfx_object->location.y_max = gfx_object->location.y_min + gfx_object->image.size_y;

        gfx_object->location.x_center = gfx_object->location.x_min + gfx_object->image.size_x / 2;
        gfx_object->location.y_center = gfx_object->location.y_min + gfx_object->image.size_y / 2;

		return 1; 
	}

	else {

		if((gfx_object == &obstacle_pair1.top || gfx_object == &obstacle_pair2.top || gfx_object == &obstacle_pair3.top) && y < 0)
		{
			int16_t offset = -y;
			gfx_object->location.y_min = 0;
			gfx_object->location.y_max = 240 - offset;
			gfx_object->location.y_center = (gfx_object->location.y_min + gfx_object->location.y_max) / 2;

			gfx_object->location.x_min = x;
			gfx_object->location.x_max = gfx_object->location.x_min + gfx_object->image.size_x;

			gfx_object->image.image_array += offset * gfx_object->image.size_x;
			gfx_object->image.size_y -= offset;
			gfx_object->image.size = gfx_object->image.size_x * gfx_object->image.size_y;

			return 1; 
		}

		return 0;	
	}
}

void GFX_set_gfx_object_velocity(graphic_object_t *gfx_object, int8_t velocity_x, int8_t velocity_y) {
	gfx_object->velocity.x = velocity_x;
	gfx_object->velocity.y = velocity_y;
}

void GFX_move_location(location_t *location, location_t *location_new, int16_t dx, int16_t dy) {
	location_new->x_min = location->x_min + dx;
	location_new->x_max = location->x_max + dx;
	location_new->x_center = location->x_center + dx;

	location_new->y_min = location->y_min + dy;
	location_new->y_max = location->y_max + dy;
	location_new->y_center = location->y_center + dy;
}

uint8_t GFX_update_moving_gfx_object_location(graphic_object_t *gfx_object) {

	uint8_t object_placement_successful;

	GFX_save_gfx_object_location(gfx_object);

	GFX_move_location(&gfx_object->location, &gfx_object->location_new, gfx_object->velocity.x, -gfx_object->velocity.y);

	object_placement_successful = GFX_set_gfx_object_location(gfx_object, gfx_object->location_new.x_min, gfx_object->location_new.y_min);

	if (!object_placement_successful) {
		switch (gfx_object->edge_behavior) {

		case EDGE_BOUNCE:

			object_placement_successful = GFX_bounce_moving_object_from_edge(
					gfx_object);

			return object_placement_successful;

			break;

		case EDGE_CROSS:

			return object_placement_successful;

			break;

		case EDGE_IGNORE:

			return object_placement_successful;

			break;

		default:

			return object_placement_successful;

			break;

		}

	} else
		return object_placement_successful;

}

uint8_t GFX_bounce_moving_object_from_edge(graphic_object_t *gfx_object) {
	int16_t x, y;
	int16_t dx, dy;

	dx = gfx_object->location_new.x_min - gfx_object->top_left_limits.X_MAX;
	if (dx >= 0) {
		x = gfx_object->location_new.x_min - 2 * dx;
		y = gfx_object->location_new.y_min;

		GFX_set_gfx_object_velocity(gfx_object, -gfx_object->velocity.x, gfx_object->velocity.y);

	} else {

		dx = gfx_object->location_new.x_min - gfx_object->top_left_limits.X_MIN;
		if (dx <= 0) {
			x = gfx_object->location_new.x_min - 2 * dx;
			y = gfx_object->location_new.y_min;

			GFX_set_gfx_object_velocity(gfx_object, -gfx_object->velocity.x, gfx_object->velocity.y);
		}
	}

	dy = gfx_object->location_new.y_min - gfx_object->top_left_limits.Y_MAX;
	if (dy >= 0) {
		x = gfx_object->location_new.x_min;
		y = gfx_object->location_new.y_min - 2 * dy;


			GFX_set_gfx_object_velocity( gfx_object, gfx_object->velocity.x, -gfx_object->velocity.y);
		}
		else
		{
			dy = gfx_object->location_new.y_min - gfx_object->top_left_limits.Y_MIN;
			if( dy <= 0 )
			{
				x = gfx_object->location_new.x_min;
				y = gfx_object->location_new.y_min - 2*dy;

			GFX_set_gfx_object_velocity(gfx_object, gfx_object->velocity.x, -gfx_object->velocity.y);
		}

	}

	return GFX_set_gfx_object_location(gfx_object, x, y);

}

void GFX_get_object_movement_area(graphic_object_t *gfx_object, location_t *object_movement_area)
{
	object_movement_area->x_min = (gfx_object->location.x_min < gfx_object->location_old.x_min) ? gfx_object->location.x_min : gfx_object->location_old.x_min;
	object_movement_area->y_min = (gfx_object->location.y_min < gfx_object->location_old.y_min) ? gfx_object->location.y_min : gfx_object->location_old.y_min;

	object_movement_area->x_max = (gfx_object->location.x_max > gfx_object->location_old.x_max) ? gfx_object->location.x_max : gfx_object->location_old.x_max;
	object_movement_area->y_max = (gfx_object->location.y_max > gfx_object->location_old.y_max) ? gfx_object->location.y_max : gfx_object->location_old.y_max;


	object_movement_area->x_center = (object_movement_area->x_max - object_movement_area->x_min) / 2;
	object_movement_area->y_center = (object_movement_area->y_max - object_movement_area->y_min) / 2;
}

void GFX_absolute_coordinates_to_object_coordinates(graphic_object_t *object, int16_t x_abs, int16_t y_abs, int16_t *x_obj, int16_t *y_obj) {

	*x_obj = x_abs - object->location.x_min;
	*y_obj = y_abs - object->location.y_min;
}

uint8_t GFX_are_locations_overlapping(location_t *location_A,
		location_t *location_B) {

	if ((location_A->x_min > location_B->x_max) ||	
			(location_A->x_max < location_B->x_min) ||	
			(location_A->y_max < location_B->y_min) ||	
			(location_A->y_min > location_B->y_max))
			{
		return 0;	
	} else
		return 1;
}

uint8_t GFX_are_gfx_objects_overlapping(graphic_object_t *object_A, graphic_object_t *object_B) {

	return GFX_are_locations_overlapping(&object_A->location, &object_B->location);

}

void GFX_allocate_partial_frame_buffer_for_display_area(location_t *area_location) {

	partial_frame_buffer.location.x_min = area_location->x_min;
	partial_frame_buffer.location.x_max = area_location->x_max;

	partial_frame_buffer.location.y_min = area_location->y_min;
	partial_frame_buffer.location.y_max = area_location->y_max;

	partial_frame_buffer.location.x_center = area_location->x_center;
	partial_frame_buffer.location.y_center = area_location->y_center;

	// set the partial frame image buffer size
	partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
	partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
	partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;

	// allocate memory for the part of the frame, i.e. prepare the image buffer
	partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);

}


void GFX_copy_image_part_to_partial_frame_buffer( image_object_t *image )
{
	GFX_get_image_part( image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);
}

void GFX_partial_frame_buffer_overlay_object(graphic_object_t *object) {

	uint32_t i = 0;

	int16_t x_obj;
	int16_t y_obj;

	uint16_t pixel;




	for( uint16_t y_abs = partial_frame_buffer.location.y_min; y_abs < partial_frame_buffer.location.y_max; y_abs++ )
	{
		for( uint16_t x_abs = partial_frame_buffer.location.x_min; x_abs < partial_frame_buffer.location.x_max; x_abs++ )
		{
			GFX_absolute_coordinates_to_object_coordinates(object, x_abs, y_abs, &x_obj, &y_obj);
			pixel = GFX_get_image_pixel( &object->image, x_obj, y_obj);

			if ( pixel != IMG_TRANSPARENT_COLOR_CODE)
				partial_frame_buffer.image.image_array[i] = pixel;

			i++;
		}
	}

}

void GFX_deallocate_partial_frame_buffer(void) {
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);
}

void GFX_draw_one_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd) {

	location_t area;

	if (GFX_are_locations_overlapping(&object->location_old, &object->location)) {

		GFX_get_object_movement_area(object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(object);
		GFX_draw_gfx_object(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();

	} else {

		GFX_clear_location_on_background(&object->location_old, bckgnd);
		GFX_allocate_partial_frame_buffer_for_display_area(&object->location);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(object);
		GFX_draw_gfx_object(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();

	}

}

void GFX_draw_one_gfx_object_on_background_via_DMA(graphic_object_t *object, graphic_object_t *bckgnd) {
	location_t area;

	if (GFX_are_locations_overlapping(&object->location_old, &object->location)) {
		GFX_get_object_movement_area(object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(object);
		GFX_draw_gfx_object_via_DMA(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();
	} else {
		GFX_clear_location_on_background(&object->location_old, bckgnd);
		GFX_allocate_partial_frame_buffer_for_display_area(&object->location);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(object);
		GFX_draw_gfx_object_via_DMA(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();
	}
}

void GFX_draw_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd) {

	location_t area;
	location_t front_object_movement_area;
	location_t middle_object_movement_area;

	GFX_get_object_movement_area(front_object, &front_object_movement_area);
	GFX_get_object_movement_area(middle_object, &middle_object_movement_area);

	if (!GFX_are_locations_overlapping(&front_object_movement_area, &middle_object_movement_area)) {
		GFX_draw_one_gfx_object_on_background(front_object, bckgnd);
		GFX_draw_one_gfx_object_on_background(middle_object, bckgnd);

	} else {

		GFX_get_object_movement_area(middle_object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(middle_object);
		GFX_partial_frame_buffer_overlay_object(front_object);
		GFX_draw_gfx_object(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();
		GFX_get_object_movement_area(front_object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(middle_object);
		GFX_partial_frame_buffer_overlay_object(front_object);
		GFX_draw_gfx_object(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();

	}

}

void GFX_draw_two_gfx_objects_on_background_via_DMA(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd) {
	location_t area;
	location_t front_object_movement_area;
	location_t middle_object_movement_area;
	GFX_get_object_movement_area(front_object, &front_object_movement_area);
	GFX_get_object_movement_area(middle_object, &middle_object_movement_area);

	if (!GFX_are_locations_overlapping(&front_object_movement_area, &middle_object_movement_area)) {
		GFX_draw_one_gfx_object_on_background(front_object, bckgnd);
		GFX_draw_one_gfx_object_on_background(middle_object, bckgnd);
	} else {
		GFX_get_object_movement_area(middle_object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(middle_object);
		GFX_partial_frame_buffer_overlay_object(front_object);
		GFX_draw_gfx_object_via_DMA(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();
		GFX_get_object_movement_area(front_object, &area);
		GFX_allocate_partial_frame_buffer_for_display_area(&area);
		GFX_copy_image_part_to_partial_frame_buffer(&bckgnd->image);
		GFX_partial_frame_buffer_overlay_object(middle_object);
		GFX_partial_frame_buffer_overlay_object(front_object);
		GFX_draw_gfx_object_via_DMA(&partial_frame_buffer);
		GFX_deallocate_partial_frame_buffer();
	}
}

void GFX_clear_area_on_background(int16_t x_min, int16_t y_min, int16_t x_max, int16_t y_max, graphic_object_t *bckgnd)
{
	partial_frame_buffer.location.x_min = x_min;
	partial_frame_buffer.location.y_min = y_min;

	partial_frame_buffer.location.x_max = x_max;
	partial_frame_buffer.location.y_max = y_max;

	partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
	partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
	partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;
	partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);
	GFX_get_image_part( &bckgnd->image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);
	GFX_draw_gfx_object(&partial_frame_buffer);
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);

}


void GFX_clear_location_on_background( location_t *location, graphic_object_t *bckgnd )
{
	GFX_clear_area_on_background(location->x_min, location->y_min, location->x_max, location->y_max, bckgnd);
}

void GFX_clear_gfx_object_on_background(graphic_object_t *object, graphic_object_t *bckgnd)
{
	partial_frame_buffer.location.x_min = object->location.x_min;
	partial_frame_buffer.location.y_min = object->location.y_min;

	partial_frame_buffer.location.x_max = object->location.x_max;
	partial_frame_buffer.location.y_max = object->location.y_max;

	partial_frame_buffer.image.size_x = partial_frame_buffer.location.x_max - partial_frame_buffer.location.x_min;
	partial_frame_buffer.image.size_y = partial_frame_buffer.location.y_max - partial_frame_buffer.location.y_min;
	partial_frame_buffer.image.size = partial_frame_buffer.image.size_x * partial_frame_buffer.image.size_y;


	partial_frame_buffer.image.image_array = GFX_allocate_image_buffer(partial_frame_buffer.image.size);
	GFX_get_image_part( &bckgnd->image, partial_frame_buffer.location.x_min, partial_frame_buffer.location.y_min, partial_frame_buffer.image.size_x, partial_frame_buffer.image.size_y, partial_frame_buffer.image.image_array);
	GFX_draw_gfx_object(&partial_frame_buffer);
	GFX_deallocate_image_buffer(partial_frame_buffer.image.image_array);
}

void GFX_update_two_gfx_objects_on_background(graphic_object_t *front_object, graphic_object_t *middle_object, graphic_object_t *bckgnd)
{
	GFX_update_moving_gfx_object_location(front_object);
	GFX_update_moving_gfx_object_location(middle_object);
	GFX_draw_two_gfx_objects_on_background(front_object, middle_object, bckgnd);
}

void GFX_display_text_object(text_object_t *txt_object)
{
	UG_FontSelect(txt_object->font);
	UG_SetForecolor(txt_object->fore_color);
	UG_SetBackcolor(txt_object->back_color);
	UG_PutString(txt_object->x_min, txt_object->y_min, txt_object->text );
}



void GFX_display_progress_bar(progress_bar_t *progress_bar)
{
	UG_FillFrame( progress_bar->bar.x_min, progress_bar->bar.y_min, progress_bar->bar.x_max, progress_bar->bar.y_max, progress_bar->bar.color );
	UG_FillFrame( progress_bar->bar.x_max, progress_bar->bar.y_min, progress_bar->bar.x_min + progress_bar->full_length_px, progress_bar->bar.y_max, progress_bar->background_color );
}

void GFX_init_obstacle_pair_location(obstacle_pair_t *pair, int16_t x, int16_t top_y, int16_t bottom_y) 
{
	GFX_init_gfx_object_location(&pair->top, x, top_y);
	GFX_init_gfx_object_location(&pair->bottom, x, bottom_y);
	pair->bottom.image.size_y = DISPLAY_SIZE_Y-bottom_y;
}

void GFX_set_obstacle_pair_x_axis_velocity(obstacle_pair_t *pair, int8_t x_velocity) 
{
	GFX_set_gfx_object_velocity(&pair->top, x_velocity, 0);
	GFX_set_gfx_object_velocity(&pair->bottom, x_velocity, 0);
}

void GFX_update_obstacle_pair_location(obstacle_pair_t *pair) 
{
    GFX_update_moving_gfx_object_location(&pair->top);
    GFX_update_moving_gfx_object_location(&pair->bottom);
}

void GFX_draw_obstacle_pair_on_background(obstacle_pair_t *pair, graphic_object_t *background)
{
    GFX_draw_one_gfx_object_on_background(&pair->top, background);
    GFX_draw_one_gfx_object_on_background(&pair->bottom, background);
}

void GFX_get_obstacle_pair_movement_area(obstacle_pair_t *pair, location_t *object_movement_area)
{
	GFX_get_object_movement_area(&pair->top, object_movement_area);
	GFX_get_object_movement_area(&pair->bottom, object_movement_area);
}

void GFX_clear_obstacle_pair_on_background(obstacle_pair_t *pair, location_t *object_movement_area)
{
	GFX_clear_gfx_object_on_background(&pair->top, &background);
	GFX_clear_gfx_object_on_background(&pair->bottom, &background);
}