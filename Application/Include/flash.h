#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <stdint.h>

// Function prototypes
void Flash_Write_UInt16(uint32_t address, uint16_t data);
uint16_t Flash_Read_UInt16(uint32_t address);

#endif // FLASH_STORAGE_H