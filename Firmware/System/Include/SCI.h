#ifndef INCLUDE_SCI_H_
#define INCLUDE_SCI_H_

// ----------- Include other modules (for public) -------------
#include "stm32g4xx_ll_usart.h"

// -------------------- Public definitions --------------------
typedef enum { SCI_ERROR=-1, SCI_NO_ERROR=0} SCI_rtrn_codes_t;

void SCI_send_string_IT(char *str);
void SCI_send_bytes_IT(uint8_t *data, uint32_t size);

void SCI_receive_char_Callback(void);
void SCI_transmit_char_Callback(void);

// ---------------- Public function prototypes ----------------
void SCI_init(void);
void SCI_send_char(char c);
void SCI_send_byte(uint8_t data);
uint8_t SCI_is_data_waiting(void);
SCI_rtrn_codes_t SCI_read_char(char *c);
SCI_rtrn_codes_t SCI_read_byte(uint8_t *data);
void SCI_send_string(char *str);
void SCI_send_bytes(uint8_t *data, uint32_t size);
void SCI_demo_Hello_world(void);
void SCI_demo_echo_with_polling(void);
void SCI_demo_Hello_world_printf(void);
void SCI_demo_recive_with_interrupts(void);
void SCI_demo_transmit_with_interrupts(void);
void SCI_demo_echo_with_interrupts(void);

#endif 