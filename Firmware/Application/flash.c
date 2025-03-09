#include "flash.h"
#include "game.h"
#include "stdio.h"


// Funkcija za branje iz flash pomnilnika
uint16_t Flash_Read_UInt16(uint32_t address) {
    uint64_t value = *(uint64_t*)address;
    return (uint16_t)(value & 0xFFFF);
}

// Funkcija za pisanje v flash pomnilnik
void Update_High_Scores(uint16_t new_score) {
    uint16_t scores[NUM_HIGH_SCORES];
    
    for(int i = 0; i < NUM_HIGH_SCORES; i++) {
        scores[i] = Flash_Read_UInt16(HIGH_SCORES_BASE_ADDRESS + (i * 8));
        if(scores[i] == 0xFFFF) scores[i] = 0;  
    }
    
    int inserted = 0;
    for(int i = 0; i < NUM_HIGH_SCORES; i++) {
        if(new_score > scores[i] && !inserted) {
            for(int j = NUM_HIGH_SCORES - 1; j > i; j--) {
                scores[j] = scores[j-1];
            }
            scores[i] = new_score;
            inserted = 1;
            break;
        }
    }
    
    if(inserted) {
        HAL_FLASH_Unlock();
        FLASH_EraseInitTypeDef eraseInitStruct;
        uint32_t pageError;
        eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseInitStruct.Page = (HIGH_SCORES_BASE_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;
        eraseInitStruct.NbPages = 1;
        eraseInitStruct.Banks = FLASH_BANK_2;
        
        if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) == HAL_OK) {
            for(int i = 0; i < NUM_HIGH_SCORES; i++) {
                uint64_t value = 0xFFFFFFFFFFFFFFFFULL;
                value = (value & ~0xFFFFULL) | scores[i];
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                                HIGH_SCORES_BASE_ADDRESS + (i * 8), 
                                value);
            }
        }
        HAL_FLASH_Lock();
    }
}

uint16_t* Get_High_Scores(void) {
    static uint16_t scores[NUM_HIGH_SCORES];
    
    for(int i = 0; i < NUM_HIGH_SCORES; i++) {
        scores[i] = Flash_Read_UInt16(HIGH_SCORES_BASE_ADDRESS + (i * 8));
    }
    
    return scores;
}
