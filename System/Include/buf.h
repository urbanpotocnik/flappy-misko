#ifndef INCLUDE_BUF_H_
#define INCLUDE_BUF_H_


// ----------- Include other modules (for public) -------------
#include "stdint.h"

// -------------------- Public definitions --------------------
typedef struct
{
	uint8_t *buffer;		
	uint32_t length;		
	uint32_t rear;			
	uint32_t front;			
	uint32_t data_size;		
	uint32_t free_size;		

} buf_handle_t;

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

#endif 
