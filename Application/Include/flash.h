#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

#define HIGH_SCORES_BASE_ADDRESS 0x0807F800  
#define NUM_HIGH_SCORES 3                    

uint16_t Flash_Read_UInt16(uint32_t address);
void Update_High_Scores(uint16_t new_score);
uint16_t* Get_High_Scores(void);

#endif /* FLASH_H_ */