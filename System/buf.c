// ----------- Include other modules (for private) -------------
#include "buf.h"

// -------------- Public function implementations --------------

void BUF_init(buf_handle_t *buf_handle, uint8_t *buffer_ptr, uint32_t buf_length)
{
	buf_handle->buffer = buffer_ptr;
	buf_handle->length = buf_length;

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

buf_rtrn_codes_t BUF_store_byte(buf_handle_t *buf_handle, uint8_t data)
{
	if (buf_handle->data_size >= buf_handle->length)
	{
		return BUFFER_FULL;	
	}
	else
	{
		if (buf_handle->data_size == 0)
		{
			buf_handle->front = 0;
			buf_handle->rear = 0;
		}
		else
		{
			buf_handle->rear++;

			if (buf_handle->rear >= buf_handle->length)
			{
				buf_handle->rear = 0;
			}
		}

		buf_handle->buffer[buf_handle->rear] = data;

		buf_handle->data_size++;
		buf_handle->free_size--;

		return BUFFER_OK;
	}

}

buf_rtrn_codes_t BUF_store_bytes(buf_handle_t *buf_handle, uint8_t *data, uint32_t size)
{
	buf_rtrn_codes_t rtrn_code = BUFFER_OK;

	if (buf_handle->free_size < size)
	{
		return BUFFER_NOT_ENOUGH_SPACE;
	}
	else
	{
		for(uint32_t i=0; i<size; i++)
		{
			rtrn_code = BUF_store_byte(buf_handle, data[i]);

			if (rtrn_code != BUFFER_OK)
				return rtrn_code;
		}

		return BUFFER_OK;
	}
}

buf_rtrn_codes_t BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data)
{
	if (buf_handle->data_size == 0)
	{
		return BUFFER_EMPTY;	
	}

	*data = buf_handle->buffer[buf_handle->front];	
	buf_handle->data_size--;
	buf_handle->free_size++;

	buf_handle->front++;

	if (buf_handle->front >= buf_handle->length)
	{
		buf_handle->front = 0;
	}

	return BUFFER_OK;	
}

buf_rtrn_codes_t BUF_get_bytes(buf_handle_t *buf_handle, uint8_t *data,  uint32_t size)
{
	uint32_t buf_data_size;
	buf_rtrn_codes_t	rtnr_code;

	buf_data_size = buf_handle->data_size;	
	if (buf_data_size >= size)
	{
		for(int i=0; i<buf_data_size; i++ )
		{
			rtnr_code = BUF_get_byte( buf_handle, &data[i] );

			if ( rtnr_code != BUFFER_OK )
				 return rtnr_code;
		}

		return BUFFER_OK;

	}
	else
		return BUFFER_NOT_ENOUGH_DATA;

}

uint32_t BUF_get_data_size(buf_handle_t *buf_handle)
{
	return buf_handle->data_size;
}

uint32_t BUF_get_free_size(buf_handle_t *buf_handle)
{
	return buf_handle->free_size;
}

// ------- Test functions ---------
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
