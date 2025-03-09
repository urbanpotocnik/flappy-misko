// ----------- Include other modules (for private) -------------
#include "SCI.h"
#include <stdio.h>	
#include "buf.h"		


// ---------------------- Private definitions ------------------
typedef struct
{
	USART_TypeDef *		USART;

} SCI_handle_t;

SCI_handle_t SCI;
#define SCI_RX_BUF_LEN 	512
uint8_t SCI_RX_buffer[SCI_RX_BUF_LEN];			
buf_handle_t SCI_RX_buf_handle;			
#define SCI_TX_BUF_LEN 	512
uint8_t SCI_TX_buffer[SCI_TX_BUF_LEN];			
buf_handle_t SCI_TX_buf_handle;			
int _write(int file, char *ptr, int len);

// -------------- Public function implementations --------------
void SCI_init(void)
{

	SCI.USART = USART3;
	setvbuf(stdout, NULL, _IONBF, 0);

	BUF_init( &SCI_RX_buf_handle, SCI_RX_buffer, SCI_RX_BUF_LEN);	
	BUF_init( &SCI_TX_buf_handle, SCI_TX_buffer, SCI_TX_BUF_LEN);	

	LL_USART_EnableIT_RXNE_RXFNE (SCI.USART);
}

void SCI_send_char(char c)
{

	while( ! LL_USART_IsActiveFlag_TXE_TXFNF (SCI.USART) )
	{
	}

	LL_USART_TransmitData8 (SCI.USART, c);
}

void SCI_send_byte(uint8_t data)
{
	while( ! (LL_USART_IsActiveFlag_TXE_TXFNF (SCI.USART)))
	{
	}

	LL_USART_TransmitData8 (SCI.USART,data);
}

void SCI_send_string(char *str)
{
	uint32_t i = 0;
	while( str[i] != '\0' )
	{
		SCI_send_char(str[i]);
		i++;
	}
}

void SCI_send_bytes(uint8_t *data, uint32_t size)
{
	for( int i = 0 ; i < size ; i++ )
	{
		SCI_send_byte(data[i]);
	}
}

uint8_t SCI_is_data_waiting(void)
{
	return (uint8_t) LL_USART_IsActiveFlag_RXNE_RXFNE (SCI.USART);
}

SCI_rtrn_codes_t SCI_read_char(char *c)
{
	if ( SCI_is_data_waiting() )
	{
		*c = LL_USART_ReceiveData8 (SCI.USART);
		return SCI_NO_ERROR;
	}
	else
	{
		return SCI_ERROR;
	}
}

SCI_rtrn_codes_t SCI_read_byte(uint8_t *data)
{
	if ( SCI_is_data_waiting() )
	{
		*data = LL_USART_ReceiveData8 (SCI.USART);
		return SCI_NO_ERROR;

	}
	else
	{
		return SCI_ERROR;
	}
}

// -------------- Private function implementations -------------
int _write(int file, char *ptr, int len)
{
	SCI_send_bytes( (uint8_t*) ptr, (uint32_t) len );
	return len;
}

// ------- Testne funkcije ---------
void SCI_demo_Hello_world(void)
{
	SCI_send_string("Hello world!");
}

void SCI_demo_Hello_world_printf(void)
{
	for(int i=0; i<10 ; i++)
	{
		printf("%d : Hello printf() world!\n",i+1);
		HAL_Delay(1000);
	}
}

void SCI_demo_echo_with_polling(void)
{
	char c;
	float a_0 = 0.5;
	float a_n;
	float sum = 0;

	#define N_MAX		50

	while(1)
	{
		if ( SCI_read_char(&c)  == SCI_NO_ERROR )
		{
			SCI_send_string(&c);
		}

		a_n = a_0;
		for(uint32_t n=0; n<N_MAX; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}
	}
}

void SCI_send_string_IT(char *str)
{
	uint32_t i = 0;		
	uint8_t c;			

	while( str[i] != '\0')
	{
		BUF_store_byte( &SCI_TX_buf_handle, (uint8_t) str[i] );
		i++;
	}

	if ( i > 0 )
	{
		if ( LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) )		
		{
			BUF_get_byte( &SCI_TX_buf_handle, &c );
			LL_USART_TransmitData8(SCI.USART, c);
		}
		LL_USART_EnableIT_TXE_TXFNF (SCI.USART);
	}
}

void SCI_send_bytes_IT(uint8_t *data, uint32_t size)
{
	uint8_t	d;

	for(uint32_t i=0; i < size ; i++ )
	{
		BUF_store_byte( &SCI_RX_buf_handle, (uint8_t) data[i] );
	}

	if ( size > 0 )
	{
		if ( LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) )		
		{
			BUF_get_byte( &SCI_TX_buf_handle, &d );
			LL_USART_TransmitData8(SCI.USART, d);
		}

		LL_USART_EnableIT_TXE_TXFNF (SCI.USART);
	}
}

void SCI_receive_char_Callback(void)
{
	uint8_t received_data;
	received_data = LL_USART_ReceiveData8 (SCI.USART);
	BUF_store_byte( &SCI_RX_buf_handle, received_data );
}

void SCI_transmit_char_Callback(void)
{
	uint8_t 			data_to_transmit;	
	buf_rtrn_codes_t	return_code;			

	return_code = BUF_get_byte( &SCI_TX_buf_handle, &data_to_transmit );

	if ( return_code == BUFFER_OK )
	{
		LL_USART_TransmitData8 (SCI.USART, data_to_transmit);
	}

	if ( BUF_get_data_size( &SCI_TX_buf_handle ) == 0)
	{
		LL_USART_EnableIT_TXE_TXFNF (SCI.USART);
	}

}

void SCI_demo_receive_with_interrupts(void)
{
	uint8_t data;	

	while(1)
	{
		if ( BUF_get_data_size( &SCI_RX_buf_handle) )
		{
			BUF_get_byte( &SCI_RX_buf_handle, &data );
			SCI_send_byte(data);
		}
	}
}

void SCI_demo_transmit_with_interrupts(void)
{
	char message[100] = "This was sent using USART interrupts!\n";

	while(1)
	{
		SCI_send_string(message);
		HAL_Delay(1000);
	}
}

void SCI_demo_echo_with_interrupts(void)
{

	#define N_MAX_IT		500
	float a_0 = 0.5;
	float a_n;
	float sum = 0;
	uint8_t message[SCI_RX_BUF_LEN];
	uint32_t message_size;

	while(1)
	{
		message_size = BUF_get_data_size( &SCI_RX_buf_handle);

		if ( message_size > 0 )
		{
			BUF_get_bytes( &SCI_RX_buf_handle, message, message_size );
			SCI_send_bytes(message, message_size);
		}
		a_n = a_0;
		for(uint32_t n=0; n<N_MAX_IT; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}
	}
}








