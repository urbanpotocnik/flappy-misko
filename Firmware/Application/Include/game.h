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

#endif /* INCLUDE_GAME_H_ */
