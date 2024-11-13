/*
 * buf.h
 *
 *  Created on: 
 *      Author:
 */

#ifndef INCLUDE_BUF_H_
#define INCLUDE_BUF_H_


// ----------- Include other modules (for public) -------------

// Include the stdint.h library to get integer number definitions.
#include "stdint.h"




// -------------------- Public definitions --------------------


// Define a cyclic buffer handle that is used to operate an individual instance of the buffer.
typedef struct
{
	uint8_t *buffer;		// A pointer to the beginning of the buffer - i.e. the array of bytes.
	uint32_t length;		// Variable containing the buffer length.
	uint32_t rear;			// Pointer (index) to the end of the queue -> for writing into the buffer.
	uint32_t front;			// Pointer (index) to the beginning of the queue -> for reading out of the buffer.
	uint32_t data_size;		// Data size = Number of bytes currently stored in the queue/buffer.
	uint32_t free_size;		// Number of bytes still free in the buffer.

} buf_handle_t;


// Define the possible buffer return codes that are used to give information if the buffer functions
// were successful or not and why not.
typedef enum { BUFFER_OK, BUFFER_FULL, BUFFER_EMPTY, BUFFER_NOT_ENOUGH_SPACE, BUFFER_NOT_ENOUGH_DATA  } buf_rtrn_codes_t;



// ---------------- Public function prototypes ----------------

void BUF_init(buf_handle_t *buf_handle, uint8_t *buffer_ptr, uint32_t buf_length);


buf_rtrn_codes_t BUF_flush(buf_handle_t *buf_handle);

buf_rtrn_codes_t BUF_store_byte(buf_handle_t *buf_handle, uint8_t data);
buf_rtrn_codes_t BUF_store_bytes(buf_handle_t *buf_handle, uint8_t *data, uint32_t size);

buf_rtrn_codes_t BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data);
buf_rtrn_codes_t BUF_get_bytes(buf_handle_t *buf_handle, uint8_t *data,  uint32_t size);


uint32_t BUF_get_data_size(buf_handle_t *buf_handle);
uint32_t BUF_get_free_size(buf_handle_t *buf_handle);


void BUF_demo(void);




#endif /* INCLUDE_BUF_H_ */
