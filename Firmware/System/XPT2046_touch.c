/*
 * XPT2046_touch.c
 *
 */


#include "XPT2046_touch.h"

#include <stdio.h>
#include <stdlib.h>



extern SPI_HandleTypeDef 	XPT2046_SPI_PORT;	// specify the SPI handle structure




#if defined (XPT2046_ORIENTATION_LANDSCAPE) || defined(XPT2046_ORIENTATION_LANDSCAPE_ROT180)

#define READ_X 0x90
#define READ_Y 0xD0

#else

#define READ_X 0xD0
#define READ_Y 0x90

#endif


static void XPT2046_TouchSelect()
{
	WRITE_REG(XPT2046_CS_GPIO_Port->BRR, XPT2046_CS_Pin);
}


static void XPT2046_TouchUnselect()
{
	WRITE_REG(XPT2046_CS_GPIO_Port->BSRR, XPT2046_CS_Pin);
}


bool XPT2046_touch_pressed()
{
    return READ_BIT(XPT2046_IRQ_GPIO_Port->IDR, XPT2046_IRQ_Pin) == 0;
}




void XPT2046_touch_get_coordinates(uint16_t* x, uint16_t* y)
{
	#define RETURN_RAW_COORDINATES  0

	static const uint8_t cmd_read_x[] = { READ_X };
    static const uint8_t cmd_read_y[] = { READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };

    XPT2046_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;


    for(uint8_t i = 0; i < XPT2046_AVERAGING_LENGTH; i++)
    {


        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        uint8_t y_raw[2];

        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        HAL_SPI_Transmit(&XPT2046_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        uint8_t x_raw[2];

        HAL_SPI_TransmitReceive(&XPT2046_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    XPT2046_TouchUnselect();


    uint32_t raw_x = (avg_x / XPT2046_AVERAGING_LENGTH);
    uint32_t raw_y = (avg_y / XPT2046_AVERAGING_LENGTH);

	#ifdef XPT2046_ORIENTATION_LANDSCAPE_ROT180
		raw_y=32768-raw_y;
	#endif

	#ifdef XPT2046_ORIENTATION_PORTRATE_ROT180
		raw_x=32768-raw_x;
	#endif

    if ( RETURN_RAW_COORDINATES  )
    {
    	*x = (uint16_t) raw_x;
		*y = (uint16_t) raw_y;
    }
    else
    {
    	if(raw_x < XPT2046_MIN_RAW_X) raw_x = XPT2046_MIN_RAW_X;
   	    if(raw_x > XPT2046_MAX_RAW_X) raw_x = XPT2046_MAX_RAW_X;

   	    if(raw_y < XPT2046_MIN_RAW_Y) raw_y = XPT2046_MIN_RAW_Y;
   	    if(raw_y > XPT2046_MAX_RAW_Y) raw_y = XPT2046_MAX_RAW_Y;

   	    *x = (raw_x - XPT2046_MIN_RAW_X) * XPT2046_SCALE_X / (XPT2046_MAX_RAW_X - XPT2046_MIN_RAW_X);
  	    *y = (raw_y - XPT2046_MIN_RAW_Y) * XPT2046_SCALE_Y / (XPT2046_MAX_RAW_Y - XPT2046_MIN_RAW_Y);
    }

}






bool XPT2046_touch_get_coordinates_if_pressed(uint16_t* x, uint16_t* y)
{
	if( XPT2046_touch_pressed() )
	{
		XPT2046_touch_get_coordinates( x, y );
		return true;
	}
	else
	{
		return false;
	}
}
