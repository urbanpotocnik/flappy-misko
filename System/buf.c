/*
 * buf.c
 *
 *  Created on: 
 *      Author: 
 */

/* **************** MODULE DESCRIPTION *************************

The buffer module is intended to help you use any array as a circular buffer.
You must therefore provide an array that will be treated as a buffer and a
buffer handle data structure where all the required parameters regarding
the buffer are kept.

************************************************************* */




// ----------- Include other modules (for private) -------------

#include "buf.h"


// ---------------------- Private definitions ------------------





// -------------- Public function implementations --------------

void BUF_init(buf_handle_t *buf_handle, uint8_t *buffer_ptr, uint32_t buf_length)
{
	// Initialize the buffer handle according to given parameters.
	buf_handle->buffer = buffer_ptr;
	buf_handle->length = buf_length;


	// And flush the buffer.
	BUF_flush(buf_handle);
}


buf_rtrn_codes_t BUF_flush(buf_handle_t *buf_handle)
{
	buf_handle->front = 0;
	buf_handle->rear = 0;
	buf_handle->data_size = 0;
	buf_handle->free_size = buf_handle->length;

	return BUFFER_OK;
}




// Store one byte of data.
buf_rtrn_codes_t BUF_store_byte(buf_handle_t *buf_handle, uint8_t data)
{
	// check if buffer already full
	if (buf_handle->data_size >= buf_handle->length)
	{
		return BUFFER_FULL;	//-> if so, return error
	}
	else
	{
		// check special case: buffer empty
		if (buf_handle->data_size == 0)
		{
			buf_handle->front = 0;
			buf_handle->rear = 0;
		}
		else
		{
			// increase rear pointer and apply modular arithmetics
			buf_handle->rear++;

			if (buf_handle->rear >= buf_handle->length)
			{
				buf_handle->rear = 0;
			}
		}


		// by now rear pointer is ready for new data -> store data
		buf_handle->buffer[buf_handle->rear] = data;

		// update data size and free size
		buf_handle->data_size++;
		buf_handle->free_size--;

		return BUFFER_OK;
	}

}


buf_rtrn_codes_t BUF_store_bytes(buf_handle_t *buf_handle, uint8_t *data, uint32_t size)
{
	buf_rtrn_codes_t rtrn_code = BUFFER_OK;

	// First, check if there is even enough free space to store new data.
	if (buf_handle->free_size < size)
	{
		return BUFFER_NOT_ENOUGH_SPACE;
	}
	else
	{
		// store the data cluster in the buffer
		for(uint32_t i=0; i<size; i++)
		{
			// store i-th byte
			rtrn_code = BUF_store_byte(buf_handle, data[i]);

			// if the return code not OK, break the operation with the error code
			if (rtrn_code != BUFFER_OK)
				return rtrn_code;
		}

		// if all data store successfully, return OK
		return BUFFER_OK;
	}
}







// Read one byte of data and store it to the given location.
buf_rtrn_codes_t BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data)
{
	// first check if buffer empty
	if (buf_handle->data_size == 0)
	{
		return BUFFER_EMPTY;	// return error code and exit this function
	}

	// else read and return data from the buffer "front"
	*data = buf_handle->buffer[buf_handle->front];	//Get data from the queue "front"

	// update data size and free size
	buf_handle->data_size--;
	buf_handle->free_size++;


	// update the "front" value NOT using the modulo arithmetics but
	// using if-then instead because it is faster on uC
	buf_handle->front++;

	if (buf_handle->front >= buf_handle->length)
	{
		buf_handle->front = 0;
	}

	return BUFFER_OK;	// return BUFFER_OK error code

}


// Read the specified number of bytes (size) from the buffer and store it to the &data location.
buf_rtrn_codes_t BUF_get_bytes(buf_handle_t *buf_handle, uint8_t *data,  uint32_t size)
{
	uint32_t buf_data_size;
	buf_rtrn_codes_t	rtnr_code;

	// check if enough bytes available in the buffer
	buf_data_size = buf_handle->data_size;	// remember the initial buffer data size
	if (buf_data_size >= size)
	{
		// get all the data byte by byte and store in the &data location
		for(int i=0; i<buf_data_size; i++ )
		{
			// get one byte and remember the return code
			rtnr_code = BUF_get_byte( buf_handle, &data[i] );

			// if something went wrong reading this byte
			if ( rtnr_code != BUFFER_OK )
				 return rtnr_code;
		}

		// if we are here, everything went OK
		return BUFFER_OK;

	}
	// if not enough bytes available, return error
	else
		return BUFFER_NOT_ENOUGH_DATA;


}



// Get the number of bytes currently in the buffer queue.
uint32_t BUF_get_data_size(buf_handle_t *buf_handle)
{
	return buf_handle->data_size;
}


// Get the size of the free space in the buffer in bytes.
uint32_t BUF_get_free_size(buf_handle_t *buf_handle)
{
	return buf_handle->free_size;
}







// ------- Test functions ---------


// A simple buffer demonstration for debug stepping.
void BUF_demo(void)
{
	#define TEST_BUF_LEN 	4

	char test_string[16] = "test";

	uint8_t data = 0;
	uint8_t test_buffer[TEST_BUF_LEN];
	buf_handle_t test_buf_handle;

	BUF_init( &test_buf_handle, test_buffer, TEST_BUF_LEN);


	BUF_store_byte( &test_buf_handle, 1);
	BUF_store_byte( &test_buf_handle, 2);
	BUF_store_byte( &test_buf_handle, 3);
	BUF_store_byte( &test_buf_handle, 4);


	BUF_get_byte(&test_buf_handle, &data);
	BUF_get_byte(&test_buf_handle, &data);
	BUF_get_byte(&test_buf_handle, &data);

	BUF_store_byte( &test_buf_handle, 5);
	BUF_store_byte( &test_buf_handle, 6);

	BUF_get_byte(&test_buf_handle, &data);
	BUF_get_byte(&test_buf_handle, &data);
	BUF_get_byte(&test_buf_handle, &data);



	BUF_flush(&test_buf_handle);

	BUF_store_bytes(&test_buf_handle, (uint8_t*)test_string, 4);



}




// -------------- Private function implementations -------------
