#include "flash.h"
#include "stm32g4xx_hal.h"  

// Funkcija za pisanje v flash
// Prvih 16 bitov je nastavljenih v zacetnem delu, ostalo nastavimo na FFFF
void Flash_Write_UInt16(uint32_t address, uint16_t data) {
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError;
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.Page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    eraseInitStruct.NbPages = 1;
    eraseInitStruct.Banks = FLASH_BANK_1; 

    if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return;
    }

    
    uint64_t value = 0xFFFFFFFFFFFFFFFFULL;
    value = (value & ~0xFFFFULL) | data;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, value);

    HAL_FLASH_Lock();
}

// Funkcija za branje iz flash pomnilnika
// Prebere prvi del double worda
uint16_t Flash_Read_UInt16(uint32_t address) {
    uint64_t value = *(uint64_t*)address;
    return (uint16_t)(value & 0xFFFF);
}