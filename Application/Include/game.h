#ifndef INCLUDE_GAME_H_
#define INCLUDE_GAME_H_

// ----------- Include other modules (for public) -------------


// -------------------- Public definitions --------------------
#define INTRO_DELAY_BEFORE_KBD_ACTIVE 3000
#define GAMEOVER_DELAY_BEFORE_KBD_ACTIVE 3000

// ---------------- Public function prototypes ----------------

void 		Game();
uint8_t 	Intro();
uint8_t 	GamePlay();
uint8_t 	GameOver();

void Flash_Write_UInt16(uint32_t address, uint16_t data);
uint16_t Flash_Read_UInt16(uint32_t address);

void Update_High_Scores(uint16_t new_score);
uint16_t* Get_High_Scores(void);
void Init_High_Scores(void);
#define HIGH_SCORES_BASE_ADDRESS 0x0807F800
#define NUM_HIGH_SCORES 3


#endif /* INCLUDE_GAME_H_ */
